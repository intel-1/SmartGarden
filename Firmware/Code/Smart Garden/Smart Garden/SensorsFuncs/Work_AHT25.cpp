
#include <Arduino.h>
#include "../Configuration.h"
#include "Work_AHT25.h"
#include "../Sensors.h"


Adafruit_AHTX0 AHT25;


void CalculateAHT25(byte NumberSensor, byte TypeDataSensor){
	sensors_event_t humidity, temp;
	AHT25.getEvent(&humidity, &temp);
	
	if(AHT25.getStatus() != 0xFF){														// ���� ������ ���������
		switch(TypeDataSensor){
			case TEMP_AIR_VALUE:	
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.print(F("\t\t\t...measurement Temp")); 
				}
				if(-40 <= temp.temperature && temp.temperature <= 120){					// ���� ����������� ������� � ������� -40-120*� (�������� ��������� 0.3%)
					RealValueSensors[NumberSensor][VALUE_1] = temp.temperature;
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.println(F("...done"));
					}
				}
				else{
					React_to_Error_Calculate_Value(NumberSensor, TEMP_AIR_VALUE, RealValueSensors[NumberSensor][VALUE_1]);	// ��������� ������ ������ ���������
				}
				break;
			case HUMM_AIR_VALUE:
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.print(F("\t\t\t...measurement Humm")); 
				}
				if(0 <= temp.temperature && temp.temperature <= 100){					// ���� ��������� ������� � ������� 0-100*� (�������� ��������� 0.3%)
					RealValueSensors[NumberSensor][VALUE_2] = humidity.relative_humidity;
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.println(F("...done"));
					}
				}		
				else{
					React_to_Error_Calculate_Value(NumberSensor, HUMM_AIR_VALUE, RealValueSensors[NumberSensor][VALUE_2]);	// ��������� ������ ������ ���������
				}	
				break;	
			case HUMM_AND_TEMP_VALUE:	
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.print(F("\t\t\t...measurement Temp")); 
				}
				if(-40 <= temp.temperature && temp.temperature <= 105){					// ���� ����������� ������� � ������� -40-120*� (�������� ��������� 0.3%)
					RealValueSensors[NumberSensor][VALUE_1] = temp.temperature;
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.println(F("...done"));
					}
				}
				else{
					React_to_Error_Calculate_Value(NumberSensor, TEMP_AIR_VALUE, RealValueSensors[NumberSensor][VALUE_1]);	// ��������� ������ ������ ���������
				}
				// -------------------------------------------------------------------------------
				if (OUTPUT_LEVEL_UART_SENSOR){
					Serial.print(F("\t\t\t...measurement Humm")); 
				}
				if(0 <= temp.temperature && temp.temperature <= 100){					// ���� ��������� ������� � ������� 0-100*� (�������� ��������� 0.3%)
					RealValueSensors[NumberSensor][VALUE_2] = humidity.relative_humidity;
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.println(F("...done"));
					}
				}
				else{
					React_to_Error_Calculate_Value(NumberSensor, HUMM_AIR_VALUE, RealValueSensors[NumberSensor][VALUE_2]);	// ��������� ������ ������ ���������
				}
				break;
		}
	}
	else{
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.println(F("\t\t\t...sensor not connected"));
		}
		SensorsError[NumberSensor][VALUE_1] = 1;					// ����� ��������� ���� ����������� ������
		SensorsError[NumberSensor][VALUE_2] = 1;					// ����� ��������� ���� ����������� ������
	}
}