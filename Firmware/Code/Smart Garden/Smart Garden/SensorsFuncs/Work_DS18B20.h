
#ifndef WORK_DS18B20_H
#define WORK_DS18B20_H

#include "../lib/DallasTemperature.h"
#include "../lib/OneWire.h"


extern DallasTemperature sensors1;
extern DallasTemperature sensors2;
extern DallasTemperature sensors3;
extern DallasTemperature sensors4;
extern DallasTemperature sensors5;
extern DallasTemperature sensors6;


void CalculateDS18B20(byte NumberSensor);
//void DS18B20_scaner(byte ViewLogs);
void DS18B20_scaner(bool LogsToUART);
void StartMeasurementIndicationsDS18B20();

#endif