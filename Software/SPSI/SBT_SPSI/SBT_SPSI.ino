/***
 * Title: SolarBoatTwente_SPSI
 * Author: Niels Leijen
 * Commissioned by: Solar Boat Twente (year 2018-2019)
 * Designed for: Arduino Uno Rev3 + Sparkfun CAN-Bus Shield
 */

#include "Arduino.h"
#include <Wire.h>
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

#define DEBUG // uncomment for debug messages over serial at 9600 baud

#define I2C_ADDRESS 112 // lowest I2C address of 16 SPS's (standard = 112 to 127)

#define CAN_BAUD CANSPEED_250 // CAN Baud rate (standard = 250kb)
#define CAN_ADDRESS_RELAY 0x3C // RX relay setting address (standard = 0x3C)
#define CAN_ADDRESS_POWER 0x3D // lowest TX address of 16 SPS's (standard = 0x3D to 0x4C)
#define CAN_INTERVAL 1000 // CAN TX interval in ms (standard = 1000ms)

unsigned int SPSDevices = 0; // SPS devices present in I2C network

unsigned int previousMillis = 0; // from Arduino blink without delay example

unsigned int SPSScan(); // checks for SPS devices in I2C network

void setup() {
	// Initialize I2C and scan for SPS devices
	Wire.begin();
	SPSDevices = SPSScan();

	// Initialize MCP2515 CAN controller at specified speed
	bool initSucces = Canbus.init(CAN_BAUD);

#ifdef DEBUG
	// Initialize debug messages via serial
	Serial.begin(9600);
	Serial.println("I2C initialized.");
	Serial.print("SPS devices present: ");
	Serial.println(SPSDevices, BIN);
	Serial.print("CAN-Bus initializing at speed: ");
	Serial.println(CAN_BAUD);
	initSucces ?
			Serial.println("CAN initialization ok.") :
			Serial.println("CAN initialization failed.");
#endif
}

void loop() {
	if (mcp2515_check_message()) {
		tCAN msg;
		if (mcp2515_get_message(&msg)) {
#ifdef DEBUG
			Serial.print("ID: ");
			Serial.print(msg.id, HEX);
			Serial.print(", ");
			Serial.print("Data: ");
			Serial.print(msg.header.length, DEC);
			for (int i = 0; i < msg.header.length; i++) {
				Serial.print(msg.data[i], HEX);
				Serial.print(" ");
			}
			Serial.println("");
#endif
			// RX command to turn relays of the SPS on or off
			if (msg.id == CAN_ADDRESS_RELAY) {
				if (msg.header.length == 2) {
					// first two bytes (LSB first) have relay status setting info as unsigned integer
					for (char i = 0; i < 2; i++) {
						for (char j = 0; j < 8; j++) {
							// write relay setting to SPS
							Wire.beginTransmission((I2C_ADDRESS + i * 8 + j));
							// shorthand for check byte, bitshift byte j positions to the right, check if it is an odd number (=1) and write to I2C
              // If bytes is send: 01000100 00101100 then device numbers 3 4 6 11 16 (Numbering starts at 1)
							Wire.write((msg.data[1-i] >> j) % 2);
							Wire.endTransmission();
						}
#ifdef DEBUG
						Serial.print("Sent I2C messages: ");
						for (int j = 0; j < 2; j++) {
							Serial.print(msg.data[j], BIN);
						}
						Serial.print(" to I2CIDs: ");
						Serial.print(I2C_ADDRESS, HEX);
						Serial.print("..");
						Serial.print(I2C_ADDRESS + 16, HEX);
#endif
					}
				} else if (msg.header.length == 0) {
					// do a SPS exist check
					SPSDevices = SPSScan();
				} // else do nothing with the message
			} // SPSI does not respond to any other CAN commands
		}
	}

// lazy mode; used blinkwithoutdelay example
	unsigned long currentMillis = millis();
	if (currentMillis - previousMillis >= CAN_INTERVAL) {
		previousMillis = currentMillis;

		for (char i = 0; i < 16; i++) {
			// only send messages to devices present
			if ((SPSDevices >> i) % 2) {
				// initialize CAN buffer
				tCAN msg;
				msg.id = CAN_ADDRESS_POWER + i;
				msg.header.rtr = 0;
				msg.header.length = 8;

				// request 8 bytes from slave device
				Wire.requestFrom(i + I2C_ADDRESS, 8);
				for (char j = 0; j < 8; j++) {
					// save requested data to message buffer (= equal length)
					msg.data[j] = Wire.read();
				}

				//mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
				mcp2515_send_message(&msg);

#ifdef DEBUG
				Serial.print("Sent CAN message: ");
				for (int j = 0; j < 8; j++) {
					Serial.print(msg.data[j], HEX);
				}
				Serial.print(" to CANID: ");
				Serial.println(msg.id, HEX);
#endif

			}
		}
	}
}

unsigned int SPSScan() {
	unsigned int devices = 0;

	for (char i = 0; i < 16; i++) {
		Wire.beginTransmission(i + I2C_ADDRESS);
		// if no fault during transmission occurs (device = present)
		if (Wire.endTransmission() == 0) {
			// save the SPS number
			devices |= 1 << i;
		}
	}
  //devices = 8;
#ifdef DEBUG
	Serial.print("SPSScan() found devices: ");
	Serial.println(devices, BIN);
#endif

	return devices;
}
