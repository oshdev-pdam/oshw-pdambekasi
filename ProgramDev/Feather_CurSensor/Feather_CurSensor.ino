#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads(0x48);
float Voltage = 0.0;

// Number of samples to average the reading over
// Change this to make the reading smoother... but beware of buffer overflows!
const int avgSamples = 1; //DISABLED

float sensorValue = 0;

float sensitivity = 100.0 / 500.0; //100mA per 500mV = 0.2
float Vref = 2500; // Output voltage with no current: ~ 2500mV or 2.5V

void setup(void) 
{
  Serial.begin(9600);  
  ads.begin();
}

void loop(void) 
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
}
