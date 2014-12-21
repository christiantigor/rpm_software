//get then save config for wifi connection
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

SoftwareSerial debugPort(A3,A2);

char recvConf[24];

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
  delay(1000);
  
  Serial.println(F("AT+CIPMUX=1"));
  delay(500);
  Serial.println(F("AT+CIPSERVER=1,8888"));
  delay(500);
}

void loop(){
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
      //debugPort.print(recvConf[index]); //put to EEPROM
      EEPROM.write(addr,recvConf[index]);
      addr++;
      index++;
    }
    debugPort.println();
  }
  else if(recvConf[index] == '('){
    addr = 128;
    index = 1;
    while(recvConf[index] != ')'){
      debugPort.print(recvConf[index]); //put to EEPROM
      addr++;
      index++;
    }
    debugPort.println();
  }
  //debugPort.println(recvConf);
  debugPort.println(F("==="));
  delay(1000);
}
