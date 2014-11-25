#include <SoftwareSerial.h>
#define SSID "Tritronik Mobile"
#define PASS "Tri12@11"
#define DEST "184.106.153.149" //thingspeak.com
//#define GETCMD "GET /talkbacks/887/commands/execute?api_key=ACM8XW24UDVY1GTV HTTP/1.1\r\n\r\n"
#define GETCMD "GET /talkbacks/887/commands?api_key=ACM8XW24UDVY1GTV HTTP/1.1\r\n\r\n" //show all cmd in queue, for testing
//#define DEST "220.181.111.85" //baidu.com
//#define GETCMD "GET / HTTP/1.0\r\n\r\n"
SoftwareSerial debugPort(A3,A2); //rx,tx

void setup() {
  //hardware serial
  Serial.begin(9600);
  Serial.setTimeout(5000);
  
  //software serial
  debugPort.begin(9600);
  debugPort.println("ESP8266 Demo");
  
  //reset the wifi module
  Serial.println("AT+RST");
  while (!Serial.available());
  if(Serial.find("Ready")){
    debugPort.println("Module is ready");
  }
  else{
    debugPort.println("Module have no response");
    while(1);
  }
  
  //get or initialize mac address (future)
    
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
  while(!Serial.available());
  while(Serial.available()){
    char c = Serial.read();
    debugPort.write(c);
  }
}

void loop() {
  //AT+CIPSTART
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DEST;
  cmd += "\",80";
  Serial.println(cmd);
  while(!Serial.available());
  while(Serial.available()){
    char c = Serial.read();
    debugPort.write(c);
  }
  delay(2000);
  if(Serial.find("Error")) return;
  
  //AT+CIPSEND
  cmd = GETCMD;
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
  
  //get received data
  while(!Serial.available());
  while(Serial.available()){
    char c = Serial.read();
    debugPort.write(c);
  }
  debugPort.println("\r\n=====\r\n");
  delay(1000);
}

boolean connectWifi(){
  Serial.println("AT+CWMODE=1");
  while (!Serial.available());
  while(Serial.available()){
    char c = Serial.read();
    debugPort.write(c);
  }
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  while (!Serial.available());
  while(Serial.available()){
    char c = Serial.read();
    debugPort.write(c);
  }
  delay(2000);
  if(Serial.find("OK")){
    return true;
  }
  else{
    debugPort.println("Retry");
    return false;
  }
}
