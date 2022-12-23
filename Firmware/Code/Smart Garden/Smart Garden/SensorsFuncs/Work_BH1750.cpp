#include <Arduino.h>


#include "Work_BH1750.h"

#include "../Configuration.h"
#include "../Sensors.h"


BH1750 BH1750_1(0x23);
BH1750 BH1750_2(0x5c);


void CalculateBH1750(byte NumberSensor, byte TypeDataSensor){
	byte AddressSensor = EEPROM.read(E_Address_Sensor + (NumberSensor * 10));
	float lux;
	
	if (OUTPUT_LEVEL_UART_SENSOR){
		switch(EEPROM.read(E_Address_Sensor + (NumberSensor * 10))){
			case 0x23:
				Serial.println(F("_0x23"));
				break;
			case 0x5c:
				Serial.println(F("_0x5c"));
				break;
		}
	}
	
	ControllPort(NumberSensor, 1);								// Включаем управление Controll портом
	
	if (OUTPUT_LEVEL_UART_SENSOR){
		Serial.print(F("\t\t\t...measurement Light"));
	}
		
	switch(AddressSensor){
		case 0x23:
			lux = BH1750_1.readLightLevel(true);
			break;
		case 0x5c:
			lux = BH1750_2.readLightLevel(true);
			break;
	}
		
	if (lux < 0) {												// Если значение не получено
		SensorsError[NumberSensor][VALUE_2] = 1;				// Поднимаем ошибку чтения
		RealValueSensors[NumberSensor][VALUE_2] = 0;			// Обнуляем значение в массиве
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.println(F("...error or reading less than zero"));
		}
	}
	else {
		SensorsError[NumberSensor][VALUE_2] = 0;				// Снимаем возможную ошибку чтения данных
		RealValueSensors[NumberSensor][VALUE_2] = lux;			// Сохраняем измеренное значение датчика
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.println(F("...done"));
		}
	}
	ControllPort(NumberSensor, 0);								// Выключаем управление Controll портом
}