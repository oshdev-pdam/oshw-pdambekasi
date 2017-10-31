/* YourDuino SoftwareSerialExample1
   - Connect to another Arduino running "YD_SoftwareSerialExampleRS485_1Remote"
   - Connect this unit Pins 10, 11, Gnd
   - Pin 3 used for RS485 direction control
   - To other unit Pins 11,10, Gnd  (Cross over)
   - Open Serial Monitor, type in top window. 
   - Should see same characters echoed back from remote Arduino

   Questions: terry@yourduino.com 
*/

/*-----( Import needed libraries )-----*/
//#include <SoftwareSerial.h>
#include <Arduino.h>
#include "wiring_private.h"
/*-----( Declare Constants and Pin Numbers )-----*/
//#define SSerialRX        10 //10 //Serial Receive pin
//#define SSerialTX        13  //11 //Serial Transmit pin

#define SSerialTxControl 12 //3   //RS485 Direction control
//#define SSerialTxControlRe 13 //tambahan buat RE/ yang dijumper di sample

#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define Pin13LED         13

/*-----( Declare objects )-----*/
//SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

/*-----( Declare Variables )-----*/
int byteReceived;
int byteSend;

int udahKirim;

Uart Serial2 (&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);

void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}

void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial1.begin(9600);
  
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);
  
  Serial.println("YourDuino.com SoftwareSerial remote loop example");
  Serial.println("Use Serial Monitor, type in upper window, ENTER");
  
  pinMode(Pin13LED, OUTPUT);   
  pinMode(SSerialTxControl, OUTPUT);    
  //pinMode(SSerialTxControlRe, OUTPUT);    
  
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver   
  //digitalWrite(SSerialTxControlRe, RS485Receive);  // Init Transceiver   
  
  // Start the software serial port, to another device
  //RS485Serial.begin(4800);   // set the data rate 
  Serial2.begin(4800);

}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  digitalWrite(Pin13LED, HIGH);  // Show activity
  if (Serial.available())
  {
    byteReceived = Serial.read();
    
    digitalWrite(SSerialTxControl, RS485Transmit);  // Enable RS485 Transmit   
    //digitalWrite(SSerialTxControlRe, RS485Transmit);  // Enable RS485 Transmit   
    //RS485Serial.write(byteReceived);          // Send byte to Remote Arduino
    Serial2.write(byteReceived);          // Send byte to Remote Arduino
    
    digitalWrite(Pin13LED, LOW);  // Show activity    
    delay(10);
    digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit       
    //digitalWrite(SSerialTxControlRe, RS485Receive);  // Disable RS485 Transmit       
  }
  
  if (Serial2.available())  //Look for data from other Arduino
   {
    digitalWrite(Pin13LED, HIGH);  // Show activity
    byteReceived = Serial2.read();    // Read received byte
    Serial.write(byteReceived);        // Show on Serial Monitor
    delay(10);
    digitalWrite(Pin13LED, LOW);  // Show activity   
   }

}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/

//NONE
//*********( THE END )***********

