
#include <Arduino.h>

#include "Work_TSL2561.h"

#include "../Configuration.h"
#include "../Sensors.h"


#define TSL2561_ADDR_LOW  0x29
#define TSL2561_ADDR_FLOAT 0x39
#define TSL2561_ADDR_HIGH 0x49


Adafruit_TSL2561 tsl1 = Adafruit_TSL2561(TSL2561_ADDR_LOW);
Adafruit_TSL2561 tsl2 = Adafruit_TSL2561(TSL2561_ADDR_FLOAT);
Adafruit_TSL2561 tsl3 = Adafruit_TSL2561(TSL2561_ADDR_HIGH);



uint16_t CalculateToLux(uint8_t Sensor, uint16_t ir, uint16_t full){
	uint16_t broadband = 0;
	uint16_t infrared = 0;
	switch(Sensor){
		case 0x39:
			tsl1.getLuminosity(&broadband, &infrared);
			//return tsl1.calculateLux(full,ir);
		case 0x29:
			tsl2.getLuminosity(&broadband, &infrared);
			//return tsl2.calculateLux(full,ir);
		case 0x49:
			tsl3.getLuminosity(&broadband, &infrared);
			//return tsl3.calculateLux(full,ir);
	}
	switch(EEPROM_int_read(E_ConfigSensor_A + Sensor)){
		case 1:
			return broadband;								// Возвращаем значение видимого спектра
		case 3:
			return infrared;								// IR спектра
	}
}


void CalculateTSL2561(byte NumberSensor, byte TypeDataSensor){
	uint32_t lum;
	byte AddressSensor = EEPROM.read(E_Address_Sensor + (NumberSensor * 10));
	byte Sensor;

	sensors_event_t event;
	
	switch(AddressSensor){
		case 0x39:
			lum = tsl1.getEvent(&event);
			Sensor = 0x39;
			break;
		case 0x29:
			lum = tsl2.getEvent(&event);
			Sensor = 0x29;
			break;
		case 0x49:
			lum = tsl3.getEvent(&event);
			Sensor = 0x49;
			break;
	}
	
	if (OUTPUT_LEVEL_UART_SENSOR){
		switch(EEPROM.read(E_Address_Sensor + (NumberSensor * 10))){
			case 0x39:
				Serial.println(F("_0x39"));
				break;
			case 0x29:
				Serial.println(F("_0x29"));
				break;
			case 0x49:
				Serial.println(F("_0x49"));
				break;
		}
	}
	ControllPort(NumberSensor, 1);								// Включаем управление Controll портом
	if (OUTPUT_LEVEL_UART_SENSOR){
		Serial.print(F("\t\t\t...measurement Light"));
		Serial.println(F("...done"));
	}
	
	uint16_t ir = lum >> 16;
	uint16_t full = lum & 0xFFFF;
	
	if(lum == 0){												// Если значение датчика не получена
		SensorsError[NumberSensor][VALUE_2] = 1;				// Поднимаем ошибку чтения данных датчиком
		RealValueSensors[NumberSensor][VALUE_2] = 0;			// Обнуляем значение датчика в массиве
	}
	else{
		SensorsError[NumberSensor][VALUE_2] = 0;				// Снимаем возможную ошибку чтения данных
		switch(EEPROM.read(E_ConfigSensor_A + NumberSensor)){
			case 1:
				if(EEPROM_int_read(E_ConfigSensor_B + NumberSensor) == 1){
					RealValueSensors[NumberSensor][VALUE_2] = CalculateToLux(Sensor, ir, full);	// Измеряем в люксах
				}
				else RealValueSensors[NumberSensor][VALUE_2] = full - ir;						// Измеряем в люминах
				break;
			case 2:
				if(EEPROM_int_read(E_ConfigSensor_B + NumberSensor) == 1){
					RealValueSensors[NumberSensor][VALUE_2] = CalculateToLux(Sensor, ir, full);	// Измеряем в люксах
				}	
				else RealValueSensors[NumberSensor][VALUE_2] = full;							// Измеряем в люминах
				break;
			case 3:
				if(EEPROM_int_read(E_ConfigSensor_B + NumberSensor) == 1){
					RealValueSensors[NumberSensor][VALUE_2] = CalculateToLux(Sensor, ir, full);	// Измеряем в люксах
				}
				else RealValueSensors[NumberSensor][VALUE_2] = ir;								// Измеряем в люминах
				break;
		}
	}
	ControllPort(NumberSensor, 0);								// Выключаем управление Controll портом
}


