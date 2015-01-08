//get then save config for wifi connection
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

SoftwareSerial debugPort(A3,A2);

char recvConf[24];
char mode;

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(5000);
  
  debugPort.begin(9600);
  debugPort.println(F("wifi config demo"));
  
  Serial.println(F("AT+RST"));
  while(!Serial.available());
  if(Serial.find("Ready")){
    debugPort.println(F("Ready"));
  }
  else{
    debugPort.println(F("No Response"));
    while(1);
  }
  delay(200);
  
  //mode = 0; //config mode
  mode = 1; //operation mode
}

void loop(){
  if(mode == 0){
    //init server
    Serial.println(F("AT+CWMODE=3"));
    delay(200);
    Serial.println(F("AT+CIPMUX=1"));
    delay(200);
    Serial.println(F("AT+CIPSERVER=1,8888"));
    delay(200);
    while(1){
      writeConfig();
    }
  }
  else if(mode == 1){
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
    debugPort.println(F("ssid is written"));
  }
  else if(recvConf[index] == '('){
    addr = 128;
    index = 1;
    while(recvConf[index-1] != ')'){
      EEPROM.write(addr,recvConf[index]); //put to EEPROM
      addr++;
      index++;
    }
    debugPort.println(F("pswd is written"));
  }
  debugPort.println(F("==="));
  delay(200);
}
