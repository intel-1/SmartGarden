
#ifndef WORK_BME280_H
#define WORK_BME280_H

#include "../lib/BME280.h"

extern BME280 bme280[2];


void CalculateBME280(byte NumberSensor, byte TypeDataSensor);

#endif
