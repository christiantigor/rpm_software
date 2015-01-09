//an integrated program for mainsnode
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <EmonLib.h>
//#include <SoftwareSerial.h>

//SoftwareSerial debugPort(9,8); //rev_a board
EnergyMonitor emon;

byte state = 2; //device status 0:off, 1:on, 2:unknown
char command[3];
char recvConf[24];
char mode;

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(5000);
  
  //debugPort.begin(4800);
  //debugPort.println(F("demo"));
  
  pinMode(2,INPUT); //config button
  pinMode(7,OUTPUT); //relay
  pinMode(A1,OUTPUT); //connection indicator
  digitalWrite(A1,LOW); //turn off connection indicator
  
  emon.current(2,2.3923);
  emon.voltage(0,468.0851,2);
  
  Serial.println(F("AT+RST"));
  while(!Serial.available());
  if(Serial.find("Ready")){
    //debugPort.println(F("rdy"));
  }
  else{
    //debugPort.println(F("no rspns"));
    //indicator
    while(1){
      digitalWrite(A1,HIGH);
      delay(500);
      digitalWrite(A1,LOW);
      delay(500);
    }
  }
  delay(200);
  
  //mode = 1; //config mode
  //mode = 0; //operation mode
  mode = digitalRead(2);
  //debugPort.println(mode);
}

void loop(){
  if(mode == 1){
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
  else if(mode == 0){
    //connect to wifi
    boolean isConnect = false;
    for(int i=0;i<2;i++){
      if(connectWifi()){
        isConnect=true;
        break;
      }
    }
    if(isConnect){
      //debugPort.println(F("connected"));
      digitalWrite(A1,HIGH); //turn on connection indicator
    }
    else{
      //debugPort.println(F("not connected"));
      //indicator
      while(1){
        digitalWrite(A1,HIGH);
        delay(500);
        digitalWrite(A1,LOW);
        delay(500);
      }
    }
    delay(200);
    Serial.println(F("AT+CIPMUX=0"));
    while(1){
      float power = calculatePower();
      sendData(power);
      delay(2000);
      
      for(char i=0;i<3;i++){
        getCmd();
      }
    }
  }
}

float calculatePower(){
  emon.calcVI(20,2000);
  float realPower = emon.realPower;
  return realPower;
}

void sendData(float data){
  Serial.println(F("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80"));
  delay(2000);
  if(Serial.find("ERROR")) return;
  //delay(1000);
  
  char temp[6];
  String strData;
  String cmd = "GET /update?api_key=99N5T6AH3NY4UKNA&field1=";
  dtostrf(data,4,1,temp);
  strData = String(temp);
  cmd += strData;

  Serial.print(F("AT+CIPSEND="));
  Serial.println(cmd.length()+2);
  if(Serial.find(">")){
    Serial.println(cmd);
  }
  else{
    Serial.println(F("AT+CIPCLOSE"));
    delay(1000);
    return;
  }
  
  if(Serial.find("OK")){
    //debugPort.println(F("dt sent"));
    digitalWrite(13,HIGH);
    delay(250);
    digitalWrite(13,LOW);
    Serial.println(F("AT+CIPCLOSE"));
    delay(250);
  }
  else{
    //debugPort.println(F("send err"));
    Serial.println(F("AT+CIPCLOSE"));
    delay(1000);
  }
}

void getCmd(){
  Serial.println(F("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80"));
  while(Serial.available()){
    Serial.read();
  }
  byte index = 0;
  while(Serial.availableCmd()){
    if(index < 3){ //size of command buffer
      char c = Serial.readCmd();
      command[index] = c;
      index++;
    }
    else{
      Serial.readCmd();
    }
  }
  if(Serial.find("ERROR")) return;
  delay(1000);
  
  Serial.println(F("AT+CIPSEND=70"));
  if(Serial.find(">")){
    //debugPort.print(">");
  }
  else{
    Serial.println(F("AT+CIPCLOSE"));
    //debugPort.println("connect timeout");
    return;
  }
  Serial.print(F("GET /talkbacks/887/commands/last?api_key=ACM8XW24UDVY1GTV HTTP/1.1\r\n\r\n"));
  while(Serial.available()){
    Serial.read();
  }
  index = 0;
  while(Serial.availableCmd()){
    if(index < 3){ //size of command buffer
      char c = Serial.readCmd();
      command[index] = c;
      index++;
    }
    else{
      Serial.readCmd();
    }
  }
  delay(1000);
  
  if(command[0]=='#' && command[1]=='#' && state!=0){ //turn off
    //debugPort.println(F("turn off"));
    digitalWrite(7,LOW);
    state = 0;
  }
  else if(command[0]=='&' && command[1]=='&' && state!=1){ //turn on
    //debugPort.println(F("turn on"));
    digitalWrite(7,HIGH);
    state = 1;
  }
  else if(state==0){
    //debugPort.println(F("keep off"));
  }
  else if(state==1){
    //debugPort.println(F("keep on"));
  }
  else{
    //debugPort.println(state);
  }
  
  Serial.println(F("AT+CIPCLOSE"));
  //debugPort.println(F("===\r\n"));
  delay(1000);
}

boolean connectWifi(){
  Serial.println(F("AT+CWMODE=1"));
  delay(200);
  //Serial.println(F("AT+CWJAP=\"Tritronik Mobile\",\"Tri12@11\""));
  Serial.print(F("AT+CWJAP=\""));
  
  //start read wifi ssid
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
  Serial.print(recvConf); //wifi ssid
  Serial.print(F("\",\""));
  //end read wifi ssid

  //start read wifi password
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
  Serial.print(recvConf); //wifi password
  Serial.print("\"");
  //end read wifi password
  
  if(Serial.find("OK")){
    return true;
  }
  else{
    return false;
  }
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
    //debugPort.println(F("ssid is written"));
  }
  else if(recvConf[index] == '('){
    addr = 128;
    index = 1;
    while(recvConf[index-1] != ')'){
      EEPROM.write(addr,recvConf[index]); //put to EEPROM
      addr++;
      index++;
    }
    //debugPort.println(F("pswd is written"));
  }
  //debugPort.println(F("==="));
  digitalWrite(A1,HIGH);
  delay(200);
  digitalWrite(A1,LOW);
  delay(200);
}
