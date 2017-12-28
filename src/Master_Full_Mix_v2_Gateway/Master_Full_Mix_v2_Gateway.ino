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

//#include <Arduino.h>
//#include "wiring_private.h"
//#include <Adafruit_ADS1015.h>
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#define LED_PIN 13
//#define BAT_PIN A7

#define VNH_A   A3 //A0
#define VNH_CS  A2
#define M0      3 //A1
#define M1      2 //A2
#define AUX     4 //A3

#define MAX_TX_SIZE 58
#include "Queue.h":
Queue<char> queue(MAX_TX_SIZE * 3);

RTC_DS1307 rtc;
/*************************** FONA Pins ***********************************/

// Default pins for Feather 32u4 FONA
#define FONA_RST A15 //A4

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

/************************* WiFi Access Point *********************************/

  // Optionally configure a GPRS APN, username, and password.
  // You might need to do this to access your network's GPRS/data
  // network.  Contact your provider for the exact APN, username,
  // and password values.  Username and password are optional and
  // can be removed, but APN is required.
#define FONA_APN       "FONANet"
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
Adafruit_MQTT_Publish pressure_bar = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pressure-bar");
Adafruit_MQTT_Publish pressure_psi = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pressure-psi");
Adafruit_MQTT_Publish batVoltage = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/batvoltage");

/*************************** Sketch Code ************************************/

// How many transmission failures in a row we're willing to be ok with before reset
uint8_t txfailures = 0;
#define MAXTXFAILURES 10

//Uart Serial2 (&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
//Uart Serial3 (&sercom2, 5, 4, SERCOM_RX_PAD_3, UART_TX_PAD_0);
extern Adafruit_FONA fona;

HardwareSerial *fonaSerial = &Serial1; //&Serial2;

//void SERCOM1_Handler()
//{
//  Serial2.IrqHandler();
//}

//void SERCOM2_Handler()
//{
//  Serial3.IrqHandler();
//}

//Adafruit_ADS1115 ads(0x48);
float Voltage = 0.0;
const int avgSamples = 1; //DISABLED

float sensorValue = 0;

float sensitivity = 100.0 / 500.0; //100mA per 500mV = 0.2
float Vref = 2500; // Output voltage with no current: ~ 2500mV or 2.5V
void setup() {

  //ads.begin();
  
  pinMode(LED_PIN, OUTPUT);  

  pinMode(VNH_A, OUTPUT);
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(AUX, INPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(9600);

  Serial.println(F("Adafruit FONA MQTT demo"));
  
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);

  digitalWrite(VNH_A, LOW);
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);
  
  //pinPeripheral(10, PIO_SERCOM);
  //pinPeripheral(11, PIO_SERCOM);
  //pinPeripheral(5, PIO_SERCOM);

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

  }
  
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
int waitTime = 0;//30;
int timeCounter = waitTime;
bool uploadOK;
float batVoltData;
float pressure_barData;
float pressure_psiData;
uint32_t batVoltDataInt;
uint32_t pressure_barDataInt;
uint32_t pressure_psiDataInt;
  
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
    
    //Serial2.write('+'); //send anything to wake Slave up
    
//    digitalWrite(FONA_KEY, HIGH);
//    delay(100);
//    digitalWrite(FONA_KEY, LOW);
//    delay(2000);

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
      //batVoltData = getVoltage();
      //batVoltData *= 1000;
      //batVoltDataInt = (int) batVoltData;
      //if (! batVoltage.publish(batVoltDataInt)) {
      //  Serial.println(F("Failed"));
      // }
      pressureData = getPressure();
      Serial.print(F("\nWater Pressure val: "));
      Serial.println(pressureData);
      if(pressureData!= -1){
        pressure_barData = ((pressureData - 0.5) / 4.0) * 12.0;
        pressure_barData *= 1000;
        pressure_barDataInt = (int) pressure_barData;
        Serial.print("Pressure (bar): ");
        Serial.println(pressure_barData);
        
        pressure_psiData = pressure_barData * 14.503773773020923;
        pressure_psiDataInt = (int) pressure_psiData;
        Serial.print("Pressure (psi): ");
        Serial.println(pressure_psiData);
  
        if (! pressure_bar.publish(pressure_barDataInt) ) {
          Serial.println(F("Failed"));
          txfailures++;
          if(txfailures >=10){
            txfailures = 0;
            uploadOK=true;
          }
        } else {
          pressure_psi.publish(pressure_psiDataInt);
          Serial.println(F("OK!"));
          txfailures = 0;
          uploadOK=true;
        }
      }else{
        txfailures++;
        if(txfailures >=10){
          txfailures = 0;
          uploadOK=true;
        }
      }
    }
    
    
    
    timeCounter=0;
    tempTime = now.second();

    digitalWrite(LED_PIN, LOW);

    //Serial2.write('s'); // s = sleep for Slave
    
