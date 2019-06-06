

#include <CurieBLE.h>
#include <Servo.h>
int val =1500;
int f =0;
char state;   
BLEPeripheral blePeripheral; 
unsigned int pmil = 0;
unsigned int pmil2 = 0;
int iypos =1500;
BLEService uartService = BLEService("0000ffe0-0000-10008000-00805f9b34fb");
// create characteristics
BLECharacteristic rxCharacteristic = BLECharacteristic("0000ffe1-0000-1000-800000805f9b34fb", BLEWriteWithoutResponse, 7);  
BLECharacteristic txCharacteristic = BLECharacteristic("0000ffe1-0000-1000-800000805f9b34fb ", BLERead | BLENotify , 7); 

Servo myServo1, myServo2;

char cmd[100];
int cmdIndex;


void setup() {
  Serial.begin(9600);
  
  blePeripheral.setLocalName("Vincent");
  blePeripheral.setAdvertisedServiceUuid(uartService.uuid());


  blePeripheral.addAttribute(uartService);
  blePeripheral.addAttribute(rxCharacteristic);
  blePeripheral.addAttribute(txCharacteristic);


  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);


  rxCharacteristic.setEventHandler(BLEWritten, rxCharacteristicWritten);


  blePeripheral.begin();
  myServo1.attach(7, 1000, 2000);
  myServo2.attach(4, 1000, 2000);
  myServo2.writeMicroseconds(iypos);
  cmdIndex = 0;
  pinMode(3,OUTPUT);
}

void loop() {
  // poll ble peripheral
  blePeripheral.poll();   
  unsigned int cur = millis();
  int diff = val-iypos;
   if((cur-pmil)>map(abs(diff),0,1000,0,3)){
  if((diff)>0){
    iypos+=2;
  }
  if((diff)<0){
    iypos-=2;
  }
  pmil = cur;
  }
     if((cur-pmil2)>map(abs(diff),0,1000,5,0)){
    myServo2.writeMicroseconds(iypos);
      pmil2 = cur;
  }
  if(f==1){
    digitalWrite(3,HIGH);
    Serial.println(f);
  }else {
    digitalWrite(3,LOW);
  }


}

void blePeripheralConnectHandler(BLECentral& central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());

  delay(2000);
}

void blePeripheralDisconnectHandler(BLECentral& central) {

  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());


}

void rxCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic) {
  if (characteristic.value()) {
    

    int len = characteristic.valueLength();
    for(int i=0; i<len; i++){
      char c = characteristic.value()[i];
      if(c=='\n') {
   // Serial.println("");
      cmd[cmdIndex] = 0;
      exeCmd();  // execute the command
      cmdIndex = 0; // reset the cmdIndex
    } else {   
  //  Serial.print(c);
      cmd[cmdIndex] = c;
      if(cmdIndex<99) cmdIndex++;
    }
    }


  } 
}

boolean cmdStartsWith(char *st) {
  for(int i=0; ; i++) {
    if(st[i]==0) return true;
    if(cmd[i]==0) return false;
    if(cmd[i]!=st[i]) return false;;
  }
  return false;
}

void exeCmd() {
  

    if( cmdStartsWith("y ") ) { 
     val = atoi(cmd+2) +154;     
  } 
      if( cmdStartsWith("f ") ) { 
     f = atoi(cmd+2);     
  } 
 

 
  
}
