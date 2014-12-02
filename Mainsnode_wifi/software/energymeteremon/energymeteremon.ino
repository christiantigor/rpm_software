#include <EmonLib.h>

EnergyMonitor emon;

void setup(){
  Serial.begin(4800);
  emon.current(1,2.3923);
  emon.voltage(2,468.0851,2); //with PHASECAL 2, PF = 0.94
                              //PF is rarely used in household device
}

void loop(){
  //double Irms = emon.calcIrms(1480);
  //Serial.println(Irms);
  //delay(2000);
  
  emon.calcVI(20,2000);
  float Irms = emon.Irms;
  float Vrms = emon.Vrms;
  float realPower = emon.realPower;
  float apparentPower = emon.apparentPower;
  float powerFactor = emon.powerFactor;
  
  Serial.print("Irms: ");
  Serial.println(Irms);
  Serial.print("Vrms: ");
  Serial.println(Vrms);
  Serial.print("realPower: ");
  Serial.println(realPower);
  Serial.print("apparentPower: ");
  Serial.println(apparentPower);
  Serial.print("powerFactor");
  Serial.println(powerFactor);
  
  Serial.println();
  delay(2000);
}