//    digitalWrite(FONA_KEY, HIGH);
//    delay(100);
//    digitalWrite(FONA_KEY, LOW);
//    delay(2000);
  
  }
  

  Watchdog.reset();  
  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
//  while ((subscription = mqtt.readSubscription(5000))) {
//    if (subscription == &onoffbutton) {
//      Serial.print(F("Got: "));
//      Serial.println((char *)onoffbutton.lastread);
//    }
//  }

  // ping the server to keep the mqtt connection alive, only needed if we're not publishing
  //if(! mqtt.ping()) {
  //  Serial.println(F("MQTT Ping failed."));
  //}

}

float getPressure(){
  int count = 10;
  int waitLimit = 100;
  int waitCount = 0;
  char dataRead;
  int total = 0;
  float average;
  float voltage;

  while(!Serial3.available()){
    //Serial3.write('p');
    waitCount++;
    if(waitCount>waitLimit)return -1;
  }
  dataRead = Serial3.read();
  total = dataRead - '0';
  while(Serial3.available()){
    dataRead = Serial3.read();
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

  return voltage;
  //pressure_bar = (voltage - 0.5) / 4.0 * 12.0;
  //Serial.print("Pressure (bar): ");
  //Serial.println(pressure_bar);
  //pressure_psi = pressure_bar * 14.503773773020923;
  //Serial.print("Pressure (psi): ");
  //Serial.println(pressure_psi);
}

//float getVoltage(){
//  float measuredvbat = analogRead(BAT_PIN);
//  measuredvbat *= 2;    // we divided by 2, so multiply back
//  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
//  measuredvbat /= 1024; // convert to voltage
//  return measuredvbat;
//}

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

boolean FONAconnect(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password) {

  Watchdog.reset();

  Serial.println(F("Initializing FONA....(May take 3 seconds)"));
  
  //fonaSS.begin(4800); // if you're using software serial
  fonaSerial->begin(38400);
  
  if (! fona.begin(*fonaSerial)) {           // can also try fona.begin(Serial1) 
    Serial.println(F("Couldn't find FONA"));
    return false;
  }
  fonaSerial->println("AT+CMEE=2");
  Serial.println(F("FONA is OK"));
  Watchdog.reset();
  Serial.println(F("Checking for network..."));
  while (fona.getNetworkStatus() != 1) {
   delay(500);
  }

  Watchdog.reset();
  delay(5000);  // wait a few seconds to stabilize connection
  Watchdog.reset();
  
  fona.setGPRSNetworkSettings(apn, username, password);

  Serial.println(F("Disabling GPRS"));
  fona.enableGPRS(false);
  
  Watchdog.reset();
  delay(5000);  // wait a few seconds to stabilize connection
  Watchdog.reset();

  Serial.println(F("Enabling GPRS"));
  if (!fona.enableGPRS(true)) {
    Serial.println(F("Failed to turn GPRS on"));  
    return false;
  }
  Watchdog.reset();

  return true;
}

float ReceiveLora(){
  int count = 10;
  int waitLimit = 100;
  int waitCount = 0;
  char dataRead;
  int total = 0;
  float average;
  float voltage;

  while(!Serial1.available()){
    //Serial3.write('p');
    waitCount++;
    if(waitCount>waitLimit)return -1;
  }
  dataRead = Serial1.read();
  total = dataRead - '0';
  while(Serial1.available()){
    dataRead = Serial1.read();
    if(dataRead == '.')break;
    if(dataRead < '0' || dataRead > '9')continue;
    total *= 10;
    total += dataRead -'0';
  }
  if (Serial1.available()) {
    while (Serial1.available())
      queue.push(Serial1.read());

    Serial.print("Queue: ");
    Serial.print(queue.count(), DEC);
    Serial.println(" bytes");
  }

  if (queue.count() > 0 && digitalRead(AUX)) {
    char c;
    static char s[MAX_TX_SIZE];
    static int cnt = 0;

    do {
      char c = queue.pop();  // The oldest
      s[cnt++] = c;

      if (cnt == MAX_TX_SIZE || c == '\n') {
        Serial1.write(s, cnt);

        Serial.write(s, cnt);  // Display
        if (c != '\n' && c != '\r')  // Add 'newline'
          Serial.println();
        Serial.print("Send: ");
        Serial.print(cnt);
        Serial.println(" bytes.");
        cnt = 0;
        break;
      }
    } while (queue.count() > 0);

  }
  return average;
}

/*float getCurrentSensor() 
{
  int16_t adc3;  // we read from the ADC, we have a sixteen bit integer as a result

  adc3 = ads.readADC_SingleEnded(3);
  Voltage = (adc3 * 0.1875)/1000;
  
  Serial.print("AIN3: "); 
  Serial.print(adc3);
  Serial.print("\tVoltage: ");
  Serial.print(Voltage, 7);  
  Serial.println();

  sensorValue = 0;
//  for (int i = 0; i < avgSamples; i++)
//  {
    sensorValue += adc3; //ads.readADC_SingleEnded(3);

    // wait 2 milliseconds before the next loop
    // for the analog-to-digital converter to settle
    // after the last reading:
    Serial.print("GET: "); 
    Serial.print(sensorValue);
    Serial.println();
//    delay(2);

//  }

  sensorValue = sensorValue; // / (float) avgSamples;

  // The on-board ADC is 10-bits -> 2^10 = 1024 -> 5V / 1024 ~= 4.88mV
  // The voltage is in millivolts
  //float voltage = 4.88 * sensorValue;
  float voltage = 0.0763 * sensorValue;

  // This will calculate the actual current (in mA)
  // Using the Vref and sensitivity settings you configure
  //float current = (voltage - Vref) * sensitivity;
  float current = (voltage) * sensitivity;

  // This is the raw sensor value, not very useful without some calculations
  //Serial.print(sensorValue);
  
  Serial.print("sensVal: "); 
  Serial.print(sensorValue);
  Serial.println();
  Serial.print("voltCurrent: "); 
  Serial.print(voltage);
  Serial.println();
  Serial.print("Current: "); 
  Serial.print(current);
  Serial.println();
  Serial.println();

  delay(2);
  return current;
}*/

float getVNHCurrentSensor() 
{
  int16_t adc2;  // we read from the ADC, we have a sixteen bit integer as a result

  adc2 = analogRead(VNH_CS); //ads.readADC_SingleEnded(2);
  Voltage = (adc2 * 0.1875)/1000;
  
  Serial.print("AIN2: "); 
  Serial.print(adc2);
  Serial.print("\tVoltage: ");
  Serial.print(Voltage, 7);  
  Serial.println();

  sensorValue = 0;
//  for (int i = 0; i < avgSamples; i++)
//  {
    sensorValue += adc2; //ads.readADC_SingleEnded(3);

    // wait 2 milliseconds before the next loop
    // for the analog-to-digital converter to settle
    // after the last reading:
    Serial.print("GET: "); 
    Serial.print(sensorValue);
    Serial.println();
//    delay(2);

//  }

  sensorValue = sensorValue; // / (float) avgSamples;

  // The on-board ADC is 10-bits -> 2^10 = 1024 -> 5V / 1024 ~= 4.88mV
  // The voltage is in millivolts
  //float voltage = 4.88 * sensorValue;
  float voltage = 0.0763 * sensorValue;

  // This will calculate the actual current (in mA)
  // Using the Vref and sensitivity settings you configure
  //float current = (voltage - Vref) * sensitivity;
  float current = (voltage) * sensitivity;

  // This is the raw sensor value, not very useful without some calculations
  //Serial.print(sensorValue);
  
  Serial.print("sensVal: "); 
  Serial.print(sensorValue);
  Serial.println();
  Serial.print("voltCurrent: "); 
  Serial.print(voltage);
  Serial.println();
  Serial.print("Current: "); 
  Serial.print(current);
  Serial.println();
  Serial.println();

  delay(2);
  return current;
}
