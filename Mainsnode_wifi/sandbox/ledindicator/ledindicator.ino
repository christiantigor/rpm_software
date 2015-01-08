void setup(){
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT); //verified, not A0
  pinMode(A2,OUTPUT);
}

void loop(){
  digitalWrite(A0,HIGH);
  digitalWrite(A1,HIGH);
  digitalWrite(A2,HIGH);
  delay(3000);
}
