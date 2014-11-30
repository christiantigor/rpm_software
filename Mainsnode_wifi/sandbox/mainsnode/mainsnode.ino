#include <SoftwareSerial.h>
#define SSID "Tritronik Mobile"
#define PASS "Tri12@11"
//#define DEST "184.106.153.149" //thingspeak.com
#define SENDDATA "GET /update?api_key=99N5T6AH3NY4UKNA&field1="
//#define GETCMD "GET /talkbacks/887/commands/execute?api_key=ACM8XW24UDVY1GTV HTTP/1.1\r\n\r\n"
//#define GETCMD "GET /talkbacks/887/commands?api_key=ACM8XW24UDVY1GTV HTTP/1.1\r\n\r\n" //show all cmd in queue, for testing
#define DEST "220.181.111.85" //baidu.com
#define GETCMD "GET / HTTP/1.0\r\n\r\n"
SoftwareSerial debugPort(A3,A2); //rx,tx
int indicator;
long randNum;
int timer1Counter;
int sendDur;
int sendInc;
String buffer = "";

void setup() {
  //hardware serial
  Serial.begin(19200);
  Serial.setTimeout(5000);
  
  //software serial
  debugPort.begin(19200);
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
  
  //set for single connection
  Serial.println("AT+CIPMUX=0");
  Serial.flush();
  
  //initialize sendDur and sendInc
  sendDur = 15; //in seconds
  sendInc = 0;
  
  //initialize timer1
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  timer1Counter = 31250; //one second of 8MHz/256 = 31.25kHz
  
  TCNT1 = timer1Counter;
  TCCR1B |= (1 << CS12); //256 prescaller
  TIMSK1 |= (1 << TOIE1); //enable timer overflow interrupt
  interrupts();
}

ISR(TIMER1_OVF_vect){
  TCNT1 = timer1Counter;
  sendInc++;
  //debugPort.println("one sec");
}

void loop() {
  //indicator
  debugPort.println(indicator);
  
  //read power measurement
  randNum = random(25,30);
  char buffer[10];
  String power = dtostrf(randNum, 4, 1, buffer);
  //send power measurement
  if(sendInc > sendDur){
    //sendPower(power); //must not be commented
    sendInc = 0;
  }
  
  //getcmd
  getCmd();
  
  //delay(1000);
  indicator++;
  
//  debugPort.println(indicator);
//  //AT+CIPSTART
//  String cmd = "AT+CIPSTART=\"TCP\",\"";
//  cmd += DEST;
//  cmd += "\",80";
//  Serial.println(cmd);
//  delay(2000);
//  //Serial.flush();
//  if (Serial.find("ERROR")){
//    //debugPort.println("cipstart error");
//    return;
//  }
//  
//  //AT+CIPSEND
//  cmd = GETCMD;
//  Serial.print("AT+CIPSEND=");
//  Serial.println(cmd.length());
//  if(Serial.find(">")){
//    debugPort.print(">");
//  }
//  else{
//    Serial.println("AT+CIPCLOSE");
//    //Serial.flush();
//    debugPort.println("connect timeout");
//    delay(1000);
//    return;
//  }
//  Serial.print(cmd);
//  delay(2000);
//  
//  //get received data
//  while(Serial.available()){
//    char c = Serial.read();
//    debugPort.write(c);
//  }
//  debugPort.println("\r\n=====\r\n");
//  indicator++;
//  delay(1000);
}

//send power measurement
void sendPower(String power){
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DEST;
  cmd += "\",80";
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("ERROR")) return;
  
  cmd = SENDDATA;
  cmd += power;
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if(Serial.find(">")){
    Serial.print(cmd);
  }
  else{
    Serial.println("AT+CIPCLOSE");
    delay(1000);
    return;
  }
  if(Serial.find("OK")){
    debugPort.println("Data sent");
    Serial.println("AT+CIPCLOSE");
  }
  else{
    debugPort.println("Sent Error");
    Serial.println("AT+CIPCLOSE");
  }
}

//getcmd
void getCmd(){
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DEST;
  cmd += "\",80";
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("ERROR")){
    debugPort.println("cipstart error");
    return;
  }
  
//  Serial.println("AT+CIPSTATUS");
//  delay(2000);
//  while(Serial.available()){
//    char c = Serial.read();
//    debugPort.write(c);
//  }
  
  cmd = GETCMD;
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if(Serial.find(">")){
    debugPort.print(">");
    Serial.print(cmd);
    delay(2000);
    while(Serial.available()){
      char c = Serial.read();
      buffer.concat(c);
    }
  }
  else{
    Serial.println("AT+CIPCLOSE");
    debugPort.println("connect timeout");
    delay(1000);
    return;
  }
  
  //int cmdOn = buffer.indexOf("ON");
  //int cmdOff = buffer.indexOf("OFF");
  //debugPort.println(buffer.length());
  //debugPort.println(buffer.substring(buffer.length()-60));
  //debugPort.println(cmdOn);
  //debugPort.println(cmdOff);
  
//  if(buffer.indexOf("TurnOn")){
//    debugPort.println("Turn On");
//  }
//  else if(buffer.indexOf("TurnOff")){
//    debugPort.println("Turn Off");
//  }
//  else{
//    debugPort.println("cmd unknown/not found");
//  }
  debugPort.println("\r\n=====\r\n");
  delay(1000);
}

//connect WiFi
boolean connectWifi(){
  Serial.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("OK")){
    return true;
  }
  else{
    debugPort.println("Retry");
    return false;
  }
}
