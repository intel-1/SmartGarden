
#include <Arduino.h>

#include "Work_BME280.h"

#include "../Configuration.h"
#include "../Sensors.h"


BME280 bme280[2];




float ReadValue(byte Address, byte TypeDataSensor){
	switch (Address){
		case 0x76:
			switch(TypeDataSensor){
				case 1:
					return bme280[0].readTemperature();
					break;
				case 2:
					return bme280[0].readHumidity();
					break;
				case 5:
					return bme280[0].readPressure() * 0.0075;
					break;
			}
			break;
		case 0x77:
			switch(TypeDataSensor){
				case 1:
					return bme280[1].readTemperature();
					break;
				case 2:
					return bme280[1].readHumidity();
					break;
				case 5:
					return bme280[1].readPressure() * 0.0075;
					break;
			}
			break;
	}
}


void CalculateBME280(byte NumberSensor, byte TypeDataSensor){
	byte AdressSensor = EEPROM.read(E_Address_Sensor + (NumberSensor * 10));
	
	ControllPort(NumberSensor, 1);								// Включаем управление Controll портом
	switch(TypeDataSensor){
		case 1:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp")); Serial.println(F("...done"));
			}
			Sensors.PresentValue[NumberSensor][VALUE_1] = ReadValue(AdressSensor, 1);
			break;
		case 2:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Humm"));Serial.println(F("...done"));
			}
			Sensors.PresentValue[NumberSensor][VALUE_2] = ReadValue(AdressSensor, 2);
			break;
		case 5:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Press")); Serial.println(F("...done"));
			}
			Sensors.PresentValue[NumberSensor][VALUE_3] = ReadValue(AdressSensor, 5);
			break;
		case 12:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp")); Serial.println(F("...done"));
				Serial.print(F("\t\t\t...measurement Humm")); Serial.println(F("...done"));
			}
			Sensors.PresentValue[NumberSensor][VALUE_1] = ReadValue(AdressSensor, 1);
			Sensors.PresentValue[NumberSensor][VALUE_2] = ReadValue(AdressSensor, 2);
			break;
		case 25:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Humm")); Serial.println(F("...done"));
				Serial.print(F("\t\t\t...measurement Press")); Serial.println(F("...done"));
			}
			Sensors.PresentValue[NumberSensor][VALUE_2] = ReadValue(AdressSensor, 2);
			Sensors.PresentValue[NumberSensor][VALUE_3] = ReadValue(AdressSensor, 5);
			break;
		case 125:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp")); Serial.println(F("...done"));
				Serial.print(F("\t\t\t...measurement Humm")); Serial.println(F("...done"));
				Serial.print(F("\t\t\t...measurement Press")); Serial.println(F("...done"));
			}
			Sensors.PresentValue[NumberSensor][VALUE_1] = ReadValue(AdressSensor, 1);
			Sensors.PresentValue[NumberSensor][VALUE_2] = ReadValue(AdressSensor, 2);
			Sensors.PresentValue[NumberSensor][VALUE_3] = ReadValue(AdressSensor, 5);
			break;
	}
	ControllPort(NumberSensor, 0);								// Выключаем управление Controll портом
}
