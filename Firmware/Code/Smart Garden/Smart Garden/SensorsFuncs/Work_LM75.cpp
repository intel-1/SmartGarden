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
	Wire.write(0x01);			// conf reg
	Wire.write(0b00000000);		// data 00000000
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


void ConfigurationInputTempSensor(){
	byte Address = ADRESS_INPUT_TEMP_SENSOR;
	LM75_Config_CONF(Address, 0, 0, 0);
	LM75_Thyst(Address, E_INT_LM75_THYST);
	LM75_Tos(Address, E_INT_LM75_TOS);
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





























// 
// byte ConfigLM[8][8];		/*	- Номер датчика
// 								- Номер объекта инициализации (LM_1...LM_7)
// 							*/ 
// 
// LM75 LM_1(0b001);
// LM75 LM_2(0b010);
// LM75 LM_3(0b011);
// LM75 LM_4(0b100);
// LM75 LM_5(0b101);
// LM75 LM_6(0b110);
// LM75 LM_7(0b111);
// 
// 
// 
// 
// void CalculateLM75(byte NumberSensor){
// 	
// 	/*
// 	byte AdressSensor = EEPROM.read(E_Address_Sensor + (NumberSensor * 10));
// 	
// 	if (OUTPUT_LEVEL_UART_SENSOR){
// 		Serial.println();
// 	}
// 	
// 	ControllPort(NumberSensor, 1);								// Включаем управление Controll портом
// 	
// 	if (OUTPUT_LEVEL_UART_SENSOR){
// 		Serial.print(F("\t\t\t...measurement Temp"));
// 	}
// 	
// 	for(byte Sensor = 1; Sensor <= 8; Sensor++){				// Проходим по всем ячейкам массива инициальзации в поисках объекта
// 		if(NumberSensor == ConfigLM[NumberSensor][0]){			// и когда нашли
// 			switch(ConfigLM[NumberSensor][0]){*/
// 			switch(NumberSensor){
// 				case 1:
// 					RealValueSensors[NumberSensor][VALUE_1] = LM_1.temp();
// 					Serial.println(LM_1.temp());
// 					break;
// 				case 2:
// 					RealValueSensors[NumberSensor][VALUE_1] = LM_2.temp();
// 					Serial.println(LM_2.temp());
// 					break;
// 				case 3:
// 					RealValueSensors[NumberSensor][VALUE_1] = LM_3.temp();
// 					Serial.println(LM_3.temp());
// 					break;
// 				case 4:
// 					RealValueSensors[NumberSensor][VALUE_1] = LM_4.temp();
// 					Serial.println(LM_4.temp());
// 					break;
// 				case 5:
// 					RealValueSensors[NumberSensor][VALUE_1] = LM_5.temp();
// 					Serial.println(LM_5.temp());
// 					break;
// 				case 6:
// 					RealValueSensors[NumberSensor][VALUE_1] = LM_6.temp();
// 					Serial.println(LM_6.temp());
// 					break;
// 				case 7:
// 					RealValueSensors[NumberSensor][VALUE_1] = LM_7.temp();
// 					Serial.println(LM_7.temp());
// 					break;
// 			}/*
// 			if(Sensor == 7){									// Если прошли по всем ячейкам массива и не нашли совпадений
// 				if (OUTPUT_LEVEL_UART_SENSOR){
// 					Serial.println(F("...error configuration sensor"));
// 				}
// 				else{
// 					if (OUTPUT_LEVEL_UART_SENSOR){
// 						Serial.println(F("...done"));
// 					}
// 				}
// 			}
// 			Sensor = 8;											// Принудительно завершаем проверку
// 		}
// 	}
// 
// 	ControllPort(NumberSensor, 0);								// Выключаем управление Controll портом*/	
// }