/***************************************************
  Adafruit MQTT Library FONA Example

  Designed specifically to work with the Adafruit FONA
  ----> http://www.adafruit.com/products/1946
  ----> http://www.adafruit.com/products/1963
  ----> http://www.adafruit.com/products/2468
  ----> http://www.adafruit.com/products/2542

  These cellular modules use TTL Serial to communicate, 2 pins are
  required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_SleepyDog.h>
//#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_FONA.h"

#include <Arduino.h>
#include "wiring_private.h"
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#define LED_PIN 13

RTC_DS1307 rtc;
/*************************** FONA Pins ***********************************/

// Default pins for Feather 32u4 FONA
//#define FONA_RX  9
//#define FONA_TX  8
#define FONA_RST A5
#define FONA_KEY A4
#define FONA_PS A3
//SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

/************************* WiFi Access Point *********************************/

  // Optionally configure a GPRS APN, username, and password.
  // You might need to do this to access your network's GPRS/data
  // network.  Contact your provider for the exact APN, username,
  // and password values.  Username and password are optional and
  // can be removed, but APN is required.
#define FONA_APN       "xlunlimited"
#define FONA_USERNAME  ""
#define FONA_PASSWORD  ""

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "hariantow"
#define AIO_KEY         "37dd1a3c98214ea1963f7e0c9967f22d"

/************ Global State (you don't need to change this!) ******************/

// Setup the FONA MQTT class by passing in the FONA class and MQTT server and login details.
Adafruit_MQTT_FONA mqtt(&fona, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// You don't need to change anything below this line!
#define halt(s) { Serial.println(F( s )); while(1);  }

// FONAconnect is a helper function that sets up the FONA and connects to
// the GPRS network. See the fonahelper.cpp tab above for the source!
boolean FONAconnect(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");
Adafruit_MQTT_Publish pressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pressure");

// Setup a feed called 'onoff' for subscribing to changes.
//Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

/*************************** Sketch Code ************************************/

// How many transmission failures in a row we're willing to be ok with before reset
uint8_t txfailures = 0;
#define MAXTXFAILURES 10

Uart Serial2 (&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);

void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}

void setup() {
  //while (!Serial);

  // Watchdog is optional!
  //Watchdog.enable(8000);

  pinMode(LED_PIN, OUTPUT);  
  pinMode(FONA_KEY, OUTPUT);

  Serial.begin(9600);

  Serial.println(F("Adafruit FONA MQTT demo"));
  
  Serial2.begin(9600);
  
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }

   if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    Serial2.write('s'); // s = sleep for Slave
    digitalWrite(FONA_KEY, HIGH);
    delay(100);
    digitalWrite(FONA_KEY, LOW);
    delay(2000);
  }
//  mqtt.subscribe(&onoffbutton);
  
  Watchdog.reset();
  delay(5000);  // wait a few seconds to stabilize connection
  Watchdog.reset();
  
  // Initialise the FONA module

  Watchdog.reset();
  delay(5000);  // wait a few seconds to stabilize connection
  Watchdog.reset();
}

float pressureData;
int tempTime = 0;
int waitTime = 30;
int timeCounter = waitTime;
bool uploadOK;

void loop() {
  // Make sure to reset watchdog every loop iteration!
  Watchdog.reset();

  digitalWrite(LED_PIN, LOW);
  
  DateTime now = rtc.now();
  
  tempTime = now.second()-tempTime;
  if(tempTime<0)tempTime = 1;
  timeCounter += tempTime;
  tempTime = now.second();
  Serial.println(timeCounter);
  if(timeCounter > waitTime){

    digitalWrite(LED_PIN, HIGH);
    
    Serial2.write('+'); //send anything to wake Slave up
    
    digitalWrite(FONA_KEY, HIGH);
    delay(100);
    digitalWrite(FONA_KEY, LOW);
    delay(2000);

      while (! FONAconnect(F(FONA_APN), F(FONA_USERNAME), F(FONA_PASSWORD))) {
      Serial.println("Retrying FONA");
    }

  Serial.println(F("Connected to Cellular!"));
    uploadOK = false;
    // Ensure the connection to the MQTT server is alive (this will make the first
    // connection and automatically reconnect when disconnected).  See the MQTT_connect
    // function definition further below.

    while(uploadOK == false){
      MQTT_connect();
  
      Watchdog.reset();
    
      // Now we can publish stuff!
      pressureData = getPressure();
      Serial.print(F("\nWater Pressure val: "));
      Serial.println(pressureData);
      if(pressureData!= -1){
        if (! pressure.publish(pressureData)) {
          Serial.println(F("Failed"));
          txfailures++;
          if(txfailures >=5){
            txfailures = 0;
            uploadOK=true;
          }
        } else {
          Serial.println(F("OK!"));
          txfailures = 0;
          uploadOK=true;
        }
      }else{
        txfailures++;
        if(txfailures >=5){
          txfailures = 0;
          uploadOK=true;
        }
      }
    }
    timeCounter=0;
    tempTime = now.second();

    digitalWrite(LED_PIN, LOW);

    Serial2.write('s'); // s = sleep for Slave
    
    digitalWrite(FONA_KEY, HIGH);
    delay(100);
    digitalWrite(FONA_KEY, LOW);
    delay(2000);
  
  }
  

  Watchdog.reset();  
  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
//    if (subscription == &onoffbutton) {
//      Serial.print(F("Got: "));
//      Serial.println((char *)onoffbutton.lastread);
//    }
  }

  // ping the server to keep the mqtt connection alive, only needed if we're not publishing
  //if(! mqtt.ping()) {
  //  Serial.println(F("MQTT Ping failed."));
  //}

}

float getPressure(){
  int count = 10;
  char dataRead;
  int total = 0;
  float average;
  float voltage;
  float pressure_bar;
  float pressure_psi;
  while(!Serial2.available()){
    Serial2.write('p');
  }
  dataRead = Serial2.read();
  total = dataRead - '0';
  while(Serial2.available()){
    dataRead = Serial2.read();
    if(dataRead == '.')break;
    if(dataRead < '0' || dataRead > '9')continue;
    total *= 10;
    total += dataRead -'0';
  }
  Serial.print ("Total RAW: ");
  Serial.println(total);
  average = (float) total /  count;
  Serial.print("Average RAW: ");
  Serial.println(average);
  voltage = average * 5.0 / 1024.0;
  Serial.print("Voltage (volt): ");
  Serial.println(voltage);
  if(voltage<0.5 || voltage >4.5)return -1;
  pressure_bar = (voltage - 0.5) / 4.0 * 12.0;
  Serial.print("Pressure (bar): ");
  Serial.println(pressure_bar);
  pressure_psi = pressure_bar * 14.503773773020923;
  Serial.print("Pressure (psi): ");
  Serial.println(pressure_psi);

  return pressure_bar;
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}
