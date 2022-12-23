
#ifndef WORK_LM75_H
#define WORK_LM75_H


void LM75_Config(byte Address, byte SHUTDOWN, byte OS_COMP_INT, byte OS_POL);
void LM75_Thyst(byte Address, byte Value);
void LM75_Tos(byte Address, byte Value);
float lm75(byte address);

float ReadValueLM75(byte _NumberSensor, byte _Address);

void Configuration_LM75(byte _Address, byte _Thyst, byte _Tos);

#endif