#include <SoftwareSerial.h>
SoftwareSerial debugPort(A3,A2);

void setup(){
  Serial.begin(9600);
  Serial.println("HW - This is a normal string");
  debugPort.begin(9600);
  debugPort.println("SW - OK then normal string");
}

void loop(){
  if(debugPort.available()){
    debugPort.write(Serial.read());
  }
  if(Serial.available()){
    Serial.write(debugPort.read());
  }
}
