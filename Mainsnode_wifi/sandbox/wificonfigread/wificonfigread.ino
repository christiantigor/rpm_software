//load wifi config from EEPROM
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

SoftwareSerial debugPort(A3,A2);

void setup(){
  debugPort.begin(9600);
  debugPort.println(F("wifi config read demo"));
}

void loop(){
  //read wifi ssid
  boolean readCont = true;
  byte addr = 0;
  while(readCont){
    char c = EEPROM.read(addr);
    if(c == '>'){
      readCont = false;
    }
    else{
      debugPort.print(c);
      addr++;
    }
  }
  debugPort.println();
  //read wifi password
  readCont = true;
  addr = 128;
  while(readCont){
    char c = EEPROM.read(addr);
    if(c == ')'){
      readCont = false;
    }
    else{
      debugPort.print(c);
      addr++;
    }
  }
  while(1);
}
