

#include <Wire.h>
#include <PixyI2C.h>

//DECLARE PINS
int E1 = 10;
int M1 = 12;
int E2 =11;
int M2 = 13;
int ledPin =  4;  
int iol = 9;
int lf = 6;
int lb = 7;
int ior = 3;
int rb = 5;
int rf = 2;
const int trigPin = 8;

//CONFIGURATIONS
int center = 102;
int error = 10;
long interval = 500;  
int frspeed = 100;
int fscan = 30;
int tscan = 30;
int turntime = 500;
int pos = 0;
//I/O
  bool b = false;  
  bool d = true;
  bool f = false;
int ledState = LOW; 
int fuegoState = LOW; 

//STARTS
int x = 0;                     
int y = 0;           
unsigned long previousMillis = 0;  
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
int spe ;
int spe2;
long duration;
int distance=0;



uint32_t lastBlockTime = 0;
PixyI2C pixy(0x55); 

void setup()
{
  Serial.begin(9600);
  pinMode ( iol,OUTPUT);
  pinMode ( lf,OUTPUT);
  pinMode ( lb,OUTPUT);
  pinMode ( ior,OUTPUT);
  pinMode ( rf,OUTPUT);
  pinMode ( rb,OUTPUT);
  pinMode ( rb,OUTPUT);
  pinMode(trigPin, OUTPUT); 
  pinMode(A0, INPUT); 
  pinMode(A1,INPUT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  mov8();
  pixy.init();
  yamete();
  retcent();
}
//-------------------------------------------------------TURRET
void left(int val){
digitalWrite(M1,HIGH);
analogWrite(E1,val);
}
void right(int val){
  digitalWrite(M1,LOW);
analogWrite(E1,val);
}

void fuego(int val){
  digitalWrite(M2,HIGH);
  analogWrite(E2,val);
}
void alarm(bool a,bool c){
    unsigned long currentMillis = millis();
if(a&&c&&d){
  if (currentMillis - previousMillis2 >= 500) {
    if(interval>=100){
    interval-=100;
    }else{
      b= true;
      d=false;
    }
    previousMillis2 = currentMillis;
  }
  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;


    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }


    digitalWrite(ledPin, ledState);
  }
}else digitalWrite(ledPin, LOW);

if(b&&a){
  if (currentMillis - previousMillis3 >= 3000){

      if (fuegoState == 255) {
      fuegoState = 0;
      d = true;
        b= false;
        f=false;
    }
  previousMillis3 = currentMillis;
    fuego(fuegoState);
  }else {
    fuegoState=255; 
    fuego(255);
  }
  
}else {
  fuego(0);
}
}
void yamete(){
  analogWrite(E1,0);
  analogWrite(E2,0);
  alarm(false,false);
}

  //-------------------------------------------------------------------------------MOVEMENT
  void mov0(){
  analogWrite(iol,frspeed);
  digitalWrite(lf,HIGH);
  digitalWrite(lb,LOW);
  analogWrite(ior,frspeed);
  digitalWrite(rf,HIGH);
  digitalWrite(rb, LOW);
}
void mov2(){
  analogWrite(iol,100);
  digitalWrite(lf,HIGH);
  digitalWrite(lb,LOW);
  analogWrite(ior,100);
  digitalWrite(rf,LOW);
  digitalWrite(rb, HIGH);
}
void mov4(){
  analogWrite(iol,frspeed);
  digitalWrite(lf,LOW);
  digitalWrite(lb,HIGH);
  analogWrite(ior,frspeed);
  digitalWrite(rf,LOW);
  digitalWrite(rb, HIGH);
}
void mov6(){
  analogWrite(iol,100);
  digitalWrite(lf,LOW);
  digitalWrite(lb,HIGH);
  analogWrite(ior,100);
  digitalWrite(rf,HIGH);
  digitalWrite(rb, LOW);
}
void mov8(){
  analogWrite(iol,0);
  digitalWrite(lf,LOW);
  digitalWrite(lb,LOW);
  analogWrite(ior,0);
  digitalWrite(rf,LOW);
  digitalWrite(rb,LOW);
}

int scan(){
  for(int i =0; i <4; i++){
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(A0, HIGH);
distance= duration*0.034/2;
  }
return distance;
}

int lookl(){
  int ans = scan();
  mov2();
  delay(turntime);
  mov8();
  delay(turntime);
  if(ans<fscan && ans>5){
      mov2();
      delay(turntime);
      mov8();
      delay(turntime);
      ans = scan();
  }else{
    ans=scan();
  }
   return ans;
}

int lookr(){
  int ans = scan();
  mov6();
  delay(turntime);
  mov8();
  delay(turntime);
  if(ans<fscan && ans>5){
      mov6();
      delay(turntime);
      mov8();
      delay(turntime);
      ans = scan();
  }else{
    ans=scan();
  }
   return ans;
}

void rev(){
  mov4();
  delay(turntime);
  mov8();
}
//-------------------------------------------------------------------------------
void target(){
  
  uint16_t blocks;
    blocks = pixy.getBlocks();
    if(blocks){
      x = pixy.blocks[0].x;                    //get x position
      y = pixy.blocks[0].y;
      spe = map(x,0,center,255,40);
      spe2 = map(x,center,319,40,255);
     if(pixy.blocks[0].signature == 2){
      mov8();
      f=true;
      if(x>(center + error)){
        left(spe2);
         alarm(true,false);
       interval = 500;
      }else if (x<(center - error)){
        right(spe);
       alarm(true,false);
      interval = 500;
      }else if (x<(center + error) && x>(center-error)){
        alarm(true,true);
      }else{
        yamete();
      }
        lastBlockTime = millis();
    }
    }else if (millis() - lastBlockTime > 100)
  { 
      f=false;
      yamete();
  }
  }
  void retcent(){
    while(pos!=1){
    int sense = digitalRead(A1);
    Serial.println(sense);
    if(pos ==0){
      while(sense ==1){
        sense = digitalRead(A1);
        left(150);
      }
      left(0);
      pos = 1;
    }else if(pos ==2){
      while(sense==0){
        sense = digitalRead(A1);
        right(150);
      }
     right(0);
     pos =1;
    }
    }
  }
//----------------------------------------------------------------------
void loop() {
  target();
  if(!f){
  int sc = scan();
  if(sc >fscan && sc<400){
    mov0();
  }else if(sc<fscan && sc>5){
    int lookl1 = lookl();
    if(lookl1<fscan && lookl1>5){
      int lookr1 = lookr();
        if(lookr1<fscan && lookr1>5){
          rev();
        }else{
          mov0();
        }
    }else{
      mov0();
    }
  }
}else{
  mov8();
}
}
  



