#include <SoftwareSerial.h>
#define SSID "Tritronik Mobile"
#define PASS "Tri12@11"
#define DEST "173.194.117.3" //google.com
SoftwareSerial debugPort(A3,A2); //rx,tx

void setup() {
  //hardware serial
  Serial.begin(9600);
  Serial.setTimeout(5000);
  
  //software serial
  debugPort.begin(4800);
  debugPort.println("ESP8266 Demo");
  
  //reset the wifi module
  Serial.println("AT+RST");
  while (!Serial.available());
  //debugPort.write(Serial.read());
  if(Serial.find("Ready")){
    debugPort.println("Module is ready");
  }
  else{
    debugPort.println("Module have no response");
    while(1);
  }
    
  //connect to wifi
  boolean isConnect=false;
  for(int i=0;i<5;i++){
    if(connectWifi()){
      isConnect=true;
      break;
    }
  }
  if(isConnect){
    debugPort.println("Finish trying, connected");
  }
  else{
    debugPort.println("Finish trying, error");
    while(1);
  }
  
  //connect to destination
  Serial.println("AT+CIPMUX=0");
}

void loop() {
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DEST;
  cmd += "\",80";
  debugPort.println(cmd);
  Serial.println(cmd);
  while (!Serial.available());
  debugPort.write(Serial.read());
  delay(2000);
  if(Serial.find("Error")) return;
  cmd = "GET / HTTP/1.0\r\n\r\n";
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if(Serial.find(">")){
    debugPort.print(">");
  }
  else{
    Serial.println("AT+CIPCLOSE");
    debugPort.println("connect timeout");
    delay(1000);
    return;
  }
  Serial.print(cmd);
  delay(2000);
  while(Serial.available()){
    char c = Serial.read();
    debugPort.write(c);
    if(c=='\r') debugPort.print('\n');
  }
  debugPort.println("====");
  delay(1000);
}

boolean connectWifi(){
  Serial.println("AT+CWMODE=1");
  while (!Serial.available());
  //debugPort.write(Serial.read());
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  //debugPort.println(cmd);
  Serial.println(cmd);
  while (!Serial.available());
  debugPort.write(Serial.read());
  delay(2000);
  if(Serial.find("OK")){
    //debugPort.println("Connected to WiFi");
    return true;
  }
  else{
    debugPort.println("Retry");
    return false;
  }
}
