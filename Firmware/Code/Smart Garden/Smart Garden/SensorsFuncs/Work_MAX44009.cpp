#include <Arduino.h>

#include "Work_MAX44009.h"

#include "../Configuration.h"
#include "../Sensors.h"



// bool MAX44009_begin(uint8_t addr, byte NumberSensor){
// 	Wire.beginTransmission(addr);
// 	Wire.write(0x02);
// 	switch(EEPROM.read(E_ConfigSensor_A + NumberSensor)){
// 		case 0:
// 			Wire.write(0x43);				// Задержка 100ms (для тусклого света)
// 			break;
// 		case 1:
// 			Wire.write(0x40);				// Задержка 800ms (для яркого света)
// 			break;
// 		default:
// 			Wire.write(0x40);				// Задержка 800ms (для яркого света)			
// 	}
// 	
// 	if(Wire.endTransmission() == 0){
// 		return true;
// 	}
// 	else return false;
// }


float MAX44009_get_lux(byte _Address){
	unsigned int data[2];
	Wire.beginTransmission(_Address);
	Wire.write(0x03);
	Wire.endTransmission();
	
	// Request 2 bytes of data
	Wire.requestFrom(_Address, 2);
	
	// Read 2 bytes of data luminance msb, luminance lsb
	if (Wire.available() == 2)
	{
		data[0] = Wire.read();
		data[1] = Wire.read();
	}
	
	// Convert the data to lux
	int exponent = (data[0] & 0xF0) >> 4;
	int mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F);
	
	//float luminance = pow(2, exponent) * mantissa * 0.045;
	//float luminance = (float)(((0x00000001 << exponent) * (float)mantissa) * 0.045);
	
	return (float)(((0x00000001 << exponent) * (float)mantissa) * 0.045);
}



float ReadData(byte NumberSensor, byte _Address){
	byte _QuantityRead = EEPROM.read(E_QuantityReadSensors + NumberSensor);
	
	byte _QuantitySummValue = 1;
	float _BufferValue;
	while(_QuantitySummValue <= _QuantityRead){
		_BufferValue += MAX44009_get_lux(_Address);							// Опрашиваем датчик
		_QuantitySummValue ++;
		_delay_ms(50);
	}
	return _BufferValue / _QuantitySummValue;
}



void CalculateMAX44009(byte NumberSensor, byte TypeDataSensor){
	bool Error_Address = false;
	
	byte AddressSensor = EEPROM.read(E_Address_Sensor + (NumberSensor * 10));
	
	if (OUTPUT_LEVEL_UART_SENSOR){
		switch(AddressSensor){
			case 0x4a:
				Serial.println(F("_0x4a"));
				break;
			case 0x4b:
				Serial.println(F("_0x4b"));
				break;
			default:
				Error_Address = true;
				Serial.println(F("_Error address"));
		}
	}
	if(!Error_Address){																		// Если не висит ошибка адреса датчика
		ControllPort(NumberSensor, 1);														// Включаем управление Controll портом
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.print(F("\t\t\t...measurement Light"));
		}
	
		SensorsError[NumberSensor][VALUE_2] = 0;											// Снимаем возможную ошибку чтения данных
		RealValueSensors[NumberSensor][VALUE_2] = ReadData(NumberSensor, AddressSensor);	// Сохраняем измеренное значение датчика
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.println(F("...done"));
		}
		ControllPort(NumberSensor, 0);														// Выключаем управление Controll портом
	}
}