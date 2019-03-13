#include <Wire.h>


int allAddresses[16];
int numberOfSlaves;
int total = 0;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  delay(100);
  numberOfSlaves = getAddresses();
}

void loop() {
  // put your main code here, to run repeatedly:

  for(int i = 0; i<numberOfSlaves; i++){
    Wire.requestFrom(allAddresses[i],8);
    Serial.print("From device: ");
    Serial.print(allAddresses[i]);
    Serial.print("\t");
    while(Wire.available()){
      total = 4 * Wire.read() + Wire.read(); 
      //total = Wire.read();
        Serial.print(total);
        Serial.print(" ");    
    }
  
    Serial.print("\n");
  }
  delay(1000);
/*
  for(int i = 0; i<numberOfSlaves; i++){
    Wire.beginTransmission(allAddresses[i]);
    Wire.write(0);
    Serial.print("Sending to device:");
    Serial.print(i);
    Serial.print(" 0\n");
    Wire.endTransmission();
    delay(1000);
    Wire.beginTransmission(allAddresses[i]);
    Serial.print("Sending to device:");
    Serial.print(i);
    Serial.print(" 1\n");
    Wire.write(1);
    Wire.endTransmission();
  }
*/
}

word slaveScanner(){
  word slave;
  byte error;

  slave = 0;
  for(byte address = 112; address<128; address++){
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error==0){
      slave += 1 << (address - 112);
    }
  }
  
  return slave;
}

int getAddresses(){
  int counter = 0;
  word slaves = slaveScanner();
  Serial.println(slaves, BIN);
  int slaveCounter = 0;
  while(slaves){
      if(slaves&1){
        allAddresses[counter] = 112 + slaveCounter;
        
        counter+=1;
      }
      slaveCounter += 1;
      slaves >>= 1;
  }
  return counter;
}
 
