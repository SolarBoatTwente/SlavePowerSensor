/*
 * definePins.h
 *
 *  Created on: Nov 1, 2018
 *      Author: Sander Oosterveld
 */

#ifndef INCLUDE_DEFINEPINS_H_
#define INCLUDE_DEFINEPINS_H_

#include <Arduino.h>


//defining the pins for the voltage and current sensor
#define PIN_VSENSE_PV 25
#define PIN_ISENSE_PV 26
#define PIN_VSENSE_BAT 24
#define PIN_ISENSE_BAT 23

//defining the pins for I2C
#define PIN_I2C_CLOCK 28
#define PIN_I2C_DATA 27

//defining the pins for the RELAY
#define PIN_RELAY 14

//defining the two status LEDS
#define PIN_LED_STATUS 9
#define PIN_LED_RELAY 32

//defining the I2C_Adress pins
#define PIN_I2C_ADDRESS_1 9
#define PIN_I2C_ADDRESS_2 10
#define PIN_I2C_ADDRESS_3 11
#define PIN_I2C_ADDRESS_4 12


#endif /* INCLUDE_DEFINEPINS_H_ */
