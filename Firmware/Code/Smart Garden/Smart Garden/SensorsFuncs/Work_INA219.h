
#ifndef WORK_INA219_H
#define WORK_INA219_H


#include "../lib/Adafruit_INA219.h"


extern Adafruit_INA219 ina219;


void CalculateINA219(byte NumberSensor, byte TypeDataSensor);

#endif