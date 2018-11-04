/*
 * functions.cpp
 *
 *  Created on: Nov 1, 2018
 *      Author: Sander Oosterveld
 */
#include <arduino.h>
#include <Wire.h>

#include "functions.h"

int getAddress() {
	/*
	 * Calculates the address of the controller based on the the 4 way switch
	 *
	 * Inputs: none
	 *
	 * Outputs:
	 *    int address: The address as an integer 112 - 127
	 */

	//Calculate the address of the controller. The base of 112 is required because the lower addresses are reserved in I2C
	int address = 112 + (digitalRead(PIN_I2C_ADDRESS_1)*8 + digitalRead(PIN_I2C_ADDRESS_2)*4 + digitalRead(PIN_I2C_ADDRESS_1)*2+ digitalRead(PIN_I2C_ADDRESS_1)*1);

#ifdef DEBUG
	Serial.print("Address: \t");
	Serial.print(address);
	Serial.print("\n");
#endif

	return address;
}

void initiatePins(){
	/*
	 * Initiates all the pins of the ATMega.
	 *
	 * First thing to be called in setup() function
	 *
	 * Inputs: nothing
	 *
	 * Output: nothing
	 *
	 */

	//initiating the data pins
	pinMode(PIN_ISENSE_PV, INPUT);
	pinMode(PIN_ISENSE_BAT, INPUT);
	pinMode(PIN_VSENSE_PV, INPUT);
	pinMode(PIN_VSENSE_BAT, INPUT);

	//initiating the Led pins
	pinMode(PIN_LED_STATUS, OUTPUT);
	pinMode(PIN_LED_RELAY, OUTPUT);

	//initiate the relay output pin
	pinMode(PIN_RELAY, OUTPUT);

	//initiate I2C address pins
	pinMode(PIN_I2C_ADDRESS_1, INPUT_PULLUP);
	pinMode(PIN_I2C_ADDRESS_2, INPUT_PULLUP);
	pinMode(PIN_I2C_ADDRESS_3, INPUT_PULLUP);
	pinMode(PIN_I2C_ADDRESS_4, INPUT_PULLUP);

#ifdef DEBUG
	Serial.print("All pins initiated\n");
#endif
}

void printData(String name, int measured, float real) {
	/*
	 * Prints the measured to the serial port in a readable way.
	 *
	 * Inputs:
	 *    String name: Identifier for the variable
	 *    int measured: The raw data measured on the pin
	 *    float real: The real current/voltage after scaling
	 *
	 * Output: nothing
	 */
	Serial.print(name);
	Serial.print("\t");
	Serial.print(measured);
	Serial.print("\t");
	Serial.print(real);
	Serial.print("\n");
}

void onRequestEvent(){
	/*
	 * Sends the Data from the 4 input pins to the I2C master.
	 *
	 * This function needs to put in the Wire.onRequest() inside the setup loop.
	 *
	 * Inputs: nothing
	 *
	 * Outputs: nothing
	 */
	void printData(String name, int measured, float real);

	//Read the pin data.
	int VsensPV = analogRead(PIN_VSENSE_PV);
	int IsensPV = analogRead(PIN_ISENSE_PV);
	int VsensBat = analogRead(PIN_VSENSE_BAT);
	int IsensBat = analogRead(PIN_ISENSE_BAT);


#ifdef DEBUG
	//The correction since both the current and voltage do not map 1-1 to voltages on the pins
	float GAIN_VOLTAGE_DIVIDER = 2.0;
	float GAIN_CURRENT_CIRCUIT = 10.0;

	// The reference voltage, regulated by a zener diode at 4.096
	float VREF = 4.096;

	// Calculating the real values of the measured data
	float realVsensPV = GAIN_VOLTAGE_DIVIDER * VREF / 1024 * VsensPV;
	float realVsensBat = GAIN_VOLTAGE_DIVIDER * VREF / 1024 * VsensBat;
	float realIsensPV = GAIN_CURRENT_CIRCUIT * VREF / 1024 * IsensPV;
	float realIsensBat = GAIN_CURRENT_CIRCUIT * VREF / 1024 * IsensBat;

	// Printing this data to the Serial port
	Serial.print("Sensor \t Measured(int) \t Real(V/A) \n");
	printData("VsensePV", VsensePV, realVsensPV);
	printData("IsensePV", IsensePV, realIsensPV);
	printData("VsenseBat", VsenseBat, realVsensBat);
	printData("IsenseBat", IsenseBat, realIsensBat);

#endif

	//Bitshifts the read gathered data such that it becomes exactly one byte (8 bits)
	IsensPV = IsensPV >> 2;
	VsensPV = VsensPV >> 2;
	IsensBat = IsensBat >> 2;
	VsensBat = VsensBat >> 2;

	//Writes the data unto the I2C bus
	Wire.write(VsensPV);
	Wire.write(IsensPV);
	Wire.write(VsensBat);
	Wire.write(IsensBat);

}


void relaySwitcher(int numBytes){
	/*
	 * Tests the data received from the master, if it is a 0 it is will turn of the relay, if it is a 1 it will turn on the relay
	 *
	 * This function needs to be put in the Wire.onrecieve()
	 *
	 * Inputs: message length in bytes (done automatically by the onReceive function)
	 *
	 * Ouptuts: none (but changes a global value of RELAY_STATUS)
	 */

	while(Wire.available()>0){
		//If we receive that the relay should be on
		if(Wire.read()==0){
			RELAY_STATUS = false;
		} else{
			//turn relay on
			RELAY_STATUS = true;
		}
	}

}
