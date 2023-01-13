
#include <Arduino.h>

#include "Work_TSL2561.h"

#include "../Configuration.h"
#include "../Sensors.h"




Adafruit_TSL2561 tsl1 = Adafruit_TSL2561(TSL2561_ADDR_LOW);
Adafruit_TSL2561 tsl2 = Adafruit_TSL2561(TSL2561_ADDR_FLOAT);
Adafruit_TSL2561 tsl3 = Adafruit_TSL2561(TSL2561_ADDR_HIGH);


uint16_t CalculateToLux(byte _ConfigSensorA, byte AddressSensor, uint16_t ir, uint16_t full){
	Serial.println("CalculateToLux");
	uint16_t broadband = 0;
	uint16_t infrared = 0;
	switch(AddressSensor){
		case TSL2561_ADDR_LOW:
			Serial.println("Privet/////");
			tsl1.getLuminosity(&broadband, &infrared);
			break;
		case TSL2561_ADDR_FLOAT:
			Serial.println("Privet/////");
			tsl2.getLuminosity(&broadband, &infrared);
			break;
		case TSL2561_ADDR_HIGH:
			Serial.println("Privet/////");
			tsl3.getLuminosity(&broadband, &infrared);
			break;
	}
	switch(_ConfigSensorA){
		case 1:
			return broadband;								// Возвращаем значение видимого спектра
		case 3:
			return infrared;								// IR спектра
	}
}


void CalculateTSL2561(byte NumberSensor, byte TypeDataSensor){
	bool Error_Address = false;
	byte AddressSensor = EEPROM.read(E_Address_Sensor + (NumberSensor * 10));
	byte ConfigSensorA = EEPROM_int_read(E_ConfigSensor_A + NumberSensor);
	
	switch(AddressSensor){
		case TSL2561_ADDR_FLOAT:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("_0x39"));
			}
			break;
		case TSL2561_ADDR_LOW:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("_0x29"));
			}
			break;
		case TSL2561_ADDR_HIGH:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("_0x49"));
			}
			break;
		default:
			Error_Address = true;
	}
	
	if(!Error_Address){												// Если нет ошибки адреса датчика
		ControllPort(NumberSensor, 1);								// Включаем управление Controll портом
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.print(F("\t\t\t...measurement Light"));
		}
		
 		sensors_event_t event;

		bool ErrorCalculate = false;								// Ошибка измерения показаний
		
		switch(AddressSensor){
			case TSL2561_ADDR_LOW:			
				if(tsl1.getEvent(&event)){
					if (event.light) {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println(F("...done"));
						}
						Sensors.PresentValue[NumberSensor][VALUE_2] = event.light;
					}
					else {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println("...overload");
						}
					}
				}
				else{
					ErrorCalculate = true;							// Данные не получены
					Sensors.Error_Value[NumberSensor][VALUE_2] = 1;		// Поднимаем ошибку чтения данных датчиком
				}
				break;
			case TSL2561_ADDR_FLOAT:				
				if(tsl2.getEvent(&event)){
					if (event.light) {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println(F("...done"));
						}
						Sensors.PresentValue[NumberSensor][VALUE_2] = event.light;
					}
					else {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println("...overload");
						}
					}
				}
				else{ 
					ErrorCalculate = true;							// Данные не получены
					Sensors.Error_Value[NumberSensor][VALUE_2] = 1;		// Поднимаем ошибку чтения данных датчиком
				}
				break;
			case TSL2561_ADDR_HIGH:
				if(tsl3.getEvent(&event)){
					if (event.light) {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println(F("...done"));
						}
						Sensors.PresentValue[NumberSensor][VALUE_2] = event.light;
					}
					else {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println("...overload");
						}
					}
				}
				else{
					ErrorCalculate = true;								// Данные не получены
					Sensors.Error_Value[NumberSensor][VALUE_2] = 1;		// Поднимаем ошибку чтения данных датчиком
				}
				break;
		}
  
  
		if(ErrorCalculate){														// Если значение датчика не получено
			React_to_Error_Calculate_Value(NumberSensor, LIGHT, Sensors.PresentValue[NumberSensor][VALUE_2]);	// Обработка ошибок чтения показаний
		}
		else{
			Sensors.Error_Value[NumberSensor][VALUE_2] = 0;						// Снимаем возможную ошибку чтения данных
			switch(ConfigSensorA){
				case 1:															// Видимый спектр
					Sensors.PresentValue[NumberSensor][VALUE_2] = event.light;	
					break;
				case 2:															// Полный спектр
					Sensors.PresentValue[NumberSensor][VALUE_2] = event.light;	
					break;
				case 3:															// IR спектр
					Sensors.PresentValue[NumberSensor][VALUE_2] = event.light;	
					break;
			}
 		}
		ControllPort(NumberSensor, 0);								// Выключаем управление Controll портом
	}
	else {
		Sensors.Error_Value[NumberSensor][VALUE_2] = 1;					// Поднимаем ошибку чтения данных датчиком
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.print(F("...error address sensor - 0x")); Serial.println(AddressSensor);
		}
	}
}


