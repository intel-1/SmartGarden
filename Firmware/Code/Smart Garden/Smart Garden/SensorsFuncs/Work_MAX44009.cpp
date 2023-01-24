#include <Arduino.h>

#include "Work_MAX44009.h"

#include "../Configuration.h"
#include "../Sensors.h"


MAX44009 MAX44009_1(0x4A);
MAX44009 MAX44009_2(0x4B);



float ReadData(byte NumberSensor, byte _Address){
	byte _QuantityRead = EEPROM.read(E_QuantityReadSensors + NumberSensor);
	
	byte _QuantitySummValue = 1;
	float _BufferValue;
	while(_QuantitySummValue <= _QuantityRead){
		//_BufferValue += MAX44009_get_lux(_Address);							// Опрашиваем датчик
		_QuantitySummValue ++;
		_delay_ms(50);
	}
	return _BufferValue / _QuantitySummValue;
}



void CalculateMAX44009(byte NumberSensor, byte TypeDataSensor){
	bool Error_Address = false;
	bool State_Connect = false;
	
	byte AddressSensor = EEPROM.read(E_Address_Sensor + (NumberSensor * 10));
	
	switch(AddressSensor){
		case 0x4a:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("_0x4a"));
			}
			if(MAX44009_1.isConnected()){State_Connect = true;}
			break;
		case 0x4b:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("_0x4b"));
			}
			if(MAX44009_2.isConnected()){State_Connect = true;}
			break;
		default:
			Error_Address = true;
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println();
				Serial.println(F("\t\t\t...Error address"));
			}
	}
	
	if(!Error_Address){																	// Если не висит ошибка адреса датчика
		if(State_Connect){
			ControllPort(NumberSensor, 1);												// Включаем управление Controll портом
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Light"));
			}
	
			Sensors.Error_Value[NumberSensor][VALUE_2] = 0;									// Снимаем возможную ошибку чтения данных
			
			switch(AddressSensor){
				case 0x4a:
					BuferValueSensors.Value[VALUE_2] = MAX44009_1.getLux();					// Записываем временное показание датчика
					//Sensors.PresentValue[NumberSensor][VALUE_2] = MAX44009_1.getLux();	// Сохраняем измеренное значение датчика
					break;
				case 0x4b:
					BuferValueSensors.Value[VALUE_2] = MAX44009_2.getLux();					// Записываем временное показание датчика
					//Sensors.PresentValue[NumberSensor][VALUE_2] = MAX44009_2.getLux();	// Сохраняем измеренное значение датчика
					break;
			}
			
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("...done"));
			}
			
			BuferValueSensors.Allow[VALUE_2] = true;						// и разрешаем его обработку
			Recording_Sensor_Readings(NumberSensor);						// Запускаем обработку показаний
			
			ControllPort(NumberSensor, 0);									// Выключаем управление Controll портом
		}
		else{ 
			Sensors.Error_Value[NumberSensor][VALUE_2] = 1;
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("\t\t\t...Not connection"));
			}
		}
	}
	else Sensors.Error_Value[NumberSensor][VALUE_2] = 1;
}