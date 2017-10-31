void setup()
{
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  Serial1.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  delay(100);
}

char input;


void loop()
{
  if (Serial.available()>0){
    input = Serial.read();
    //Serial.write(input);
    if(input == 's' || input == 'r'){
    switch(input)
    {
      case 's':
        SendMessage();
        break;
      case 'r':
        RecieveMessage();
        break;
    }
    }else{
      Serial1.println(input);
    }
  }
  if (Serial1.available()){
    while(Serial1.available()>0){
      input = Serial1.read();
      Serial.print(input);
    }
    //Serial.println();
  }
}
 void SendMessage()
{
  Serial1.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  Serial1.println("AT+CMGS=\"+62857xxxx3667\"\r"); // Replace x with mobile number
  delay(1000);
  Serial1.println("I am SMS from GSM Module");// The SMS text you want to send
  delay(100);
  Serial1.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}


 void RecieveMessage()
{
  Serial1.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
 }
