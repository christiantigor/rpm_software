//testing write and read eeprom
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

SoftwareSerial debugPort(A3,A2);

void setup(){
  debugPort.begin(9600);
  debugPort.println(F("eeprom test"));
}

void loop(){
  boolean readCont = true;
  byte i = 0;
  while(readCont){
    char c = EEPROM.read(i);
    if(c == '>'){
      readCont = false;
    }
    else{
      debugPort.print(c);
      i++;
    }
  }
  debugPort.println();
  while(1);
}
