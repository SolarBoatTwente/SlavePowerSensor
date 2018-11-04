/*
 * functions.h
 *
 *  Created on: Nov 1, 2018
 *      Author: Sander Oosterveld
 */

#ifndef INCLUDE_FUNCTIONS_H_
#define INCLUDE_FUNCTIONS_H_

#include <Arduino.h>

#include "definePins.h"

int getAddress();

void initiatePins();

void onRequestEvent();

void relaySwitcher(int numBytes);

extern boolean RELAY_STATUS;

#endif /* INCLUDE_FUNCTIONS_H_ */