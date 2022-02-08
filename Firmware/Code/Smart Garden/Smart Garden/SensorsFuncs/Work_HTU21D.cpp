
#include <Arduino.h>

#include "Work_HTU21D.h"

#include "../Configuration.h"
#include "../Sensors.h"


HTU21D HTU21(HTU21D_RES_RH8_TEMP12);


void CalculateHTU21D(byte NumberSensor, byte TypeDataSensor){
	float BufferTemp;
	float BufferHumm;
	ControllPort(NumberSensor, 1);													// Включаем управление Controll портом
	switch(TypeDataSensor){
		case 1:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp"));Serial.println(F("...done"));
			}
			RealValueSensors[NumberSensor][VALUE_1] = HTU21.readTemperature(SI70xx_TEMP_READ_AFTER_RH_MEASURMENT);
			if(RealValueSensors[NumberSensor][VALUE_1] != 255){									// Если не ошибки данных
				SensorsError[NumberSensor][VALUE_1] = 0;
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...done"));
				}
			}
			else{
				SensorsError[NumberSensor][VALUE_1] = 1;			// Иначе поднимаем флаг ошибочности данных
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...error"));
				}
			}
			break;
		case 2:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Humm"));
			}
			RealValueSensors[NumberSensor][VALUE_2] = HTU21.readHumidity();
			if(RealValueSensors[NumberSensor][VALUE_2] != 255){
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...done"));
				}
				SensorsError[NumberSensor][VALUE_2] = 0;
			}
			else{
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...error"));
				}
				SensorsError[NumberSensor][VALUE_2] = 1;			// Иначе поднимаем флаг ошибочности данных
			}
			break;
		case 12:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp"));
			}
			RealValueSensors[NumberSensor][VALUE_1] = HTU21.readTemperature(SI70xx_TEMP_READ_AFTER_RH_MEASURMENT);
			if(RealValueSensors[NumberSensor][VALUE_1] != 255){									// Если не ошибки данных
				SensorsError[NumberSensor][VALUE_1] = 0;
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...done"));
				}
			}
			else{
				SensorsError[NumberSensor][VALUE_1] = 1;			// Иначе поднимаем флаг ошибочности данных
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...error"));
				}
			}
			// --------------------------------------------
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Humm"));
			}
			RealValueSensors[NumberSensor][VALUE_2] = HTU21.readHumidity();
			if(RealValueSensors[NumberSensor][VALUE_2] != 255){
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...done"));
				}
				SensorsError[NumberSensor][VALUE_2] = 0;
			}
			else{
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("...error"));
				}
				SensorsError[NumberSensor][VALUE_2] = 1;			// Иначе поднимаем флаг ошибочности данных
			}
			break;
	}
	ControllPort(NumberSensor, 0);								// Выключаем управление Controll портом
}


