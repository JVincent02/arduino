#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Servo.h>

const char *ssid = "Vincent";  
const char *pw = ""; 
const int port = 1234;
String msg;
char c;
int pos=0;

IPAddress ip(192, 168, 0, 1);
IPAddress netmask(255, 255, 255, 0);
WiFiServer server(port);
WiFiClient client;
Servo pitch;


void setup() {
Serial.begin(9600);
  WiFi.softAPConfig(ip, ip, netmask); 
  WiFi.softAP(ssid, pw);     
  server.begin();
   Serial.println((String)"SSID: " + ssid + "  PASS: " + pw);
  Serial.println((String) ip.toString() + ":" + port);
  pitch.attach(5);
}

void loop() {
   if(!client.connected()) {
    client = server.available();
    return;
  }
  if(client.available()) {
    
    char c = (char)client.read();
    Serial.print(c);
    if(client.available()==0)Serial.println();
  }
if(Serial.available()){
  c=(char)Serial.read();
  if(c!='\n')msg+=c;
  if(Serial.available()==0){
    client.println(msg+".");
    msg="";
  }
}

}
