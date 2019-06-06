#define TRIG              28
#define ECHO              26
#define VCC1              40
#define VCC2              A14
#define GND1              42
#define GND2              A5
#define IR                34
enum us_sensor_phase {
  trigger, 
  sending, 
  receive 
};
enum us_sensor_phase ultrasonicPhase = trigger;
unsigned long ultrasonicPhaseTime = 0;        
unsigned long ultrasonicPulseDuration = 0;           
unsigned int ultrasonicDistanceInCentimeters = 0;    
unsigned int lastUltrasonicDistanceInCentimeters = 0; 
int blackdifference = 40;
int left[3]={54,55,38};//step,dir,ena
int right[3]={60,61,56};
int irsensor[4]={3,11,10,15};
int mode[4];
int maxDev[4];
int getdata[32];
int getdev[32];
int getdist[32];
int reading1[4][8];
int reading2[4][8];
int lag=0;
int filterdistance=0;
int filtercount=0;
int stepcount=0;
boolean lastleft=false;
boolean justavoided=false;
int lastavoid=0;
unsigned long t[4]={0,0,0,0};
unsigned int distance = 0;
boolean leftdir[4] ={0,1,0,1};
boolean rightdir[4]={0,1,1,0};
boolean stp = false;
boolean detect[4]={false,false,false,false};
boolean enemyahead=false;
boolean avoid=false;
boolean irdata[32];
void setup() {
  for(int i=0;i<32;i++)
    irdata[i]=false;
  Serial.begin(9600);
  //Set pins
  for(int i=0;i<3;i++){
    pinMode(left[i],OUTPUT);
    pinMode(right[i],OUTPUT);
  }
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(VCC1,OUTPUT);
  pinMode(VCC2,OUTPUT);
  pinMode(GND1,OUTPUT);
  pinMode(GND2,OUTPUT);
  pinMode(IR,INPUT_PULLUP);
  //Initial states
  digitalWrite(left[2], LOW);
  digitalWrite(right[2], LOW);
  digitalWrite(VCC1,HIGH);
  digitalWrite(VCC2,HIGH);
  digitalWrite(GND1,LOW);
  digitalWrite(GND2,LOW);
  calibrate();
  delay(2000);
}
void loop(){
  refresh();
  sampleSensor();
 // for(int i=0;i<4;i++)
 // Serial.print((String)(analogRead(irsensor[i])-mode[i])+" , ");
 // Serial.println();
  if(checkIfClear(detect)){
    avoid=true;
    stepcount=0;
    }else{
         if(filterdistance<50&&filterdistance>15){
    directMov(0,7);
    lastleft=true;
  }if((filterdistance>50&&enemyahead)||(filterdistance<=15&&enemyahead)&&lastleft){
    directMov(3,2);
  }if(filterdistance<=15&&enemyahead){
    lastleft=true;
  }if((filterdistance<=15&&!enemyahead)&&!lastleft){
    directMov(2,4);
  }if(filterdistance<=15&&!enemyahead&&lastleft){
    directMov(0,7);
  }if(filterdistance>50&&!enemyahead){
    lastleft=false;
    if(lastavoid==2)directMov(2,5);
    else directMov(3,5);
  }
    }
//--------------------------------------------------
     while(avoid){
      refresh();
      sampleSensor();
      for(int i=0;i<4;i++)
      if(detect[i]){
        if(i!=lastavoid)stepcount=0;
        lastavoid=i;
      }
      switch(lastavoid){
        case 0:
        radialback();
        break;
        case 2:
        radialbackleft();
        break;
        case 3:
        radialbackright();
        break;
        case 1:
        desperation();
        break;
      }
    }
  }
  
