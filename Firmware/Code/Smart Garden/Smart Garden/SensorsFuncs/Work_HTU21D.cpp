
#include <Arduino.h>

#include "Work_HTU21D.h"

#include "../Configuration.h"
#include "../Sensors.h"


HTU21D HTU21(HTU21D_RES_RH8_TEMP12);


float ReadValue(byte _DataType){
	float _BufferValue;
	switch (_DataType) {
		case 1:
			_BufferValue = HTU21.readTemperature(SI70xx_TEMP_READ_AFTER_RH_MEASURMENT);		// Опрашиваем датчик
			break;
		case 2:
			_BufferValue = HTU21.readHumidity();
			break;
	}
	return _BufferValue;
}


float Value(byte _NumberSensor, byte _DataType){
	byte _QuantityRead = EEPROM.read(E_QuantityReadSensors + _NumberSensor);
	
	byte _QuantitySummValue = 0;
	float _BufferValue;
	while(_QuantitySummValue <= _QuantityRead){
		_BufferValue += ReadValue(_DataType);								// Опрашиваем датчик
		_QuantitySummValue ++;
		_delay_ms(50);
	}
	return _BufferValue / _QuantitySummValue;
}


void CalculateHTU21D(byte _NumberSensor, byte TypeDataSensor){
	#define TEMP_VALUE 1
	#define HUMM_VALUE 2
	#define HUMM_AND_TEMP_VALUE 12
	
	float _BufferValue;
	ControllPort(_NumberSensor, 1);											// Включаем управление Controll портом
	switch(TypeDataSensor){
		case TEMP_VALUE:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp"));Serial.println(F("...done"));
			}
			_BufferValue = Value(_NumberSensor, TEMP_VALUE);
			if(_BufferValue != 255){										// Если показания верные							
				RealValueSensors[_NumberSensor][VALUE_1] = _BufferValue;
				SensorsError[_NumberSensor][VALUE_1] = 0;
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...done"));
				}
			}
			else {
				SensorsError[_NumberSensor][VALUE_1] = 1;					// Иначе поднимаем флаг ошибочности данных
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...error"));
				}
			}
			break;
		case HUMM_VALUE:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Humm"));
			}
			_BufferValue = Value(_NumberSensor, HUMM_VALUE);
			if(_BufferValue != 255){										// Если показания верные
				RealValueSensors[_NumberSensor][VALUE_2] = _BufferValue;
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...done"));
				}
				SensorsError[_NumberSensor][VALUE_2] = 0;
			}
			else{
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...error"));
				}
				SensorsError[_NumberSensor][VALUE_2] = 1;			// Иначе поднимаем флаг ошибочности данных
			}
			break;
		case HUMM_AND_TEMP_VALUE:		
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp"));
			}
			_BufferValue = Value(_NumberSensor, TEMP_VALUE);
			if(_BufferValue != 255){								// Если показания верные
				SensorsError[_NumberSensor][VALUE_1] = 0;
				RealValueSensors[_NumberSensor][VALUE_1] = _BufferValue;
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...done"));
				}
			}
			else{
				SensorsError[_NumberSensor][VALUE_1] = 1;			// Иначе поднимаем флаг ошибочности данных
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...error"));
				}
			}
			// --------------------------------------------
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Humm"));
			}
			_BufferValue = Value(_NumberSensor, HUMM_VALUE);
			if(_BufferValue != 255){								// Если показания верные
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...done"));
				}
				SensorsError[_NumberSensor][VALUE_2] = 0;
				RealValueSensors[_NumberSensor][VALUE_2] = _BufferValue;
			}
			else{
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...error"));
				}
				SensorsError[_NumberSensor][VALUE_2] = 1;			// Иначе поднимаем флаг ошибочности данных
			}
			break;
	}
	ControllPort(_NumberSensor, 0);									// Выключаем управление Controll портом
}


