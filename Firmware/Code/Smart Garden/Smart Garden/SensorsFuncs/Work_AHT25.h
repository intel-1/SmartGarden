
#ifndef WORK_AHT25_H
#define WORK_AHT25_H

#include "../lib/Adafruit_AHTX0.h"


extern Adafruit_AHTX0 AHT25;


void CalculateAHT25(byte NumberSensor, byte TypeDataSensor);

#endif