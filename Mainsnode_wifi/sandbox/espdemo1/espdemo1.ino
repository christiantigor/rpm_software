//this is a program to get command from cloud, use arduino platform with 768 serial buffer size
#include <avr/pgmspace.h>
#include <SoftwareSerial.h>
SoftwareSerial debugPort(A3,A2);

//"AT+CWJAP=\"Tritronik Mobile\",\"Tri12@11\""
//"AT+CIPSTART=\"TCP\",\"184.106.153.149\",80" //thingspeak.com
//String cmd = "GET /talkbacks/887/commands/last?api_key=ACM8XW24UDVY1GTV HTTP/1.1\r\n\r\n"; //length 70
//String cmd = "GET / HTTP/1.0\r\n\r\n";

char state = 'u'; //device state, unknown

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
  
  pinMode(10,OUTPUT);
}

void loop(){
  Serial.println(F("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80"));
  while(Serial.available()){
    char c = Serial.read();
    if(c=='#') state = 'f'; //state off
    else if(c=='&') state = 'n'; //state on
    debugPort.write(c);
  }
  if(Serial.find("Error")) return;
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
    char c = Serial.read();
    if(c=='#') state = 'f'; //state off
    else if(c=='&') state = 'n'; //state on
    debugPort.write(c);
  }
  delay(1000);
  
  //start - working if debugPort.write(c) is not commented
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
  //end not fully working
  
  Serial.println(F("AT+CIPCLOSE"));
  debugPort.println(F("\r\n=====\r\n"));
  delay(1000);
}

boolean connectWifi(){
  Serial.println("AT+CWMODE=1");
  delay(1000);
  Serial.println(F("AT+CWJAP=\"Tritronik Mobile\",\"Tri12@11\""));
  if(Serial.find("OK")){
    return true;
  }
  else{
    return false;
  }
}
