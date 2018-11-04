#include <Arduino.h>
#include <Wire.h>


#include "source/functions.h"

boolean RELAY_STATUS = false;

void setup() {
	initiatePins();

	int address = getAddress();
	Wire.begin(address);

#ifdef DEBUG
	Serial.begin(9600);
#endif

	Wire.onReceive(relaySwitcher);
	Wire.onRequest(onRequestEvent);
}

void loop() {
	if (RELAY_STATUS == true){
		digitalWrite(PIN_RELAY, HIGH);
		digitalWrite(PIN_RELAY, HIGH);
		delay(100);
	}
	digitalWrite(PIN_RELAY, LOW);
	digitalWrite(PIN_LED_RELAY, LOW);
	delay(100);

}