/*
void loop () {
  refresh();
  //for(int i=0;i<4;i++)
  //Serial.print((String)(analogRead(irsensor[i])-mode[i])+" , ");
  //Serial.println();
  sampleSensor();
  if(!checkIfClear(detect)&&!avoid){
   if(filterdistance<50&&filterdistance>15){
    directMov(0,7);
    lastleft=true;
  }if((filterdistance>50&&enemyahead)||(filterdistance<=15&&enemyahead)&&lastleft){
    directMov(3,2);
  }if(filterdistance<=15&&enemyahead){
    lastleft=true;
  }if((filterdistance<=15&&!enemyahead)&&!lastleft){
    directMov(2,4);
  }if(filterdistance<=15&&!enemyahead&&lastleft){
    directMov(0,7);
  }if(filterdistance>50&&!enemyahead){
    lastleft=false;
    directMov(2,5);
  }
  }
  ///Serial.println(ultrasonicDistanceInCentimeters);
   //Serial.println((String)(detect[0])?"black":"white"+" , "+(String)(detect[1])?"black":"white"+" , "+(String)(detect[2])?"black":"white"+" , "+(String)(detect[3])?"black":"white");
Serial.println((String)(analogRead(irsensor[0])-mode[0]));
//Serial.println((String)(detect[0])?"black":"white");
//if(enemyahead)Serial.println("Black");
//else Serial.println("White");
}
*/
void refresh(){
  t[0] = micros();
  pingPong();
}
void directMov(int dir,int rps){
      digitalWrite(left[1]   , leftdir[dir]);
      digitalWrite(right[1]  , rightdir[dir]);
      if((t[0]-t[1])>=((500.00)/rps)-lag){
      stp=!stp;
      t[1]=t[0];
      lag=0;
      }
      digitalWrite(left[0],stp);
      digitalWrite(right[0],stp);
}
void calibrate(){
      digitalWrite(left[1]   , leftdir[2]);
      digitalWrite(right[1]  , rightdir[2]);
      for(int i=0;i<8;i++){
        for(int p=0;p<4;p++)
          getdata[i+(8*p)]=analogRead(irsensor[p]);
      for(int stpcnt=0;stpcnt<1050;stpcnt++){
        digitalWrite(left[0],HIGH);
        digitalWrite(right[0],HIGH);
        delayMicroseconds(20);
        digitalWrite(left[0],LOW);
        digitalWrite(right[0],LOW);
        delayMicroseconds(400);
      }
      }
      for(int i=0;i<4;i++){
        for(int p=0;p<8;p++){
          reading2[i][p]=getdata[p+(8*i)];
          //Serial.println(reading[i]);
        }
        mode[i]=getMode(reading2[i]);
      }
          // Serial.println((String)mode[0]+" , "+mode[1]+" , "+mode[2]+" , "+mode[3]);
      digitalWrite(left[1]   , leftdir[3]);
      digitalWrite(right[1]  , rightdir[3]);
      for(int i=0;i<8;i++){
        for(int p=0;p<4;p++)
          getdev[i+(8*p)]=abs(analogRead(irsensor[p])-mode[p]);
      for(int stpcnt=0;stpcnt<1050;stpcnt++){
        digitalWrite(left[0],HIGH);
        digitalWrite(right[0],HIGH);
        delayMicroseconds(20);
        digitalWrite(left[0],LOW);
        digitalWrite(right[0],LOW);
        delayMicroseconds(400);
      }
      }
      for(int i=0;i<4;i++){
        for(int p=0;p<8;p++){
          reading1[i][p]=getdev[p+(8*i)];
       //   Serial.println(reading1[i][p]);
        }
        maxDev[i]=getMaxDev(reading1[i]);
        //Serial.println(maxDev[i]);
      }
     Serial.println((String)maxDev[0]+" , "+maxDev[1]+" , "+maxDev[2]+" , "+maxDev[3]);
}
void sampleSensor(){
  if(t[0]-t[2]>=100000){
  for(int i=0;i<4;i++)
  detect[i]=(abs(analogRead(irsensor[i])-mode[i])>maxDev[i])?true:false;
  t[2]=t[0];
  lag=460;
  }
}
int getMode(int val[8]){
  int modecnt=0,temp=0,num=0;
  for(int i=0;i<7;i++){
    for(int p=i+1;p<8;p++){
    if(val[i]==val[p])temp++;
    }
    if(temp>modecnt){
      modecnt=temp;
      temp=0;
      num=i;
    }
  }
  return val[num];
}
int getMode2(int val[32]){
  int modecnt=0,temp=0,num=0;
  for(int i=0;i<31;i++){
    for(int p=i+1;p<32;p++){
    if(val[i]==val[p])temp++;
    }
    if(temp>modecnt){
      modecnt=temp;
      temp=0;
      num=i;
    }
  }
  return val[num];
}
int getMaxDev(int dev[8]){
  int maxDev=0;
  for(int i=0;i<8;i++)
    if(dev[i]>maxDev&&dev[i]<=100)maxDev=dev[i];
    return maxDev+blackdifference;
}
void pingPong(){
  switch(ultrasonicPhase) {
  case trigger:
    ultrasonicPhase = sending;
    digitalWrite(TRIG, LOW);
    delayMicroseconds(10);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(20);
    digitalWrite(TRIG, LOW);
    ultrasonicPhaseTime = micros();
    break;
  case sending:
    if((micros() - ultrasonicPhaseTime) > 500UL ) {
      ultrasonicPhase = trigger;
    } else if(digitalRead(ECHO) == HIGH) {
      ultrasonicPhase = receive;
      ultrasonicPhaseTime = micros();
    }
    break;
  case receive:
    if((micros() - ultrasonicPhaseTime) > 20000UL) {
      ultrasonicPhase = trigger;
    } else if(digitalRead(ECHO) == LOW) {
      ultrasonicPulseDuration = (micros() - ultrasonicPhaseTime);
      ultrasonicDistanceInCentimeters = ultrasonicPulseDuration / 58.2;
      ultrasonicPhase = trigger;
    }
    break;
  }

  if(ultrasonicDistanceInCentimeters != lastUltrasonicDistanceInCentimeters) {
    lastUltrasonicDistanceInCentimeters = ultrasonicDistanceInCentimeters;
    getdist[filtercount]=ultrasonicDistanceInCentimeters;
    irdata[filtercount]=!digitalRead(IR);
    if(filtercount<31)filtercount++;
    else {
      filtercount=0;
      filterdistance=getMode2(getdist);
      enemyahead=checkIfAllFalse(irdata);
    }
  }
}
boolean checkIfAllFalse(boolean getirdata[32]){
  for(int i=0;i<32;i++)
    if(getirdata[i]) return true;
  return false;
}
boolean checkIfClear(boolean getavoid[4]){
    for(int i=0;i<4;i++)
    if(getavoid[i]) return true;
  return false;
}
void radialbackleft(){
      digitalWrite(right[1],1);
      if((t[0]-t[1])>=accelerator(stepcount)&&stepcount<16000){
      stp=!stp;
      t[1]=t[0];
      stepcount++;
      if(stepcount>=16000)avoid=false;
      }
      digitalWrite(right[0],stp);
      
}
void radialbackright(){
      digitalWrite(left[1],1);
      if((t[0]-t[1])>=accelerator(stepcount)&&stepcount<16000){
      stp=!stp;
      t[1]=t[0];
      stepcount++;
      if(stepcount>=16000)avoid=false;
      }
      digitalWrite(left[0],stp);
}
void radialback(){
      digitalWrite(left[1],1);
      digitalWrite(right[1],1);
      if((t[0]-t[1])>=accelerator((stepcount<8000)?stepcount:stepcount-8000)&&stepcount<16000){
      stp=!stp;
      t[1]=t[0];
      stepcount++;
      if(stepcount>=16000)avoid=false;
      }
      if(stepcount<8000)digitalWrite(left[0],stp);
      else digitalWrite(right[0],stp);
}
void desperation(){
        digitalWrite(left[1],1);
        digitalWrite(right[1],0);
      if((t[0]-t[1])>=accelerator(stepcount)&&stepcount<50000){
      stp=!stp;
      t[1]=t[0];
      stepcount++;
      if(stepcount>=50000)avoid=false;
      }
      digitalWrite(left[0],stp);
      digitalWrite(right[0],stp);
}
int accelerator(int stpcnt){
 if(stpcnt<500){
    return map(stpcnt,0,500,300,100);
  }else if(stpcnt<4000){
    return map(stpcnt,500,4000,100,50);
  }else if(stpcnt>=4000){
    return 50;
  }
}



