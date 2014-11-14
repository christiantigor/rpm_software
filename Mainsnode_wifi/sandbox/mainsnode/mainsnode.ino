#include <SoftwareSerial.h>
#define SSID "Tritronik Mobile 2"
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
    debugPort.println("Finish trying, connected to WiFi");
  }
  else{
    debugPort.println("Finish trying, error connecting to WiFi");
    while(1);
  }
}

void loop() {
  ;
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
    debugPort.println("Connected to WiFi");
    return true;
  }
  else{
    debugPort.println("Trying to connect to WiFi");
    return false;
  }
}
