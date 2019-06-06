#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Servo.h>
#include <Wire.h>

const uint8_t MPU6050SlaveAddress = 0x68;


const uint8_t scl = D1;
const uint8_t sda = D2;


const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;


const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;

int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;

int setpitch=65,posx=90,nAx;

unsigned int prev=0,cur=0;






// config:
const char *ssid = "Vincent";  
const char *pw = ""; 
IPAddress ip(192, 168, 0, 1);
IPAddress netmask(255, 255, 255, 0);
const int port = 1234;

WiFiServer server(port);
WiFiClient client;
Servo servoTilt,servoPan;
int x=0,y=0,z=0;
char cmd[100];
int cmdIndex;
String sx="",sy="",sz="",sf="";
int offset =0;
boolean start =true;
void setup() {

  
  pinMode(BUILTIN_LED,OUTPUT);        
  
  for(int i=0; i<5;i++){    
  digitalWrite(BUILTIN_LED,HIGH);
  delay(100);
  digitalWrite(BUILTIN_LED,LOW);
  delay(100);
  }
    
  Serial.begin(115200);

  WiFi.softAPConfig(ip, ip, netmask);
  WiFi.softAP(ssid, pw); 

  server.begin(); // start TCP server

  cmdIndex=0;
  servoTilt.attach(12);
  servoPan.attach(14);
  servoTilt.write(90);
  servoPan.write(90);
  pinMode(0,OUTPUT);
  digitalWrite(0,LOW);
  Wire.begin(sda, scl);
  MPU6050_Init();
}

void loop() {
   if(!client.connected()) {
    client = server.available();
    return;
  }
  cur=millis();
 double Ax, Ay, Az, T, Gx, Gy, Gz;
  Ax = (double)AccelX/AccelScaleFactor;
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
   nAx=90-(Ax*90);
 if((cur-prev)>10) {
  if(abs(nAx-setpitch)>10){
  if(nAx>setpitch)posx--;
  else posx++;
  }
  if(posx<0)posx=0;
  if(posx>180)posx=180;
  

  servoTilt.write(posx);
  prev=cur;
 }

  if(client.available()) {
    
    char c = (char)client.read();
    Serial.print(c);
    if(c=='\n') {
      cmd[cmdIndex] = 0;
      exeCmd();  
      cmdIndex = 0; 
    } else {      
      cmd[cmdIndex] = c;
      if(cmdIndex<99) cmdIndex++;
    }
   
    
  }
  
}

boolean isFound=false ;
int t=-1;
int nx=90;
void exeCmd(){
  sx="";sy="";sz="";sf="";
  t=-1;
for(int i=0;i<100;i++){
    if(cmd[i]=='X'){
      isFound = true;
      i+=2;
      t=0;
    }else if(cmd[i]=='Y'){
      isFound = true;
      i+=2;
      t=1;
    }else if(cmd[i]=='Z'){
      isFound = true;
      i+=2;
      t=2;
    }else if(cmd[i]=='F'){
      isFound = true;
      i+=2;
      sf+=cmd[i];
      if(sf.toInt()==1)
      digitalWrite(0,HIGH);
      else digitalWrite(0,LOW);
    }else if(cmd[i]=='C'){
      start=true;
    }
    if(isFound){
      if(t==0)sx+=cmd[i];
      else if(t==1)sy+=cmd[i];
      else if(t==2)sz+=cmd[i];
    }
    if(cmd[i]==0)isFound=false;
  }
  if(t==0){
    x=sx.toInt();
    if(start) {
      offset=x-90;
      start=false;
    }
    if(x<=offset-180)
      nx=x+360-offset;
     else if(x>=360+offset)
      nx=360-x-offset;
    else {
      nx=x-offset;
      if(nx>180) nx=180;
      if(nx<0)nx=0;
    }
    servoPan.write(map(nx,0,180,180,0));
   
  }
  if(t==1) {
    y=sy.toInt();
  }
  if(t==2){
    z=sz.toInt();
   z-=25;
     if(z>180) z=180;
      if(z<0)z=0;
      setpitch=z;
  }
}
void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}


void MPU6050_Init(){
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}



