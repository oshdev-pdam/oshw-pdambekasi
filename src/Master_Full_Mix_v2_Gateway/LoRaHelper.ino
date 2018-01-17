//Device ID for LoRa
#define DEVICE_ID '1'

//Data headers
#define ID_HEADER '!'
#define HANDSHAKE '+'
#define DATA_PRESSURE '@'
#define DATA_CURSENSOR '#'
#define DATA_END '~'

#define MAX_TX_SIZE 58
#define INTBUF_SIZE 6

Adafruit_MQTT_Publish no2_pressure_bar = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/no2-pressure-bar");
Adafruit_MQTT_Publish no2_pressure_psi = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/no2-pressure-psi");
Adafruit_MQTT_Publish no2_curSensor = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/no2-cursensor");

Adafruit_MQTT_Publish no3_pressure_psi = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/no3-pressure-psi");
Adafruit_MQTT_Publish no3_pressure_bar = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/no3-pressure-bar");
Adafruit_MQTT_Publish no3_curSensor = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/no3-cursensor");

void sendLora(uint32_t data, char dataType){
    char buf[MAX_TX_SIZE] = {'0'};
    int cnt = 0;
    
    char intBuf[INTBUF_SIZE];
    //bool firstDigit = false;
    sprintf(intBuf, "%06d", data);
    
    buf[cnt++] = ID_HEADER;
    buf[cnt++] = DEVICE_ID;
    buf[cnt++] = dataType;

    for(int i=0; i < INTBUF_SIZE; i++){
      //if(!firstDigit && intBuf[i] =='0')continue;
      //firstDigit=true;
      buf[cnt++] = intBuf[i];
    }
    
    buf[cnt++] = DATA_END;
    
    Serial.write(buf, cnt);
}

bool readLoraHandshake(){
  char buf[MAX_TX_SIZE] = {'0'};
  int cnt = 0;
  
  while (Serial.available() && cnt < MAX_TX_SIZE){
    buf[cnt++] = Serial.read();
  }

  for(int i = 0; i < cnt; i++){
    if(buf[i] == HANDSHAKE)return true;
  }
  return false;
}

void sendLoraWaitReply(uint32_t data, char dataType){
  int sendTries = 100;
  int sendCount = 0;

  int checkWait = 100;
  int checkCount = 0;
  while(sendCount < sendTries){
    checkCount = 0;
    sendLora(data, dataType);
    delay(10);
    sendCount++;
    while(checkCount < checkWait){
      if(digitalRead(AUX)==LOW){
        if(readLoraHandshake()==true)return;
      }
      delay(2);
    }  
  }
}

void sendLoraHandshake(){
  int sendTries = 100;
  int sendCount = 0;
  
  while(sendCount < sendTries){
    Serial.write(HANDSHAKE);
    delay(2);
  }
}

bool waitLoraData(){
  int checkWait = 1000;
  int checkCount = 0;
  while(checkCount < checkWait){
    if(digitalRead(AUX)==LOW){
      readLoraData();
      return true;
    }
    delay(10);
    checkCount++;
  }
  return false;
}

bool readLoraData(){
  //while(!Serial.available());

  char buf[MAX_TX_SIZE];
  char outBuf[MAX_TX_SIZE] = {'0'};
  int cnt = 0;
  char intBuf[INTBUF_SIZE] = {'0'};

  char ID = ' ';
  char dataType = ' ';

  char data = ' ';
  while (Serial.available() && cnt < MAX_TX_SIZE){
    data = Serial.read();
    switch(data){
      case ID_HEADER:
        outBuf[cnt++] = data;
        ID = Serial.read();
        outBuf[cnt++] = ID;
        break;
      case DATA_PRESSURE:
        dataType = data;
        outBuf[cnt++] = data;
        for(int i=0; i < INTBUF_SIZE; i++){
          intBuf[i] = Serial.read();
          outBuf[cnt++] = intBuf[i];
          //Serial.println(intBuf);
        }
        break;
      case DATA_CURSENSOR:
        dataType = data;
        outBuf[cnt++] = data;
        for(int i=0; i < INTBUF_SIZE; i++){
          intBuf[i] = Serial.read();
          outBuf[cnt++] = intBuf[i];
          //Serial.println(intBuf);
        }
        break;
      case DATA_END:
        outBuf[cnt++] = data;
        cnt = MAX_TX_SIZE;
        break;
    }
  }
  
  //Serial.print("data received: ");
  //Serial.println(outBuf);
  
  if(ID < '1' || ID > '9' || (dataType != '@' && dataType != '#') )return false;

  int IDnum = ID - '0';
  uint32_t intData = 0;
  float calcData = 0.0;
  uint32_t intDataBar;
  uint32_t intDataPsi;
  
  intData = atoi(intBuf);
  switch(dataType){
    case DATA_PRESSURE:
      intDataBar = intData;
        
      calcData = (float) intData * 14.503773773020923;
      intDataPsi = (int) calcData;
      Serial.println(intDataBar);
      Serial.println(intDataPsi);  
      if(IDnum <= 1){
        //0 is unassigned, 1 is for gateway
      }else if(IDnum <= 2){
        no2_pressure_bar.publish(intDataBar);
        no2_pressure_psi.publish(intDataPsi);
      }else if(IDnum <= 3){
        no3_pressure_bar.publish(intDataBar);
        no3_pressure_psi.publish(intDataPsi);
      }
      break;
    case DATA_CURSENSOR:
      if(IDnum <= 1){
        //0 is unassigned, 1 is for gateway
      }else if(IDnum <= 2){
        no2_curSensor.publish(intData);
      }else if(IDnum <= 3){
        no3_curSensor.publish(intData);
      }
      break;
  }

  sendLoraHandshake();

  return true;
}

