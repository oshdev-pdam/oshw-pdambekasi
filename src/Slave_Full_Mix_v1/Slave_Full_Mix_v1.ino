#include "Sleep_n0m1.h"

#define intPin 0
#define LED_PIN 13

Sleep sleep;

int sensorPin = A5;
int sensorRaw;
float sensorAverage;
int count;
char dataRead;

void setup(){
 Serial.begin(9600);
 Serial1.begin(9600);
 count = 10;

 pinMode(LED_PIN, OUTPUT);
}

void loop(){
  while(!Serial1.available());
  dataRead=Serial1.read();
  if(dataRead=='p'){

    digitalWrite(LED_PIN, HIGH);
    
    sensorAverage = 0;
    for(int i=0; i<count; i++){
      sensorRaw = analogRead(sensorPin);
      sensorAverage += sensorRaw;
      Serial.println(sensorRaw);
    }
    //sensorAverage /= (float) count;
    //voltage = (float) sensorAverage * 5.0 / 1024.0;
    //pressure_kPa = (voltage - 0.5) / 4.0 * 1200.0; 
    //pressure_kPa = (voltage - 0.48) / 4.02 * 1200.0;
    //if(pressure_kPa<0)pressure_kPa=0; 
    //pressure_psi = pressure_kPa * 0.14503773773020923;
    
    //Serial.print("Raw value (average): ");
    Serial1.print(sensorAverage);
    Serial.print("Total RAW: ");
    Serial.println(sensorAverage);
    //Serial.println((int)(100*sensorAverage));

    digitalWrite(LED_PIN, LOW);
    
    delay(1000);
  }else if(dataRead=='s'){
    digitalWrite(LED_PIN, HIGH);
    delay(400);
    digitalWrite(LED_PIN, LOW);
    delay(200);
    digitalWrite(LED_PIN, HIGH);
    delay(400);
    digitalWrite(LED_PIN, LOW);
    sleep.pwrDownMode(); //set sleep mode

    //Sleep till interrupt pin equals a particular state.
    //In this case "low" is state 0.
    sleep.sleepPinInterrupt(intPin,LOW); //(interrupt Pin Number, interrupt State)
    
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW); 
  }
}
