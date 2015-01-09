//get then save config for wifi connection
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

SoftwareSerial debugPort(9,8);

char recvConf[24];
char mode;

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(5000);
  
  debugPort.begin(4800);
  debugPort.println(F("wifi config demo"));
  
  pinMode(2,INPUT); //config button
  pinMode(A1,OUTPUT); //connection indicator
  digitalWrite(A1,LOW); //turn off connection indicator
  
  Serial.println(F("AT+RST"));
  delay(1000);
  while(!Serial.available());
  if(Serial.find("Ready")){
    debugPort.println(F("rdy"));
  }
  else{
    debugPort.println(F("no rspn"));
    while(1){
      digitalWrite(A1,HIGH);
      delay(200);
      digitalWrite(A1,LOW);
      delay(200);
    }
  }
  delay(1000);
  
  //mode = 0; //config mode
  //mode = 1; //operation mode
  mode = digitalRead(2);
}

void loop(){
  if(mode == 1){
    //init server
    debugPort.println(F("init srvr"));
    delay(200);
    Serial.println(F("AT+CWMODE=3"));
    delay(200);
    Serial.println(F("AT+CIPMUX=1"));
    delay(200);
    Serial.println(F("AT+CIPSERVER=1,8888"));
    delay(1000);
    while(1){
      writeConfig();
    }
  }
  else if(mode == 0){
    //operation mode
    readConfig();
    debugPort.println(F("operation mode"));
  }
}

void readConfig(){
  //read wifi ssid
  boolean readCont = true;
  byte addr = 0;
  byte index = 0;
  memset(recvConf,0,sizeof(recvConf)); //clear array
  
  while(readCont){
    char c = EEPROM.read(addr);
    if(c == '>'){
      readCont = false;
    }
    else{
      recvConf[index] = c;
      addr++;
      index++;
    }
  }
  debugPort.println(recvConf);
  //read wifi password
  readCont = true;
  addr = 128;
  index = 0;
  memset(recvConf,0,sizeof(recvConf)); //clear array
  while(readCont){
    char c = EEPROM.read(addr);
    if(c == ')'){
      readCont = false;
    }
    else{
      recvConf[index] = c;
      addr++;
      index++;
    }
  }
  debugPort.println(recvConf);
  while(1); //must be deleted
}

void writeConfig(){
  boolean ssidEn = false;
  boolean passEn = false;
  byte index = 0;
  memset(recvConf,0,sizeof(recvConf)); //clear array
  
  while(!Serial.available());
  while(Serial.available()){
    char c = Serial.read();
    //debugPort.write(c);
    if(c == '<') ssidEn = true;
    else if(c == '(') passEn = true;
    if(ssidEn){
      if(c == '>') ssidEn = false;
      recvConf[index] = c;
      index++;
    }
    else if(passEn){
      if(c == ')') passEn = false;
      recvConf[index] = c;
      index++;
    }
  }
  //put recvConf to EEPROM
  //put <SSIDdata> start from addr 0
  //put (PASSdata) start from addr 128
  byte addr;
  index = 0;
  if(recvConf[index] == '<'){
    addr = 0;
    index = 1;
    while(recvConf[index-1] != '>'){
      EEPROM.write(addr,recvConf[index]); //put to EEPROM
      addr++;
      index++;
    }
    debugPort.println(F("wrt ssid"));
  }
  else if(recvConf[index] == '('){
    addr = 128;
    index = 1;
    while(recvConf[index-1] != ')'){
      EEPROM.write(addr,recvConf[index]); //put to EEPROM
      addr++;
      index++;
    }
    debugPort.println(F("wrt pswd"));
  }
  debugPort.println(F("==="));
  
  digitalWrite(A1,HIGH);
  delay(250);
  digitalWrite(A1,LOW);
  delay(250);
}
