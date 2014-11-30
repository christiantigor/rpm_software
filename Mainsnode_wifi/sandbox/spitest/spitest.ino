#include <SPI.h>

char outByte[10] = "SPI Test";
char inByte;

void setup(){
  Serial.begin(4800);
  SPI.begin();
  delay(100);
}

void loop(){
  for(int i=0; outByte[i]!='\0';i++){
    inByte = SPI.transfer(outByte[i]);
    Serial.write(inByte);
  }
  Serial.println();
  delay(1000);
}
