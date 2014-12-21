//load wifi config from EEPROM
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

SoftwareSerial debugPort(A3,A2);

byte val;

void setup(){
  debugPort.begin(9600);
  debugPort.println(F("wifi config read demo"));
}

void loop(){
  for(byte addr = 0; addr < 17; addr++){
    val = EEPROM.read(addr);
    debugPort.print(val);
  }
  debugPort.println();
  while(1);
}
