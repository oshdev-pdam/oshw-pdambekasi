#include <OneWire.h>
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

#define LED_PIN 13
#define VNH_A   A3 
#define VNH_B   A4 
#define VNH_CS  A2

#define SENSOR  A0
#define BAT     A8
#define DATA_PRESSURE '@'
#define DATA_CURSENSOR '#'
#define MAX_TX_SIZE 58
#define MAXTXFAILURES 10
#define SerialAT Serial1

//-----------------------CONFIG_01-------------------------------
// Device-specific config
#define SLEEPTIME        360                                   //Sleep time, in seconds
// Provider APN settings
const char apn[]  = "internet";
const char user[] = "";
const char pass[] = "";
//-----------------------CONFIG_01-------------------------------

#include "Queue.h":
Queue<char> queue(MAX_TX_SIZE * 3);
RTC_DS1307 rtc;
OneWire  ds(A10);  // on pin 10 (a 4.7K resistor is necessary)
#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, Serial);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif
TinyGsmClient client(modem);

//-----------------------CONFIG_02-------------------------------
// Specific to adafruit.io
#define AIO_SERVER      "io.adafruit.com"
#define AIO_USERNAME    "madiqbal"
#define AIO_KEY         "3134c50dd40d4187a7df32b87cd3c51f"     //Private AIO key
// Name of the server we want to connect to
const char server[]   = "io.adafruit.com";
const int  port       = 80;
const char resource[] = "/api/v2/madiqbal/feeds";         //format: "/api/v2/[USERNAME]/feeds"
const char feedPressureBar[] = "/no5-pressure-bar/data";  //format: "/[FEEDNAME]/data"
const char feedPressurePsi[] = "/no5-pressure-psi/data";
const char feedCurSensor[] = "/no5-cursensor/data";
const char feedBatVoltage[] = "/no5-batvoltage/data";
const char feedTempCels[] = "/no5-tempcels/data";
const char feedQuota[] = "/no5-quota/data";
//-----------------------CONFIG_02-------------------------------

HttpClient http(client, server, port);
const int chipSelect = 53;
uint8_t txfailures = 0;
float Voltage = 0.0;
const int avgSamples = 1; //DISABLED
float sensorValue = 0;
float sensitivity = 100.0 / 500.0; //100mA per 500mV = 0.2
float Vref = 2500; // Output voltage with no current: ~ 2500mV or 2.5V

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);  
  pinMode(VNH_A, OUTPUT);
  pinMode(VNH_B, OUTPUT);
  pinMode(SENSOR, INPUT);
  digitalWrite(LED_PIN, LOW);  
  SerialAT.begin(9600); //Serial1
  digitalWrite(VNH_A, LOW);
  digitalWrite(VNH_B, LOW);  
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
  WDTCSR = (3<<3); //enable watchdog prescaler change (set WDCE to 1) - also resets
  WDTCSR = (6); //set prescaler to have 1 sec timeout / delay
  WDTCSR |= (1<<6); //enable interrupt mode (set WDIE to 1) 
  //ENABLE SLEEP - this enables the sleep mode
  SMCR |= (1 << 2); //power down mode
}

float pressureData;
int waitTime = 600;
int timeCounter = waitTime; 
bool firstRun = true;
bool uploadOK;
float batVoltData;
float pressure_barData;
float pressure_psiData;
float vnh_curData;
float vnh_totalData;
float temp_Data;
uint32_t batVoltDataInt;
uint32_t pressure_barDataInt;
uint32_t pressure_psiDataInt;
uint32_t vnh_curDataInt;
uint32_t temp_DataInt;
char logBuf[12];
char fileName[24];
int retryLimit = 10;
int retryCount;
char buf[36];
char append[12];
char quotaBuf[8];
uint32_t quotaData;
int readLoraTimeStart = 0;
int readLoraTimeEnd = 15;
int tempTime = 0;
  
