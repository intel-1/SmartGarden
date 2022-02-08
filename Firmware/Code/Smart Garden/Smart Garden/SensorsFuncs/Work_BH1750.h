
#ifndef WORK_BH1750_H
#define WORK_BH1750_H


#include "../lib/OneWire.h"
#include "../lib/BH1750.h"


extern BH1750 BH1750_1;
extern BH1750 BH1750_2;



void CalculateBH1750(byte NumberSensor, byte TypeDataSensor);


#endif