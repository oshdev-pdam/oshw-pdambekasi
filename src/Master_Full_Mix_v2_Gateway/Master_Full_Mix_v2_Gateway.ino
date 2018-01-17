#include <Adafruit_SleepyDog.h>
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

#define VNH_A   A3 
#define VNH_B   A4 
#define VNH_CS  A2
#define M0      3 
#define M1      2 
#define AUX     4 
#define SENSOR  A0

#define MAX_TX_SIZE 58
#include "Queue.h":
Queue<char> queue(MAX_TX_SIZE * 3);

#define DATA_PRESSURE '@'
#define DATA_CURSENSOR '#'

RTC_DS1307 rtc;

#include <SPI.h>
#include <SD.h>

const int chipSelect = 53;

// Default pins for Feather 32u4 FONA
#define FONA_RST A15 
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

#define FONA_APN       "indosat"
#define FONA_USERNAME  ""
#define FONA_PASSWORD  ""

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "hariantow"
#define AIO_KEY         "37dd1a3c98214ea1963f7e0c9967f22d"

Adafruit_MQTT_FONA mqtt(&fona, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

#define halt(s) { Serial.println(F( s )); while(1);  }

boolean FONAconnect(const __FlashStringHelper *apn, const __FlashStringHelper *username, const __FlashStringHelper *password);

Adafruit_MQTT_Publish no1_pressure_bar = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/no1-pressure-bar");
Adafruit_MQTT_Publish no1_pressure_psi = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/no1-pressure-psi");
Adafruit_MQTT_Publish no1_curSensor = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/no1-cursensor");

// How many transmission failures in a row we're willing to be ok with before reset
uint8_t txfailures = 0;
#define MAXTXFAILURES 10

extern Adafruit_FONA fona;

HardwareSerial *fonaSerial = &Serial1; //&Serial2;

float Voltage = 0.0;
const int avgSamples = 1; //DISABLED

float sensorValue = 0;

float sensitivity = 100.0 / 500.0; //100mA per 500mV = 0.2
float Vref = 2500; // Output voltage with no current: ~ 2500mV or 2.5V

void setup() {
  
  pinMode(LED_PIN, OUTPUT);  

  pinMode(VNH_A, OUTPUT);
  pinMode(VNH_B, OUTPUT);
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(AUX, INPUT);
  pinMode(SENSOR, INPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(9600);

  Serial.println(F("Adafruit FONA MQTT demo"));
  
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);

  digitalWrite(VNH_A, LOW);
  digitalWrite(VNH_B, LOW);
  digitalWrite(M0, HIGH);
  digitalWrite(M1, HIGH);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }

   if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

   if (!SD.begin(chipSelect)) {
     Serial.println("Card failed, or not present");
   }else{
    Serial.println("card initialized.");
   }
  Watchdog.reset();
  delay(5000);  // wait a few seconds to stabilize connection
  Watchdog.reset();
}

float pressureData;
int tempTime = 0;
int waitTime = 30;
int timeCounter = waitTime; 
bool firstRun = true;
bool uploadOK;
float batVoltData;
float pressure_barData;
float pressure_psiData;
float vnh_curData;
uint32_t batVoltDataInt;
uint32_t pressure_barDataInt;
uint32_t pressure_psiDataInt;
uint32_t vnh_curDataInt;
  
void loop() {

  Watchdog.reset();

  digitalWrite(LED_PIN, LOW);
  
  DateTime now = rtc.now();
  if (firstRun){
    tempTime = now.second();
    firstRun = false;
  }
  tempTime = now.second()-tempTime;
  if(tempTime<0)tempTime = 1;
  if(tempTime == 1){
    Serial.print("RTC Count: ");
    Serial.println(timeCounter);
  }
  timeCounter += tempTime;
  tempTime = now.second();
  
  if(timeCounter >= waitTime){

    digitalWrite(LED_PIN, HIGH);
    digitalWrite(VNH_A, HIGH);
    digitalWrite(VNH_B, LOW);
    
    while (! FONAconnect(F(FONA_APN), F(FONA_USERNAME), F(FONA_PASSWORD))) {
      Serial.println("Retrying FONA");
    }

    Serial.println(F("Connected to Cellular!"));
    uploadOK = false;
    
    digitalWrite(M0, LOW);
    digitalWrite(M1, LOW);
    delay(2000);
    
    while(uploadOK == false){
      MQTT_connect();
  
      Watchdog.reset();
      vnh_curData = getVNHCurrentSensor();
      vnh_curDataInt = (int) vnh_curData;
        
    if (! no1_curSensor.publish(vnh_curDataInt) ) {
        Serial.println(F("Failed"));
        txfailures++;
        if(txfailures >=10){
          txfailures = 0;
          uploadOK=true;
        }
      }
      uploadOK=true;
        
      pressureData = getPressure();
      Serial.print(F("\nWater Pressure val: "));
      Serial.println(pressureData);
      if(pressureData!= -1)pressureData=0; //{
        pressure_barData = ((pressureData - 0.5) / 4.0) * 12.0;
        pressure_barData *= 1000;
        pressure_barDataInt = (int) pressure_barData;
        Serial.print("Pressure (bar): ");
        Serial.println(pressure_barData);
        
        pressure_psiData = pressure_barData * 14.503773773020923;
        pressure_psiDataInt = (int) pressure_psiData;
        Serial.print("Pressure (psi): ");
        Serial.println(pressure_psiData);
  
        if (! no1_pressure_bar.publish(pressure_barDataInt) ) {
          Serial.println(F("Failed"));
          txfailures++;
          if(txfailures >=10){
            txfailures = 0;
            uploadOK=true;
          }
        }
     
        if (! no1_pressure_psi.publish(pressure_psiDataInt) ) {
          Serial.println(F("Failed"));
          txfailures++;
          if(txfailures >=10){
            txfailures = 0;
            uploadOK=true;
          }
        }    
    } 
    
    timeCounter=0;
    tempTime = now.second();
    
    bool waitMoreData = true;

    /*while(waitMoreData){
      Serial.println("Waiting for data...");
      delay(2000);
      waitLoraData();
    }*/
    
    digitalWrite(LED_PIN, LOW);

    digitalWrite(VNH_A, LOW);
    digitalWrite(VNH_B, LOW);
    
    digitalWrite(M0, HIGH);
    digitalWrite(M1, HIGH);
    delay(2000);
  }
  
  Watchdog.reset();  
  
}

float getPressure(){
  int count = 10;
  int waitLimit = 100;
  int waitCount = 0;
  char dataRead;
  int total = 0;
  float average;
  float voltage;

  dataRead = analogRead(SENSOR); //Serial3.read();
  
  voltage = dataRead * 5.0 / 1024.0;
  Serial.print("Voltage (volt): ");
  Serial.println(voltage);
  if(voltage<0.5 || voltage >4.5)return -1;

  return voltage;
}

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

float getVNHCurrentSensor() 
{
  int16_t adc2;  // we read from the ADC, we have a sixteen bit integer as a result

  adc2 = analogRead(VNH_CS); //ads.readADC_SingleEnded(2);
/*  Voltage = (adc2 * 0.1875)/1000;
  
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
*/
  float current = adc2 * 38;
  return current;
}
