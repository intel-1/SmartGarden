
#ifndef WORK_LM75_H
#define WORK_LM75_H


//#include "../lib/TSL2561.h"



//void CalculateLM75(byte NumberSensor);


void LM75_Config(byte Address, byte SHUTDOWN, byte OS_COMP_INT, byte OS_POL);
void LM75_Thyst(byte Address, byte Value);
void LM75_Tos(byte Address, byte Value);
float lm75(byte address);
void ConfigurationInputTempSensor();

#endif