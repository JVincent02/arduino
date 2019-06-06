//2-6 yung led mo tapos 7 yung button na nakaseries lng sa ground
void setup() {
  // put your setup code here, to run once:
for(int i=2;i<=6;i++)
pinMode(i,OUTPUT);
pinMode(7,INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
 if(digitalRead(7)==0){
  delay(50);
for(int i=1000;i<=5000;i+=1000)
  for(int j=2;j<=6;j++)
    turnOn(j,i);
}
}
void turnOn(int pin,int wait){
  for(int i=2;i<=6;i++){
    if(i==pin)digitalWrite(i,HIGH);
    else digitalWrite(i,LOW);
  }
  delay(wait);
}

