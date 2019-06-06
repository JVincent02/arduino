//led pin2-7;switch series with ground pin 11-12
unsigned long instance[]={0,0};
int pin=2,prevPin=2,mode=0,iteration=0;
boolean forward=true,state=true;
void setup() {
for(int i=2;i<=7;i++)
pinMode(i,OUTPUT);
pinMode(11,INPUT_PULLUP);
pinMode(12,INPUT_PULLUP);
}

void loop() {
  if(digitalRead(11)==LOW){
    mode=1;
    iteration=10;
    digitalWrite(prevPin,LOW);
  }
  if(digitalRead(12)==LOW){
    mode=2;
    pin=2;
    forward=true;
     for(int i=2;i<=7;i++)
    digitalWrite(i,LOW);
  }
  switch(mode){
    case 1:blinkLed(500);break;
    case 2:scrollLed(300);break;
  }

}

void blinkLed(int interval){
    if(checkTime(interval)&&iteration>0){
      for(int i=2;i<=7;i++)
      digitalWrite(i,state);
      state=!state;
      if(state)iteration--; 
    }
}
void scrollLed(int interval){
  if(checkTime(interval)){
    digitalWrite(prevPin,LOW);
    digitalWrite(pin,HIGH);
    prevPin=pin;
    if(forward)pin++;
    else pin--;
    if(pin==2||pin==7) forward=!forward;
  }
}

boolean checkTime(int duration){
  instance[1]=millis();
  if(instance[1]-instance[0]>=duration){
    instance[0]=instance[1];
    return true;
  }
  return false;
}

