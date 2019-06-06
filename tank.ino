
int iol = 9;
int lf = 6;
int lb = 7;
int ior = 3;
int rb = 5;
int rf = 2;
const int trigPin = 8;
long duration;
int distance=0;
int frspeed = 100;
int fscan = 30;
int tscan = 30;
char cmd[100];
int cmdIndex;
int turntime = 500;

void mov0(){
  analogWrite(iol,frspeed);
  digitalWrite(lf,HIGH);
  digitalWrite(lb,LOW);
  analogWrite(ior,frspeed);
  digitalWrite(rf,HIGH);
  digitalWrite(rb, LOW);
}
void mov1(){
  analogWrite(iol,200);
  digitalWrite(lf,HIGH);
  digitalWrite(lb,LOW);
  analogWrite(ior,100);
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
void mov3(){
  analogWrite(iol,200);
  digitalWrite(lf,LOW);
  digitalWrite(lb,HIGH);
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
void mov5(){
  analogWrite(iol,100);
  digitalWrite(lf,LOW);
  digitalWrite(lb,HIGH);
  analogWrite(ior,200);
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
void mov7(){
  analogWrite(iol,100);
  digitalWrite(lf,HIGH);
  digitalWrite(lb,LOW);
  analogWrite(ior,200);
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

void setup() {

Serial.begin(9600);
pinMode ( iol,OUTPUT);
pinMode ( lf,OUTPUT);
pinMode ( lb,OUTPUT);
pinMode ( ior,OUTPUT);
pinMode ( rf,OUTPUT);
pinMode ( rb,OUTPUT);
pinMode ( rb,OUTPUT);
analogWrite(3,0);
pinMode(trigPin, OUTPUT); 
pinMode(A0, INPUT); 
  mov8();
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
  Serial.println(distance);
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
void loop() {
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
}

