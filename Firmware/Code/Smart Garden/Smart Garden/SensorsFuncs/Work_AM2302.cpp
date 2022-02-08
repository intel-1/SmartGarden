
#include <Arduino.h>
#include "../Configuration.h"
#include "Work_AM2302.h"
#include "../Sensors.h"

AM2320 THsensor;

void CalculateAM2302(byte NumberSensor, byte TypeDataSensor){
	ControllPort(NumberSensor, 1);								// Включаем управление Controll портом
	switch(TypeDataSensor){
		case 1:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp"));
				Serial.println(F("...done"));
			}
			break;
		case 2:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Humm")); Serial.println(F("...done"));
			}
			break;
		case 12:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp")); Serial.println(F("...done"));
				Serial.print(F("\t\t\t...measurement Humm")); Serial.println(F("...done"));
			}
			break;
	}
	ControllPort(NumberSensor, 0);								// Выключаем управление Controll портом
}