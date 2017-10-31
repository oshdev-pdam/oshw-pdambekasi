int sensorPin = A5;
int sensorRaw;
float sensorAverage;
int count;
float voltage;
float pressure_kPa;
float pressure_psi;

void setup(){
 Serial.begin(9600);
 count = 1;
}

void loop(){
  sensorAverage = 0;
  for(int i=0; i<count; i++){
    sensorRaw = analogRead(sensorPin);
    sensorAverage += sensorRaw;
  }
  sensorAverage /= (float) count;
  voltage = (float) sensorAverage * 5.0 / 1024.0;
  pressure_kPa = (voltage - 0.5) / 4.0 * 1200.0; 
  //pressure_kPa = (voltage - 0.48) / 4.02 * 1200.0;
  if(pressure_kPa<0)pressure_kPa=0; 
  pressure_psi = pressure_kPa * 0.14503773773020923;
  Serial.print("Raw value (average): ");
  Serial.print(sensorAverage);
  Serial.println();
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");
  Serial.print("Pressure (kPa): ");
  Serial.print(pressure_kPa);
  Serial.println(" kPa");
  Serial.print("Pressure (Psi): ");
  Serial.print(pressure_psi);
  Serial.println(" Psi");
  
  Serial.print("Atmosphere: ");
  Serial.print(pressure_kPa*0.00986923);
  Serial.println(" Atm");
  Serial.println();  
  delay(1000);
}
