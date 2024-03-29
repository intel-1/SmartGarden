
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
			return broadband;								// ���������� �������� �������� �������
		case 3:
			return infrared;								// IR �������
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
	
	if(!Error_Address){												// ���� ��� ������ ������ �������
		ControllPort(NumberSensor, 1);								// �������� ���������� Controll ������
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.print(F("\t\t\t...measurement Light"));
		}
		
 		sensors_event_t event;

		bool ErrorCalculate = false;								// ������ ��������� ���������
		
		switch(AddressSensor){
			case TSL2561_ADDR_LOW:			
				if(tsl1.getEvent(&event)){
					if (event.light) {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println(F("...done"));
						}
						BuferValueSensors.Value[VALUE_2] = event.light;					// ���������� ��������� ��������� �������
						//Sensors.PresentValue[NumberSensor][VALUE_2] = event.light;
					}
					else {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println("...overload");
						}
					}
				}
				else{
					ErrorCalculate = true;								// ������ �� ��������
					Sensors.Error_Value[NumberSensor][VALUE_2] = 1;		// ��������� ������ ������ ������ ��������
				}
				break;
			case TSL2561_ADDR_FLOAT:				
				if(tsl2.getEvent(&event)){
					if (event.light) {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println(F("...done"));
						}
						BuferValueSensors.Value[VALUE_2] = event.light;					// ���������� ��������� ��������� �������
						//Sensors.PresentValue[NumberSensor][VALUE_2] = event.light;
					}
					else {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println("...overload");
						}
					}
				}
				else{ 
					ErrorCalculate = true;								// ������ �� ��������
					Sensors.Error_Value[NumberSensor][VALUE_2] = 1;		// ��������� ������ ������ ������ ��������
				}
				break;
			case TSL2561_ADDR_HIGH:
				if(tsl3.getEvent(&event)){
					if (event.light) {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println(F("...done"));
						}
						BuferValueSensors.Value[VALUE_2] = event.light;					// ���������� ��������� ��������� �������
						//Sensors.PresentValue[NumberSensor][VALUE_2] = event.light;
					}
					else {
						if (OUTPUT_LEVEL_UART_SENSOR){
							Serial.println("...overload");
						}
					}
				}
				else{
					ErrorCalculate = true;								// ������ �� ��������
					Sensors.Error_Value[NumberSensor][VALUE_2] = 1;		// ��������� ������ ������ ������ ��������
				}
				break;
		}
  
  
		if(ErrorCalculate){														// ���� �������� ������� �� ��������
			React_to_Error_Calculate_Value(NumberSensor, LIGHT, Sensors.PresentValue[NumberSensor][VALUE_2]);	// ��������� ������ ������ ���������
		}
		else{
			Sensors.Error_Value[NumberSensor][VALUE_2] = 0;						// ������� ��������� ������ ������ ������
			switch(ConfigSensorA){
				case 1:															// ������� ������
					BuferValueSensors.Value[VALUE_2] = event.light;				// ���������� ��������� ��������� �������
					//Sensors.PresentValue[NumberSensor][VALUE_2] = event.light;	
					break;
				case 2:															// ������ ������
					BuferValueSensors.Value[VALUE_2] = event.light;				// ���������� ��������� ��������� �������
					//Sensors.PresentValue[NumberSensor][VALUE_2] = event.light;	
					break;
				case 3:															// IR ������
					BuferValueSensors.Value[VALUE_2] = event.light;				// ���������� ��������� ��������� �������
					//Sensors.PresentValue[NumberSensor][VALUE_2] = event.light;	
					break;
			}
 		}
		
		BuferValueSensors.Allow[VALUE_2] = true;					// � ��������� ��� ���������
		Recording_Sensor_Readings(NumberSensor);					// ��������� ��������� ���������
		
		ControllPort(NumberSensor, 0);								// ��������� ���������� Controll ������
	}
	else {
		Sensors.Error_Value[NumberSensor][VALUE_2] = 1;				// ��������� ������ ������ ������ ��������
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.print(F("...error address sensor - 0x")); Serial.println(AddressSensor);
		}
	}
}