void loop() {
  digitalWrite(LED_PIN, LOW);
  digitalWrite(VNH_A, LOW);
  digitalWrite(VNH_B, LOW);
  
  if(timeCounter >= waitTime){
    vnh_totalData = 0;
    digitalWrite(VNH_A, HIGH);
    digitalWrite(VNH_B, LOW);

    modem.init();
    Serial.print(F("Waiting for network..."));
    if (!modem.waitForNetwork()) {
      Serial.println(" Network fail");
      return;
    }
    Serial.println(" OK");

    quotaData = getRemainingQuota();
    Serial.print("Remaining balance: ");
    Serial.println(quotaData);

    Serial.print(F("Waiting for network..."));
    if (!modem.waitForNetwork()) {
      Serial.println(" Network fail");
      return;
    }
    Serial.print(F("Connecting to "));
    Serial.print(apn);
    if (!modem.gprsConnect(apn, user, pass)) {
      Serial.println(" GPRS fail");
      return;
    }
    Serial.println(" OK");
    // if you get a connection, report back via serial:
    if (client.connect(server, port)) {
      Serial.print("Connected to ");
      Serial.println(server);
      retryCount = 0;

      uploadData(buf, feedQuota,quotaData);
      
      vnh_curData = getVNHCurrentSensor();
      vnh_totalData += vnh_curData;
      vnh_curDataInt = (uint32_t) vnh_curData; 
      sprintf(logBuf, "!%db", vnh_curDataInt);   

      vnh_curData = getVNHCurrentSensor();
      vnh_totalData += vnh_curData;
      vnh_curDataInt = (uint32_t) vnh_curData; 
      sprintf(logBuf, "!%dc", vnh_curDataInt);   
      
      vnh_curData = getVNHCurrentSensor();
      vnh_totalData += vnh_curData;
      vnh_curDataInt = (uint32_t) vnh_curData; 
      sprintf(logBuf, "!%dd", vnh_curDataInt);   

      batVoltData = getBattery();
      batVoltDataInt = (int) batVoltData;

      temp_Data = getTemp();
      if(temp_Data == -1000)temp_Data = 0;
      temp_DataInt = (uint32_t) temp_Data;
      Serial.print("temp = ");
      Serial.println(temp_Data);
  
      vnh_curData = getVNHCurrentSensor();
      vnh_totalData += vnh_curData;
      vnh_curDataInt = (uint32_t) vnh_totalData/5;

      vnh_curData = getVNHCurrentSensor();
      vnh_totalData += vnh_curData;
      vnh_curDataInt = (uint32_t) vnh_curData; 
      sprintf(logBuf, "!%de", vnh_curDataInt);   

      uploadData(buf, feedBatVoltage,batVoltDataInt);
      
      vnh_curData = getVNHCurrentSensor();
      vnh_totalData += vnh_curData;
      vnh_curDataInt = (int) vnh_curData; 
      sprintf(logBuf, "!%df", vnh_curDataInt);   
      uploadData(buf, feedTempCels,temp_DataInt);
     
      vnh_curData = getVNHCurrentSensor();
      vnh_totalData += vnh_curData;
      vnh_curDataInt = (uint32_t) vnh_curData; 
      sprintf(logBuf, "!%dg", vnh_curDataInt);   

      pressureData = getPressure();
      Serial.print(F("\nWater Pressure val: "));
      Serial.println(pressureData);
      pressure_barData = ((pressureData - 0.5) / 4.0) * 12.0;
      
      if(pressureData== -1)pressure_barData=0.0;
      
      pressure_barData *= 1000.0;
      pressure_barDataInt = (uint32_t) pressure_barData;
      Serial.print("Pressure (bar): ");
      Serial.println(pressure_barData);
      
      uploadData(buf, feedPressureBar,pressure_barDataInt);
      
      vnh_curData = getVNHCurrentSensor();
      vnh_totalData += vnh_curData;
      vnh_curDataInt = (uint32_t) vnh_curData; 
      sprintf(logBuf, "!%dh", vnh_curDataInt);   

      pressure_psiData = pressure_barData * 14.5038;
      pressure_psiDataInt = (uint32_t) pressure_psiData;
      Serial.print("Pressure (psi): ");
      Serial.println(pressure_psiData);
      
      uploadData(buf, feedPressurePsi, pressure_psiDataInt);
      
      vnh_curData = getVNHCurrentSensor();
      vnh_totalData += vnh_curData;
      vnh_curDataInt = (uint32_t) vnh_curData; 
      sprintf(logBuf, "!%di", vnh_curDataInt);   

      vnh_curDataInt = (uint32_t) vnh_totalData/9;
      uploadData(buf, feedCurSensor,vnh_curDataInt);
   
      vnh_curData = getVNHCurrentSensor();
      vnh_curDataInt = (uint32_t) vnh_curData; 
      sprintf(logBuf, "!%dj", vnh_curDataInt);   

      client.stop();
      Serial.println("Server disconnected");
    
      modem.gprsDisconnect();
      Serial.println("GPRS disconnected");
      
    } else {
      // if you didn't get a connection to the server:
      Serial.println("Connection failed");
    }
    
    digitalWrite(LED_PIN, LOW);
    digitalWrite(VNH_A, LOW);
    digitalWrite(VNH_B, LOW);

    LowPowerSleep(SLEEPTIME); //deep sleep for ~SLEEPTIME sec
    
    vnh_curData = getVNHCurrentSensor();
    vnh_curDataInt = (uint32_t) vnh_curData; 
    sprintf(logBuf, "!%dk", vnh_curDataInt);   
  }  
}

