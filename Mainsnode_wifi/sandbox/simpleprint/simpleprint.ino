void setup(){
  Serial.begin(9600);
  Serial.setTimeout(5000);
  
  Serial.println("setup");
  while(!Serial.available());
  while(1);
}

void loop(){
  Serial.println("loop");
  delay(2000);
}
