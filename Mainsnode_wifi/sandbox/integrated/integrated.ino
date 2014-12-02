//an integrated program for mainsnode
#include <avr/pgmspace.h>
#include <SoftwareSerial.h>
#include <EmonLib.h>

SoftwareSerial debugPort(A3,A2);
EnergyMonitor emon;

char state = 'u'; //device status unknown

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(5000);
  
  debugPort.begin(9600);
  debugPort.println(F("integrated demo"));
  
  emon.current(1,2.3923);
  emon.voltage(2,468.0851,2);
  
  Serial.println(F("AT+RST"));
  while(!Serial.available());
  if(Serial.find("Ready")){
    debugPort.println(F("ready"));
  }
  else{
    debugPort.println(F("no response"));
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
    debugPort.println(F("connected"));
  }
  else{
    debugPort.println(F("not connected"));
    while(1);
  }
  delay(1000);
  
  Serial.println(F("AT+CIPMUX=0"));
  
  pinMode(10,OUTPUT);
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
//  float power = random(60,100);
//  return power;
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
    debugPort.println(F("dt sent"));
    Serial.println(F("AT+CIPCLOSE"));
    delay(1000);
  }
  else{
    debugPort.println(F("send err"));
    Serial.println(F("AT+CIPCLOSE"));
    delay(1000);
  }
}

void getCmd(){
  Serial.println(F("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80"));
  while(Serial.available()){
    Serial.read();
  }
  while(Serial.availableCmd()){
    char c = Serial.readCmd();
    if(c=='#') state = 'f'; //state off
    else if(c=='&') state = 'n'; //state on
  }
  if(Serial.find("ERROR")) return;
  delay(1000);
  
  Serial.println(F("AT+CIPSEND=70"));
  if(Serial.find(">")){
    debugPort.print(">");
  }
  else{
    Serial.println(F("AT+CIPCLOSE"));
    debugPort.println("connect timeout");
    return;
  }
  Serial.print(F("GET /talkbacks/887/commands/last?api_key=ACM8XW24UDVY1GTV HTTP/1.1\r\n\r\n"));
  while(Serial.available()){
    Serial.read();
  }
  while(Serial.availableCmd()){
    char c = Serial.read();
    if(c=='#') state = 'f'; //state off
    else if(c=='&') state = 'n'; //state on
  }
  delay(1000);
  
  if(state == 'f'){ //turn off
    debugPort.println("turn off");
    digitalWrite(10, LOW);
  }
  else if(state == 'n'){ //turn on
    debugPort.println("turn on");
    digitalWrite(10, HIGH);
  }
  else{
    debugPort.println(state);
  }
  
  Serial.println(F("AT+CIPCLOSE"));
  debugPort.println(F("\r\n=====\r\n"));
  delay(1000);
}

boolean connectWifi(){
  Serial.println(F("AT+CWMODE=1"));
  delay(1000);
  Serial.println(F("AT+CWJAP=\"Tritronik Mobile\",\"Tri12@11\""));
  if(Serial.find("OK")){
    return true;
  }
  else{
    return false;
  }
}
