/***
 * Title: SolarBoatTwente_SPS
 * Authors: Sander Oosterveld, Niels Leijen
 * Commissioned by: Solar Boat Twente (year 2018-2019)
 * Designed for: Solar Power Sensor
 */

#include "Arduino.h"
#include <Wire.h>

//#define DEBUG //uncomment to activate debug messages over Serial

// Arduino Pin definitions
#define PIN_VSENSE_PV 2 //A2
#define PIN_ISENSE_PV 3 //A3
#define PIN_VSENSE_BAT 1 //A1
#define PIN_ISENSE_BAT 0 //A0

#define PIN_I2C_CLOCK A5
#define PIN_I2C_DATA A4

#define PIN_RELAY 10

#define PIN_LED_STATUS 2
#define PIN_LED_RELAY 5

#define PIN_I2C_ADDRESS_1 6
#define PIN_I2C_ADDRESS_2 7
#define PIN_I2C_ADDRESS_3 4
#define PIN_I2C_ADDRESS_4 3

// solar panel status blink speed; lower = faster
#define BLINK_LIMIT 10000

// this devices its I2C address
int myAddress = 0;

// variable used to determine solar blink speed
int myBlinkCounter = 0;

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
	pinMode(PIN_LED_RELAY, OUTPUT);

	pinMode(PIN_RELAY, OUTPUT);

	// set this device its address
	myAddress = 127 - digitalRead(PIN_I2C_ADDRESS_1) * 8
			- digitalRead(PIN_I2C_ADDRESS_2) * 4
			- digitalRead(PIN_I2C_ADDRESS_3) * 2
			- digitalRead(PIN_I2C_ADDRESS_4);

	// Initialize I2C and I2C Events
	Wire.begin(myAddress);
	Wire.onRequest(requestEvent);
	Wire.onReceive(receiveEvent);

#ifdef DEBUG
	// Initialize debug messages via serial
	Serial.begin(9600);
	Serial.print("I2C slave initialized on address "); Serial.println(myAddress);
}
#endif
}

void loop() {
	// all the loop does is let a led blink faster when more current is detected
	// the rest is handled via the request and receive events
	if (digitalRead(PIN_LED_STATUS) == LOW) {
		myBlinkCounter += analogRead(PIN_ISENSE_PV);
		if (myBlinkCounter >= BLINK_LIMIT) {
			digitalWrite(PIN_LED_STATUS, HIGH);
		}
	} else {
		myBlinkCounter -= analogRead(PIN_ISENSE_PV);
		if (myBlinkCounter < 0) {
			digitalWrite(PIN_LED_STATUS, LOW);
		}
	}
	delay(10);
}

void requestEvent() {

	// read current analog input status
	unsigned int VsensBat = analogRead(PIN_VSENSE_BAT);
	unsigned int IsensBat = analogRead(PIN_ISENSE_BAT);
	unsigned int VsensPV = analogRead(PIN_VSENSE_PV);
	unsigned int IsensPV = analogRead(PIN_ISENSE_PV);

	//send each 2 byte unsigned integer by cutting it in single bytes
	Wire.write((VsensBat >> 8));
	Wire.write((VsensBat && 0xFF));
	Wire.write((IsensBat >> 8));
	Wire.write((IsensBat && 0xFF));

	Wire.write((VsensPV  >> 8));
	Wire.write((VsensPV && 0xFF));
	Wire.write((IsensPV >> 8));
	Wire.write((IsensPV && 0xFF));

#ifdef DEBUG
	Serial.print("Sent I2C data from address "); Serial.println(myAddress);
	Serial.print("VsensBat: "); Serial.println(VsensBat);
	Serial.print("IsensBat: "); Serial.println(IsensBat);
	Serial.print("VsensPV: "); Serial.println(VsenPV);
	Serial.print("IsensPV: "); Serial.println(IsensPV);
#endif

}

void receiveEvent(int numBytes) {
	while (Wire.available() > 0) {
		int msg = Wire.read();

		if (msg == 1) {
			digitalWrite(PIN_RELAY, HIGH);
			digitalWrite(PIN_LED_RELAY, HIGH);
		} else {
			digitalWrite(PIN_RELAY, LOW);
			digitalWrite(PIN_LED_RELAY, LOW);
		}
#ifdef DEBUG
		Serial.print("Message received; new relay status: "); Serial.println(msg);
#endif
	}
}

