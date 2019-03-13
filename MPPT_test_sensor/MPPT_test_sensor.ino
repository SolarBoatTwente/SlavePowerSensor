#include <Wire.h>

#define PIN_VSENSE_PV A2
#define PIN_ISENSE_PV A3
#define PIN_VSENSE_BAT A1
#define PIN_ISENSE_BAT A0

//defining the pins for I2C
#define PIN_I2C_CLOCK A5
#define PIN_I2C_DATA A4

//defining the pins for the RELAY
#define PIN_RELAY 10

//defining the two status LEDS
#define PIN_LED_STATUS 2
#define PIN_LED_RELAY 5

//defining the I2C_Adress pins
#define PIN_I2C_ADDRESS_1 6
#define PIN_I2C_ADDRESS_2 7
#define PIN_I2C_ADDRESS_3 4
#define PIN_I2C_ADDRESS_4 3

int address = 0;

boolean RELAY_STATUS = false;


unsigned long currentTime;
unsigned long startTime;

void setup() {
  pinMode(PIN_I2C_ADDRESS_1, INPUT_PULLUP);
  pinMode(PIN_I2C_ADDRESS_2, INPUT_PULLUP);
  pinMode(PIN_I2C_ADDRESS_3, INPUT_PULLUP);
  pinMode(PIN_I2C_ADDRESS_4, INPUT_PULLUP);

  pinMode(PIN_ISENSE_PV, INPUT);
  pinMode(PIN_ISENSE_BAT, INPUT);
  pinMode(PIN_VSENSE_PV, INPUT);
  pinMode(PIN_VSENSE_BAT, INPUT);
  
  
  pinMode(PIN_LED_STATUS, OUTPUT);
  pinMode(PIN_LED_RELAY,OUTPUT);
  pinMode(PIN_RELAY,OUTPUT);
  // put your setup code here, to run once:
  address = 127 - (digitalRead(PIN_I2C_ADDRESS_1)*8 + digitalRead(PIN_I2C_ADDRESS_2)*4 + digitalRead(PIN_I2C_ADDRESS_3)*2 + digitalRead(PIN_I2C_ADDRESS_4));

  Wire.begin(address);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  startTime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(RELAY_STATUS == true){
    digitalWrite(PIN_RELAY, HIGH);
    digitalWrite(PIN_LED_RELAY, HIGH);    
  }else{
    digitalWrite(PIN_RELAY,LOW);
    digitalWrite(PIN_LED_RELAY,LOW);
  }
  currentTime = millis();
  if (int((currentTime - startTime)/10)==30){
  digitalWrite(PIN_LED_STATUS,HIGH);
  }else if (int((currentTime - startTime)/10)==60){
  digitalWrite(PIN_LED_STATUS,LOW);
  startTime=millis();
  }
}

void requestEvent(int numBytes){
  int VsensPV = analogRead(PIN_VSENSE_PV);
  int IsensPV = analogRead(PIN_ISENSE_PV);
  int VsensBat = analogRead(PIN_VSENSE_BAT);
  int IsensBat = analogRead(PIN_ISENSE_BAT);
  
 Wire.write((VsensPV>>2));
 Wire.write((VsensPV&3));

 Wire.write((VsensBat>>2));
  Wire.write((VsensBat&3));

  Wire.write((IsensPV>>2));
  Wire.write((IsensPV&3));

 Wire.write((IsensBat>>2));
 Wire.write((IsensBat&3));

  
  //Wire.write(VsensBat);
  
}

void receiveEvent(){
  while(Wire.available()>0){
    if(Wire.read()==0){
      RELAY_STATUS = false;
    } else{
      RELAY_STATUS = true;
    }
  }
}

