unsigned long instance[]={0,0};
int iteration=10;
boolean state=true;
void setup() {
for(int i=2;i<=7;i++)
pinMode(i,OUTPUT);
}

void loop() {
blinkLed(500);
}

void blinkLed(int interval){
    if(checkTime(interval)&&iteration>0){
      for(int i=2;i<=7;i++)
      digitalWrite(i,state);
      state=!state;
      if(state)iteration--; 
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

