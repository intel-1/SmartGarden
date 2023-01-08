
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
	
	byte _QuantitySummValue = 1;
	float _BufferValue;
	while(_QuantitySummValue <= _QuantityRead){
		_BufferValue += ReadValue(_DataType);								// Опрашиваем датчик
		_QuantitySummValue ++;
		_delay_ms(50);
	}
	return _BufferValue / _QuantitySummValue;
}


void CalculateHTU21D(byte NumberSensor, byte TypeDataSensor){
	float _BufferValue;
	ControllPort(NumberSensor, 1);											// Включаем управление Controll портом
	switch(TypeDataSensor){
		case TEMP_AIR_VALUE:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp"));Serial.println(F("...done"));
			}
			_BufferValue = Value(NumberSensor, TEMP_AIR_VALUE);
			if(-40 > _BufferValue && _BufferValue < 105){					// Если температура воздуха в пределе -40-106*С (точность измерения 0.3%)							
				RealValueSensors[NumberSensor][VALUE_1] = _BufferValue;
				SensorsError[NumberSensor][VALUE_1] = 0;
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...done"));
				}
			}
			else {
				React_to_Error_Calculate_Value(NumberSensor, TypeDataSensor, RealValueSensors[NumberSensor][VALUE_1]);	// Обработка ошибок чтения показаний
			}
			break;
		case HUMM_AIR_VALUE:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Humm"));
			}
			_BufferValue = Value(NumberSensor, HUMM_AIR_VALUE);
			if(0 < _BufferValue && _BufferValue < 100){						// Если влажность воздуха в пределе 0-100% (точность измерения 2%)
				RealValueSensors[NumberSensor][VALUE_2] = _BufferValue;
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...done"));
				}
				SensorsError[NumberSensor][VALUE_2] = 0;
			}
			else{
				React_to_Error_Calculate_Value(NumberSensor, TypeDataSensor, RealValueSensors[NumberSensor][VALUE_2]);	// Обработка ошибок чтения показаний
			}
			break;
		case HUMM_AND_TEMP_VALUE:		
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp"));
			}
			_BufferValue = Value(NumberSensor, TEMP_AIR_VALUE);
			if(-40 > _BufferValue && _BufferValue < 105){					// Если температура воздуха в пределе -40-106*С (точность измерения 0.3%)
				SensorsError[NumberSensor][VALUE_1] = 0;
				RealValueSensors[NumberSensor][VALUE_1] = _BufferValue;
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...done"));
				}
			}
			else{
				React_to_Error_Calculate_Value(NumberSensor, TypeDataSensor, RealValueSensors[NumberSensor][VALUE_1]);	// Обработка ошибок чтения показаний
			}
			// --------------------------------------------
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Humm"));
			}
			_BufferValue = Value(NumberSensor, HUMM_AIR_VALUE);
			if(0 < _BufferValue && _BufferValue < 100){						// Если влажность воздуха в пределе 0-100% (точность измерения 2%)
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...done"));
				}
				SensorsError[NumberSensor][VALUE_2] = 0;
				RealValueSensors[NumberSensor][VALUE_2] = _BufferValue;
			}
			else{
				React_to_Error_Calculate_Value(NumberSensor, TypeDataSensor, RealValueSensors[NumberSensor][VALUE_2]);	// Обработка ошибок чтения показаний
			}
			break;
	}
	ControllPort(NumberSensor, 0);											// Выключаем управление Controll портом
}


