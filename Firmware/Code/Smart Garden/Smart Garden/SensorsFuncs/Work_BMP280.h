
#ifndef WORK_BMP280_H
#define WORK_BMP280_H

#include "../lib/BMP280.h"

extern BMP280 bmp280[2];


void CalculateBMP280(byte NumberSensor, byte TypeDataSensor);

#endif
