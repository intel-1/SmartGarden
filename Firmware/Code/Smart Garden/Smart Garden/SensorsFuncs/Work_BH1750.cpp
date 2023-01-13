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
		
	if (lux < 0) {												// Если значение не получено (ошибка)
		React_to_Error_Calculate_Value(NumberSensor, LIGHT, Sensors.PresentValue[NumberSensor][VALUE_2]);	// Обработка ошибок чтения показаний
	}
	else {
		Sensors.Error_Value[NumberSensor][VALUE_2] = 0;			// Снимаем возможную ошибку чтения данных
		Sensors.PresentValue[NumberSensor][VALUE_2] = lux;		// Сохраняем измеренное значение датчика
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.println(F("...done"));
		}
	}
	ControllPort(NumberSensor, 0);								// Выключаем управление Controll портом
}