unsigned long instance[]={0,0};
int pin=2,prevPin=2;
boolean forward=true;
void setup() {
for(int i=2;i<=7;i++)
pinMode(i,OUTPUT);
}

void loop() {
scrollLed(300);
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

