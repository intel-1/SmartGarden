#include <Arduino.h>


#include "Work_BH1750.h"

#include "../Configuration.h"
#include "../Sensors.h"


BH1750 BH1750_1(0x23);
BH1750 BH1750_2(0x5c);


boolean ChangeMeasurementTime(byte AddressSensor, byte MTreg){					// Ф-ция изменения времени измерения в зависимости от освещения
	switch(AddressSensor){
		case 0x23:
			if(BH1750_1.setMTreg(MTreg)){
				return true;
			}
			else return false;
			break;
		case 0x5c:
			if(BH1750_2.setMTreg(MTreg)){
				return true;
			}
			else return false;
			break;
	}
}

void CalculateBH1750(byte NumberSensor, byte TypeDataSensor){
	byte AddressSensor = EEPROM.read(E_Address_Sensor + (NumberSensor * 10));
	float lux;
	ControllPort(NumberSensor, 1);								// Включаем управление Controll портом
	
	if (OUTPUT_LEVEL_UART_SENSOR){
		//Serial.print(F("::Choice_BH1750"));
		//Serial.print(F("BH1750"));
		switch(EEPROM.read(E_Address_Sensor + (NumberSensor * 10))){
			case 0x23:
				Serial.println(F("_0x23"));
				break;
			case 0x5c:
				Serial.println(F("_0x5c"));
				break;
		}
	}
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
		
	if (lux < 0) {											// Если значение не получено
		SensorsError[NumberSensor][VALUE_2] = 1;			// Поднимаем ошибку чтения
		RealValueSensors[NumberSensor][VALUE_2] = 0;		// Обнуляем значение в массиве
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.println(F("...ошибка или показания меньше нуля"));
		}
	}
	else {
		SensorsError[NumberSensor][VALUE_2] = 0;			// Снимаем возможную ошибку чтения данных
		RealValueSensors[NumberSensor][VALUE_2] = lux;		// Сохраняем измеренное значение датчика
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.println(F("...done"));
		}
		if (lux > 40000.0) {									// reduce measurement time - needed in direct sun light			
			if (ChangeMeasurementTime(AddressSensor, 32)) {
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("Setting MTReg to low value for high light environment"));
				}
			}
			else {
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.println(F("Error setting MTReg to low value for high light environment"));
				}
			}
		}
		else {
			if (lux > 10.0) {									// typical light environment				
				if (ChangeMeasurementTime(AddressSensor, 69)) {
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.println(F("Setting MTReg to default value for normal light environment"));
					}
				}
				else {
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.println(F("Error setting MTReg to default value for normal light environment"));
					}
				}
			}
			else {
				if (lux <= 10.0) {								//very low light environment					
					if (ChangeMeasurementTime(AddressSensor, 138)) {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println(F("Setting MTReg to high value for low light environment"));
						}
					}
					else {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println(F("Error setting MTReg to high value for low light environment"));
						}
					}
				}
			}
		}
	}
	ControllPort(NumberSensor, 0);								// Выключаем управление Controll портом
}