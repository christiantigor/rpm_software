//this is a program to send data to cloud
#include <avr/pgmspace.h>
#include <SoftwareSerial.h>
SoftwareSerial debugPort(A3,A2);

//String updtDt = "GET /update?api_key=99N5T6AH3NY4UKNA&field1="; //length 44(without data) OR 48(with data)

//check channel feed: http://api.thingspeak.com/channels/19081/feed.json?key=99N5T6AH3NY4UKNA&start=2014-01-01%2000:00:00&end=2014-12-31%2000:00:00

//init sendDur and sendInc
int sendDur = 15; //in loops
int sendInc = 0;

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(5000);
  
  debugPort.begin(9600);
  debugPort.println(F("ESP8266 Demo"));
  Serial.println(F("AT+RST"));
  while(!Serial.available());
  if(Serial.find("Ready")){
    debugPort.println(F("Ready"));
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
  
  //set relay pin
  pinMode(10,OUTPUT);
}

void loop(){
  //calculate power
  float power = random(60,100);
  //debugPort.print(F("ram"));
  //debugPort.println(freeRam());
  
  //send data
//  debugPort.print(F("GET /update?api_key=99N5T6AH3NY4UKNA&field1="));
//  debugPort.println(power);
//  delay(1000);
//  sendData(power);
//  delay(1000);
  if(sendInc > sendDur){
    sendData(power);
    sendInc = 0;
  }
  sendInc++;
}

boolean connectWifi(){
  Serial.println(F("AT+CWMODE=1"));
  delay(1000);
  //Serial.println(F("AT+CWJAP=\"Tritronik Mobile\",\"Tri12@11\""));
  Serial.println(F("AT+CWJAP=\"TCAndroid\",\"qwertyuiop\""));
  if(Serial.find("OK")){
    return true;
  }
  else{
    return false;
  }
}

void sendData(float data){
  Serial.println(F("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80"));
  delay(2000);
  if(Serial.find("ERROR")) return;
  
  char temp[6];
  String strData;
  String cmd = "GET /update?api_key=99N5T6AH3NY4UKNA&field1=";
  dtostrf(data,4,1,temp);
  strData = String(temp);
  cmd += strData;
  debugPort.println(freeRam());
  debugPort.println(cmd);
  Serial.print(F("AT+CIPSEND=")); //nanti harus diganti PANJANGNYA
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
  }
  else{
    debugPort.println(F("send err"));
    Serial.println(F("AT+CIPCLOSE"));
  }
}

int freeRam(){
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
