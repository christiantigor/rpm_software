#include <avr/pgmspace.h>
#include <SoftwareSerial.h>
SoftwareSerial debugPort(A3,A2);

prog_char getUpdate[] PROGMEM = "GET /update?api_key=99N5T6AH3NY4UKNA&field1=";
prog_char getCmd[] PROGMEM = "Get command nich";
String sensorData;
long rndmNum;
char rndmBuffer[10];
String strGetUpdate;

PROGMEM const char *atCommand[] = {
  getUpdate,
  getCmd
};

char buffer[30];

void setup(){
  Serial.begin(9600);
  debugPort.begin(9600);
}

void loop(){
  rndmNum = random(60,100);
  sensorData = dtostrf(rndmNum,4,1,rndmBuffer);
  strcpy_P(buffer,(char*)pgm_read_word(&(atCommand[0])));
  strGetUpdate = buffer;
  strGetUpdate += sensorData;
  debugPort.println(strGetUpdate);
  delay(1000);
  strcpy_P(buffer,(char*)pgm_read_word(&(atCommand[1])));
  Serial.println(buffer);
  delay(1000);
}
