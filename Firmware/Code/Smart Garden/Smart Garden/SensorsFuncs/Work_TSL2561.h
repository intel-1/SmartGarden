#ifndef WORK_TSL2561_H
#define WORK_TSL2561_H


#include "../lib/TSL2561.h"


// ������ ��������
#define TSL2561_ADDR_LOW  0x29
#define TSL2561_ADDR_FLOAT 0x39
#define TSL2561_ADDR_HIGH 0x49

// ����� �����������
#define INTEGRATIONTIME_13MS 1			// 13.7ms (default ��������)
#define INTEGRATIONTIME_101MS 2			// 101ms
#define INTEGRATIONTIME_402MS 3			// 402ms

// ��������
#define GAIN_1X 2				// 1x ( ��� �������� �����)
#define GAIN_16X 1				// 16x (���������� ��� ������ �����)
#define GAIN_AUTO 3				// �������������� �������� (default ��������)


extern Adafruit_TSL2561 tsl1;
extern Adafruit_TSL2561 tsl2;
extern Adafruit_TSL2561 tsl3;


void CalculateTSL2561(byte NumberSensor, byte TypeDataSensor);

#endif