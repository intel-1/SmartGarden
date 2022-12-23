

#include <Arduino.h>

#include "Work_INA219.h"

#include "../Configuration.h"
#include "../Sensors.h"


Adafruit_INA219 ina219;
Adafruit_INA219 ina219_int(ADDRESS_INPUT_INA);


void CalculateINA219(byte NumberSensor, byte TypeDataSensor){

	ControllPort(NumberSensor, 1);								// Включаем управление Controll портом
	switch(TypeDataSensor){
		case 6:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Voltage")); Serial.println(F("...done"));
			}
			RealValueSensors[NumberSensor][VALUE_1] = ina219.getBusVoltage_V() + (ina219.getShuntVoltage_mV() / 1000);
			break;
		case 7:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Current")); Serial.println(F("...done"));
			}
			RealValueSensors[NumberSensor][VALUE_2] = ina219.getCurrent_mA();
			break;
		case 67:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Voltage")); Serial.println(F("...done"));
				Serial.print(F("\t\t\t...measurement Current")); Serial.println(F("...done"));
			}
			RealValueSensors[NumberSensor][VALUE_2] = ina219.getCurrent_mA();
			RealValueSensors[NumberSensor][VALUE_1] = ina219.getBusVoltage_V();
			break;
	}
	ControllPort(NumberSensor, 0);								// Выключаем управление Controll портом
}