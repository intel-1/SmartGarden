
#ifndef WORK_TSL2561_H
#define WORK_TSL2561_H


#include "../lib/TSL2561.h"



extern Adafruit_TSL2561 tsl1;
extern Adafruit_TSL2561 tsl2;
extern Adafruit_TSL2561 tsl3;


void CalculateTSL2561(byte NumberSensor, byte TypeDataSensor);

#endif