float getPressure(){
  int count = 10;
  int waitLimit = 100;
  int waitCount = 0;
  int dataRead;
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

float getBattery(){
  int dataRead;
  float voltage;

  dataRead = analogRead(BAT);
    
  voltage = dataRead * 5.0 * 1000.0 / 1024.0;
  
  return voltage;
}

float getVNHCurrentSensor() 
{
  int adc2;
  
  adc2 = analogRead(VNH_CS); //ads.readADC_SingleEnded(2);

  float current = adc2 * 38;
  return current;
}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius
  
  byte data[12];
  byte addr[8];
  
  if ( !ds.search(addr)) {
    //no more sensors on chain, reset search
    ds.reset_search();
    return -1000;
  }
  
  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return -1000;
  }
  
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized");
    return -1000;
  }
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
  
  delay(750); // Wait for temperature conversion to complete
  
  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad
  
  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];
  
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;

}

void packData(char *buf, uint32_t data){
  strcpy(buf,"{\"datum\":{\"value\":\"");
  sprintf(append,"%u",data);
  strcat(buf,append);
  strcat(buf,"\"}}");
}

void uploadData(char *buf, char *feedName, uint32_t data){
  packData(buf,data);
  Serial.print("Sent data: ");
  Serial.println(buf);
  Serial.print("Data size: ");
  Serial.println(strlen(buf));
  Serial.print("Feed: ");
  Serial.print(resource);
  Serial.println(feedName);
  // Make a HTTP request:
  client.print("POST ");
  client.print(resource);
  client.print(feedName);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(server);
  
  client.print("Content-Length: ");
  client.println(strlen(buf));
  client.println("Content-Type: application/json");
  client.print("X-AIO-Key: ");
  client.println(AIO_KEY);
  client.println();// important need an empty line here 
  
  client.println(buf);
}

uint32_t getRemainingQuota(){
  int timeout = 60;
  int index = 0;
  char c;
  bool dataStart = false;
  bool numFound = false;
  SerialAT.println("AT+CUSD=1,\"*888*3*2#\"");

  while(timeout--){
    if(index >= 7){
      break;
    }
    while(SerialAT.available()){
      c = SerialAT.read();
      Serial.print(c);
      if(dataStart == false){
        if(c == '"'){
          dataStart = true;
        }
        continue;
      }
      if(c < '0' || c > '9'){
        if(numFound){
          timeout = 0;
          break; 
        }
        continue;
      }
      if(numFound == false){
        numFound = true;
      }
      quotaBuf[index] = c;
      index++;
    }
    if(timeout==0){
      break;
    }
    //delay(1000);
    LowPowerSleep(1);
  }
  quotaBuf[index] = 0;

  Serial.print("quotaBuf: ");
  Serial.println(quotaBuf);
  
  return atoi(quotaBuf);
}

void LowPowerSleep(int period){
  SMCR |= 1; //enable sleep
  //Disable ADC before sleep
  ADCSRA &= ~(1 << 7);
  for(int i=0; i<=period; i++){ 
    //BOD DISABLE - this must be called right before the __asm__ sleep instruction
    MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
    MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
    __asm__  __volatile__("sleep");//in line assembler to go to sleep
  }
  SMCR &= ~(1); //disable sleep
  //Enable ADC after waking up
  ADCSRA |= (1 << 7);
}

ISR(WDT_vect){
  //dummy interrupt handler to prevent reset
}

