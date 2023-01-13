
#include <Arduino.h>

#include "Work_BMP280.h"

#include "../Configuration.h"
#include "../Sensors.h"
#include "../GSM.h"

 
BMP280 bmp280[2];





float ReadValueBMP280(byte Address, byte TypeDataSensor){
	switch (Address){
		case 0x76:
			switch(TypeDataSensor){
				case 1:
					return bmp280[0].readTemperature();
					break;
				case 5:
					return bmp280[0].readPressure() /*/ 100.0F*/ * 0.0075;		// �� ��. ������
					break;
			}
			break;
		case 0x77:
			switch(TypeDataSensor){
				case 1:
					return bmp280[1].readTemperature();
					break;
				case 5:
					return bmp280[1].readPressure() /*/ 100.0F*/ * 0.0075;		// �� ��. ������
					break;
			}
			break;
	}
}


void CalculateBMP280(byte NumberSensor, byte TypeDataSensor){
	byte AdressSensor = EEPROM.read(E_Address_Sensor + (NumberSensor * 10));
	
	ControllPort(NumberSensor, 1);								// �������� ���������� Controll ������
	switch(TypeDataSensor){
		case TEMP_AIR_VALUE:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp")); Serial.println(F("...done"));
			}			
			
			Sensors.PresentValue[NumberSensor][VALUE_1] = ReadValueBMP280(AdressSensor, 1);
			
			React_to_Error_Calculate_Value(NumberSensor, TypeDataSensor, Sensors.PresentValue[NumberSensor][VALUE_1]);	// ��������� ������ ������ ���������			
			break;
		case ATMOSPHERIC_PRESSURE:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Press")); Serial.println(F("...done"));
			}
			Sensors.PresentValue[NumberSensor][VALUE_3] = ReadValueBMP280(AdressSensor, 5);
			React_to_Error_Calculate_Value(NumberSensor, TypeDataSensor, Sensors.PresentValue[NumberSensor][VALUE_3]);	// ��������� ������ ������ ���������
			break;
		case TEMP_AIR_AND_PRESS:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp")); Serial.println(F("...done"));
				Serial.print(F("\t\t\t...measurement Press")); Serial.println(F("...done"));
			}
			Sensors.PresentValue[NumberSensor][VALUE_1] = ReadValueBMP280(AdressSensor, 1);
			Sensors.PresentValue[NumberSensor][VALUE_3] = ReadValueBMP280(AdressSensor, 5);
			React_to_Error_Calculate_Value(NumberSensor, TypeDataSensor, Sensors.PresentValue[NumberSensor][VALUE_1]);	// ��������� ������ ������ ���������
			React_to_Error_Calculate_Value(NumberSensor, TypeDataSensor, Sensors.PresentValue[NumberSensor][VALUE_3]);	// ��������� ������ ������ ���������
			break;
	}
	ControllPort(NumberSensor, 0);								// ��������� ���������� Controll ������
}