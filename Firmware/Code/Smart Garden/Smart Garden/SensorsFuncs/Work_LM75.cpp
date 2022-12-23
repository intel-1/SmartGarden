#include <Arduino.h>

#include "Work_LM75.h"
//#include "../lib/LM75.h"

#include "../Configuration.h"
#include "../Sensors.h"




#define LM75_OS_F_QUE_1 00
#define LM75_OS_F_QUE_2 01
#define LM75_OS_F_QUE_3 10
#define LM75_OS_F_QUE_6 11

#define LM75_OS_POL_ACTIVE_HIGH 1
#define LM75_OS_POL_ACTIVE_LOW 0

#define LM75_OS_COMP_INT_COMPARATOR 0
#define LM75_OS_COMP_INT_INTERUP 1

#define LM75_SHUTDOWN_NORMAL 0
#define LM75_SHUTDOWN_SHUTDOWN 1




void LM75_Config_CONF(byte Address, byte SHUTDOWN, byte OS_COMP_INT, byte OS_POL){
	Wire.beginTransmission(Address);
	Wire.write(0x01);					// conf reg
	Wire.write(0b00000000);				// data 00000000
	Wire.endTransmission();
}


void LM75_Thyst(byte Address, byte Value){
	Wire.beginTransmission(Address);
	Wire.write(0x02);
	Wire.write((int(Value*2)) >> 1);
	Wire.write((int(Value*2) & 1) << 7);
	Wire.endTransmission();
}


void LM75_Tos(byte Address, byte Value){
	Wire.beginTransmission(Address);
	Wire.write(0x03);
	Wire.write((int(EEPROM.read(Value)*2)) >> 1);
	Wire.write((int(EEPROM.read(Value)*2) & 1) << 7);
	Wire.endTransmission();
}


void Configuration_LM75(byte _Address, byte _Thyst, byte _Tos){
	LM75_Config_CONF(_Address, 0, 0, 0);
	LM75_Thyst(_Address, _Thyst);
	LM75_Tos(_Address, _Tos);
}


float lm75(byte Address){
	/// TEMP REG
	Wire.beginTransmission(Address);
	Wire.write(0x00); // temp reg
	Wire.endTransmission();
	Wire.requestFrom(Address, 2);
	while(Wire.available() < 2);
	float value = (((Wire.read() << 8) | Wire.read()) >> 5)*0.125 ;
	return value;
}


float ReadValueLM75(byte _NumberSensor, byte _Address){
	byte _QuantityRead = EEPROM.read(E_QuantityReadSensors + _NumberSensor);;
	
	byte _QuantitySummValue = 1;
	float _BufferValue;
	
	if(_Address == ADDRESS_INPUT_LM75){
		_BufferValue = lm75(_Address);
	}
	else{
		while(_QuantitySummValue <= _QuantityRead){
			_BufferValue += lm75(_Address);								// Опрашиваем датчик
			_QuantitySummValue ++;
			_delay_ms(50);
		}
	}
	return _BufferValue / _QuantitySummValue;
}