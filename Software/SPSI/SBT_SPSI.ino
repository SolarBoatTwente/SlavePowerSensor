/***
 * Author: Niels Leijen
 * Commissioned by: Solar Boat Twente (year 2018-2019)
 * Toolchain: Eclipse + Sloeber plugin
 */

//#define DEBUG

#define I2CADDRESS 123

#define CANBAUD CANSPEED_250
#define CANADDRESS_RELAY 0x86
#define CANADDRESS_POWER 0x87

#include "Arduino.h"
#include <Wire.h>
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

void setup()
{
	// Initialize I2C
	Wire.begin();

	// Initialize MCP2515 CAN controller at the specified speed
	int initSucces = Canbus.init(CANBAUD);

#ifdef DEBUG
	// Initialize debug messages via serial
	Serial.begin(9600);
	Serial.println("I2C initialized.");
	Serial.println("CAN-Bus initializing at speed: %s.", CANBAUD);
	initSucces ? Serial.println("CAN initialization ok.") : Serial.println("CAN initialization failed.");
	}
#endif
}

void loop()
{
	if (mcp2515_check_message()){
		tCAN msg;
		if (mcp2515_get_message(&msg)){
#ifdef DEBUG
			Serial.print("ID: ");
			Serial.print(msg.id,HEX);
			Serial.print(", ");
			Serial.print("Data: ");
			Serial.print(msg.header.length,DEC);
			for(int i = 0; i < msg.header.length; i++){
				Serial.print(msg.data[i],HEX);
				Serial.print(" ");
			}
			Serial.println("");
#endif
			if(msg.id == CANADDRESS_RELAY){ // first 2 bytes have relay status info LSB first
				for(int i = 0; i < 2; i++){
					for (int j = 0; j < 8; j++){
						 Wire.beginTransmission((I2CADDRESS + i*8 + j));
						 Wire.write(msg.data[i] << j);
						 Wire.endTransmission();
					}

					// Nice-to-have: confirmation whether relay is actually turned on/off
				}
			} else if (msg.id == CANADDRESS_POWER){
				//TODO
				for(int i = 0; i < msg.header.length; i++){
					Serial.print(msg.data[i],HEX);
					Serial.print(" ");
				}
			}
		}
	}
}
