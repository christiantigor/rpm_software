//an integrated program for mainsnode
#include <avr/pgmspace.h>
//#include <SoftwareSerial.h>
#include <EmonLib.h>

//SoftwareSerial debugPort(9,8);
EnergyMonitor emon;

byte state = 2; //device status 0:off, 1:on, 2:unknown
char command[3];

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(5000);
  
  //debugPort.begin(4800);
  //debugPort.println(F("demo"));
  
  pinMode(7,OUTPUT); //relay
  pinMode(A1,OUTPUT); //indicator connected
  digitalWrite(A1,HIGH); //turn on connection indicator
  delay(500);
  digitalWrite(A1,LOW);
  
  emon.current(2,1.6042);
  emon.voltage(0,488.8889,1);
  
  Serial.println(F("AT+RST"));
  while(!Serial.available());
  if(Serial.find("Ready")){
    //debugPort.println(F("rdy"));
  }
  else{
    //debugPort.println(F("no rspns"));
    while(1);
  }
  delay(1000);
  
  boolean isConnect = false;
  for(int i=0;i<5;i++){
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
    while(1);
  }
  delay(1000);
  
  Serial.println(F("AT+CIPMUX=0"));
}

void loop(){
  float power = calculatePower();
  sendData(power);
  delay(2000);
  
  for(int i=0;i<5;i++){
    getCmd();
  }
}

float calculatePower(){
  emon.calcVI(20,2000);
  float apparentPower = emon.apparentPower;
  return apparentPower;
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
    //delay(1); //duration for print is around 10sec/baud per char
  }
  else{
    Serial.println(F("AT+CIPCLOSE"));
    //debugPort.println("connect timeout");
    //delay(17); //duration for print is around 10sec/baud per char
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
  else{
    //debugPort.println(state);
  }
  
  Serial.println(F("AT+CIPCLOSE"));
  //debugPort.println(F("===\r\n"));
  delay(1000);
}

boolean connectWifi(){
  Serial.println(F("AT+CWMODE=1"));
  delay(1000);
  Serial.println(F("AT+CWJAP=\"Tritronik Mobile\",\"Tri12@11\""));
  //Serial.println(F("AT+CWJAP=\"TCAndroid\",\"qwertyuiop\""));
  if(Serial.find("OK")){
    return true;
  }
  else{
    return false;
  }
}
