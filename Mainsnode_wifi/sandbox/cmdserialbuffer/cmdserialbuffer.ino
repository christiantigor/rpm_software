#include <SoftwareSerial.h>
SoftwareSerial debugPort(A3,A2);

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(5000);
  
  debugPort.begin(9600);
  debugPort.println("cmd serial buffer");
  delay(1000);
}

void loop(){
  Serial.println("hello");
  while(Serial.availableCmd()){
    char c = Serial.readCmd();
    debugPort.write(c);
  }
  delay(2000);
  
  Serial.println("world&");
  while(Serial.availableCmd()){
    char c = Serial.readCmd();
    debugPort.write(c);
  }
  delay(2000);
  
  Serial.println("cup#");
  while(Serial.availableCmd()){
    char c = Serial.readCmd();
    debugPort.write(c);
  }
  delay(2000);
  debugPort.println("-----");
}
