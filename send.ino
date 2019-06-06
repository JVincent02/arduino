bool bul = HIGH;
float pmil=0;
float p2mil =0;
int pre =0;
int d = 8;
float cnt = 1;
bool a[] = {1,1,1,1,0,0,0,0,0};
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(9,OUTPUT);
}

void loop() {
  float cmil = millis();
  int dat = analogRead(A0);
  if(abs(pre -dat) >10){
    Serial.println(cmil-p2mil);
    p2mil = cmil;
  }
pre = dat;
senddata(cmil);

}
void senddata(float cmil){
  if ((cmil-pmil)>=cnt){
  if (d==0) d=8;
  pmil=cmil;
  if(a[d] != a[d+1]){
    digitalWrite(9,a[d]);
  }
  d--;
}
}

