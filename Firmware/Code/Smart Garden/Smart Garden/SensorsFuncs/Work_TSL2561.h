#ifndef WORK_TSL2561_H
#define WORK_TSL2561_H


#include "../lib/TSL2561.h"


// јдреса датчиков
#define TSL2561_ADDR_LOW  0x29
#define TSL2561_ADDR_FLOAT 0x39
#define TSL2561_ADDR_HIGH 0x49

// ¬рем€ конвертации
#define INTEGRATIONTIME_13MS 1			// 13.7ms (default значение)
#define INTEGRATIONTIME_101MS 2			// 101ms
#define INTEGRATIONTIME_402MS 3			// 402ms

// ”силение
#define GAIN_1X 2				// 1x ( дл€ тусклого света)
#define GAIN_16X 1				// 16x (желательно дл€ €ркого света)
#define GAIN_AUTO 3				// јвтоматическое усиление (default значение)


extern Adafruit_TSL2561 tsl1;
extern Adafruit_TSL2561 tsl2;
extern Adafruit_TSL2561 tsl3;


void CalculateTSL2561(byte NumberSensor, byte TypeDataSensor);

#endif