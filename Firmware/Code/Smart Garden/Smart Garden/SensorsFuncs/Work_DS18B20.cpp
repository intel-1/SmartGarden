

#include <Arduino.h>

#include "Work_DS18B20.h"

#include "../Configuration.h"
#include "../Sensors.h"
#include "../GSM.h"
#include "../ConfigSensors.h"



OneWire ds1(PORT_INPUT_GPIO_P1);
OneWire ds2(PORT_INPUT_GPIO_P2);
OneWire ds3(PORT_INPUT_GPIO_P3);
OneWire ds4(PORT_INPUT_GPIO_P4);
OneWire ds5(PORT_INPUT_GPIO_P5);
OneWire ds6(PORT_INPUT_GPIO_P6);
DallasTemperature sensors1(&ds1);
DallasTemperature sensors2(&ds2);
DallasTemperature sensors3(&ds3);
DallasTemperature sensors4(&ds4);
DallasTemperature sensors5(&ds5);
DallasTemperature sensors6(&ds6);



void ViewError(){
	if (OUTPUT_LEVEL_UART_SENSOR){
		Serial.println(F("...error"));
	}
}


void CalculateDS18B20(byte NumberSensor){
	float RealValue;
	bool SensorConnect = false;
	DeviceAddress AddresSensor = {	EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 0),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 1),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 2),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 3),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 4),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 5),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 6),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 7)};
		
	
	byte Config_Sensor_B = EEPROM_int_read(E_ConfigSensor_B + NumberSensor*2);
	
	switch(Config_Sensor_B){
		case CONFIG_SENSOR_B_INPUT_GPIO_P1:
			if(sensors1.isConnected(AddresSensor)) SensorConnect = true;
			break;
		case CONFIG_SENSOR_B_INPUT_GPIO_P2:
			if(sensors2.isConnected(AddresSensor)) SensorConnect = true;
			break;
		case CONFIG_SENSOR_B_INPUT_GPIO_P3:
			if(sensors3.isConnected(AddresSensor)) SensorConnect = true;
			break;
		case CONFIG_SENSOR_B_INPUT_GPIO_P4:
			if(sensors4.isConnected(AddresSensor)) SensorConnect = true;
			break;
		case CONFIG_SENSOR_B_INPUT_GPIO_P5:
			if(sensors5.isConnected(AddresSensor)) SensorConnect = true;
			break;
		case CONFIG_SENSOR_B_INPUT_GPIO_P6:
			if(sensors6.isConnected(AddresSensor)) SensorConnect = true;
			break;
	}
	
	if(SensorConnect){																	// ���� ������ ��������� � ����
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.print(F("\t\t\t...measurement Temp"));
		}
		ControllPort(NumberSensor, 1);													// �������� ���������� Controll ������
	
		switch(Config_Sensor_B){
			case CONFIG_SENSOR_B_INPUT_GPIO_P1:
				RealValue = sensors1.getTempC(AddresSensor);							// �������� ���������� ��������
				break;
			case CONFIG_SENSOR_B_INPUT_GPIO_P2:
				RealValue = sensors2.getTempC(AddresSensor);
				break;
			case CONFIG_SENSOR_B_INPUT_GPIO_P3:
				RealValue = sensors3.getTempC(AddresSensor);
				break;
			case CONFIG_SENSOR_B_INPUT_GPIO_P4:
				RealValue = sensors4.getTempC(AddresSensor);
				break;
			case CONFIG_SENSOR_B_INPUT_GPIO_P5:
				RealValue = sensors5.getTempC(AddresSensor);
				break;
			case CONFIG_SENSOR_B_INPUT_GPIO_P6:
				RealValue = sensors6.getTempC(AddresSensor);
				break;
		}
	
		switch((int)RealValue){																// ����������� ��������� � int ��� ����������� ���������� ��������
			case -127:																		// ������ �� ��������
				SensorsError[NumberSensor][VALUE_1] = 1;									// ������ ������ ������ ��������
				EEPROM.put(E_QuantityErrors + NumberSensor, EEPROM_int_read(E_QuantityErrors + NumberSensor*2) + 1);	// ����������� ������� ���������� ������
				ViewError();
				if(EEPROM.read(E_ReactToMistakes_Ext + NumberSensor) == 1){					// ���� ��������� �� ������� ��� ��� ������ ������
					if(EEPROM.read(E_SensorOff_SMS + NumberSensor) != 1){					// ���� �� ������������ ���				
						Send_SMS(String(F("Sensor ")) + NameSensor[NumberSensor] + (F(" is off")), GSM_SMS_ERROR);  
						EEPROM.write(E_SensorOff_SMS + NumberSensor, 1);	
					}
				}
				EEPROM.write(E_ReadSensorOK_SMS + NumberSensor, 0);
				break;
			case 85:																			// ������ ������ ������
				EEPROM.put(E_QuantityErrors + NumberSensor, EEPROM_int_read(E_QuantityErrors + NumberSensor*2) + 1);	// ����������� ������� ���������� ������
				SensorsError[NumberSensor][VALUE_1] = 1;										// ������ ������ ������ ��������
				ViewError();
				if(EEPROM.read(E_ReactToMistakes_Ext + NumberSensor) == 1){						// ���� ��������� �� �������� ��� ��� ������ ������
					if(EEPROM.read(E_ErrorReadSensor_SMS + NumberSensor) != 1){					// ���� �� ������������ ���
						EEPROM.write(E_ErrorReadSensor_SMS + NumberSensor, 1);	 
					}
				}
				EEPROM.write(E_ReadSensorOK_SMS + NumberSensor, 0);
				break;
			default:																	// ��� ������, ��������� ������� �������
				if(-55 <= RealValue && RealValue <= 125){								// �������� ���������� ����������: -55�+125�C
				
				
					RealValueSensors[NumberSensor][VALUE_1] = RealValue;
					SensorsError[NumberSensor][VALUE_1] = 0;							// ������� ������ ������ �������� ��� ����� �������� ��� ������ �������
			
					if(EEPROM.read(E_ReactToMistakes_Ext + NumberSensor) == 1){			// ���� ��������� �� �������� ��� ��� ������� ��������
						if(EEPROM.read(E_ReadSensorOK_SMS + NumberSensor) == 0){		// ���� �� ������������ ���
							EEPROM.write(E_ReadSensorOK_SMS + NumberSensor, 1);
						}
					}
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.println(F("...done"));
					}
					switch(Config_Sensor_B){											// ������ ������� �������� �����������
						case CONFIG_SENSOR_B_INPUT_GPIO_P1:
							sensors1.requestTemperatures();					
							break;
						case CONFIG_SENSOR_B_INPUT_GPIO_P2:
							sensors2.requestTemperatures();
							break;
						case CONFIG_SENSOR_B_INPUT_GPIO_P3:
							sensors3.requestTemperatures();
							break;
						case CONFIG_SENSOR_B_INPUT_GPIO_P4:
							sensors4.requestTemperatures();
							break;
						case CONFIG_SENSOR_B_INPUT_GPIO_P5:
							sensors5.requestTemperatures();
							break;
						case CONFIG_SENSOR_B_INPUT_GPIO_P6:
							sensors6.requestTemperatures();
							break;
					}
				}
	// 			else{
	// 				SensorsError[NumberSensor][VALUE_1] = 1;									// ������ ������ ������ ��������
	// 				EEPROM.put(E_QuantityErrors + NumberSensor, EEPROM_int_read(E_QuantityErrors + NumberSensor*2) + 1);	// ����������� ������� ���������� ������
	// 				ViewError();
	// 				if(EEPROM.read(E_ReactToMistakes_Ext + NumberSensor) == 1){					// ���� ��������� �� ������� ��� ��� ������ ������
	// 					if(EEPROM.read(E_SensorOff_SMS + NumberSensor) != 1){					// ���� �� ������������ ���					
	// 						SendSMS(String(F("Sensor ")) + NameSensor[NumberSensor] + (F(" is off")), GSM_ERROR_SMS);
	// 						EEPROM.write(E_SensorOff_SMS + NumberSensor, 1);
	// 					}
	// 				}
	// 				EEPROM.write(E_ReadSensorOK_SMS + NumberSensor, 0);
	// 			}
				break;
		}
		ControllPort(NumberSensor, 0);								// ��������� ���������� Controll ������
	}
	else{
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.println(F("\t\t\t...sensor not connected"));
		}
		SensorsError[NumberSensor][VALUE_1] = 1;					// ����� ��������� ���� ����������� ������
	}
}




void printAddress(DeviceAddress deviceAddress, byte Units, byte ViewLogs){
/*
		Units	-	� ����� ������� �������� �����:	SENSORS_SEARCH_OUTPUT_HEX_FORMAT - HEX
													SENSORS_SEARCH_OUTPUT_DEC_FORMAT - DEC
		
 		ViewLogs:	SENSORS_SEARCH_TO_APP	- ����� ������ ��� ���������� ��� ���������
 					SENSORS_SEARCH_TO_UART	- ����� ������ ��� UART
*/

	for (uint8_t i = 0; i < 8; i++){
		if (deviceAddress[i] < 16) Serial.print(F("0"));
		switch(Units){
	 		case SENSORS_SEARCH_OUTPUT_HEX_FORMAT:
	 			Serial.print(deviceAddress[i], HEX);
	 			break;
	 		case SENSORS_SEARCH_OUTPUT_DEC_FORMAT:
	 			Serial.print(deviceAddress[i], DEC);
	 			break;
	 	}
		switch(ViewLogs){
			case SENSORS_SEARCH_TO_APP:
				if(i != 7) Serial.print(F(" "));
				break;
			case SENSORS_SEARCH_TO_UART:
				if(i != 7) Serial.print(F(":"));
				break;
		}
	}
	Serial.println();
}


void Print_Count_Sensors(byte countSensors, byte Number_Input_GPIO_Port){
	if(countSensors > 0){																							// ���� ����� ������� �� �����
		Serial.print(F("\t\tOn Input_GPIO_Port ")); Serial.print(Number_Input_GPIO_Port); Serial.print(F(": "));	// ���������� ���������� ��������� ��������
		Serial.println(countSensors);
	}
}


void DS18B20_scaner(bool LogsToUART){
/*
	LogsToUART:	0 - ����� ������ ��� ��������
				1 - ����� ������ � ���������
*/
	
	if(LogsToUART == SENSORS_SEARCH_TO_UART){
		Serial.println(F("\tDS18B20 sensors:"));
	}
	
	byte countSensors;
	DeviceAddress *sensorsUnique;
	
	sensors1.begin();
	sensors2.begin();
	sensors3.begin();
	sensors4.begin();
	sensors5.begin();
	sensors6.begin();
	
	// ��������� ��������� ����������� �� ���� ��������
	sensors1.requestTemperatures();
	sensors2.requestTemperatures();
	sensors3.requestTemperatures();
	sensors4.requestTemperatures();
	sensors5.requestTemperatures();
	sensors6.requestTemperatures();

	delay(1200);
	
	sensorsUnique = new DeviceAddress[QUANTITY_SENSORS];							// �������� ������ � ������������ ������� ��� ���������� ������������ ��������
	
	for (byte Number_Input_GPIO_Port = 1; Number_Input_GPIO_Port <= 6; Number_Input_GPIO_Port ++){	
		switch(Number_Input_GPIO_Port){
 			case CONFIG_SENSOR_B_INPUT_GPIO_P1:
 				countSensors = sensors1.getDeviceCount();
				for (int i = 0; i < countSensors; i++) {
					sensors1.getAddress(sensorsUnique[i], i);
				}
 				break;
 			case CONFIG_SENSOR_B_INPUT_GPIO_P2:
 				countSensors = sensors2.getDeviceCount();
				for (int i = 0; i < countSensors; i++) {
					sensors2.getAddress(sensorsUnique[i], i);
				}
 				break;
 			case CONFIG_SENSOR_B_INPUT_GPIO_P3:
 				countSensors = sensors3.getDeviceCount();
				for (int i = 0; i < countSensors; i++) {
					sensors3.getAddress(sensorsUnique[i], i);
				}
 				break;
 			case CONFIG_SENSOR_B_INPUT_GPIO_P4:
 				countSensors = sensors4.getDeviceCount();
				for (int i = 0; i < countSensors; i++) {
					sensors4.getAddress(sensorsUnique[i], i);
				}
 				break;
 			case CONFIG_SENSOR_B_INPUT_GPIO_P5:
 				countSensors = sensors5.getDeviceCount();
				for (int i = 0; i < countSensors; i++) {
					sensors5.getAddress(sensorsUnique[i], i);
				}
 				break;
 			case CONFIG_SENSOR_B_INPUT_GPIO_P6:
 				countSensors = sensors6.getDeviceCount();
				for (int i = 0; i < countSensors; i++) {
				 sensors6.getAddress(sensorsUnique[i], i);
				}
 				break;
 		}
		if(LogsToUART == SENSORS_SEARCH_TO_UART){
			Print_Count_Sensors(countSensors, Number_Input_GPIO_Port);		// ������� ���������� �������� �������� �� �����
		}
		
		// ========================== ������� ���������� ������ ���������� ����������� ==========================
		// ������ � ���������� ��������� ������ ��� ������������ ����� SENSORS_SEARCH_ALLOW_LOG_TO_UART =========
		// ��� ����� SENSORS_SEARCH_TO_APP ��������� ������ ����� ������� ��� ���������� ���������������� =======
		for (int i = 0; i < countSensors; i++) {
			switch(LogsToUART){
				case SENSORS_SEARCH_TO_APP:
					printAddress(sensorsUnique[i], SENSORS_SEARCH_OUTPUT_HEX_FORMAT, LogsToUART);
					break;
				case SENSORS_SEARCH_TO_UART:
					Serial.print(F("\t\t\tDevice ")); Serial.print(i); Serial.println(F(":"));
					
					Serial.print(F("\t\t\t\tAddress (DEC): "));
					printAddress(sensorsUnique[i], SENSORS_SEARCH_OUTPUT_DEC_FORMAT, LogsToUART);
					
					Serial.print(F("\t\t\t\tAddress (HEX): "));
					printAddress(sensorsUnique[i], SENSORS_SEARCH_OUTPUT_HEX_FORMAT, LogsToUART);
					
					Serial.print(F("\t\t\t\tTemperature: "));
					switch(Number_Input_GPIO_Port){
						case CONFIG_SENSOR_B_INPUT_GPIO_P1:
							Serial.print(sensors1.getTempC(sensorsUnique[i])); Serial.println(F(" C"));
							break;
						case CONFIG_SENSOR_B_INPUT_GPIO_P2:
							Serial.print(sensors2.getTempC(sensorsUnique[i])); Serial.println(F(" C"));
							break;
						case CONFIG_SENSOR_B_INPUT_GPIO_P3:
							Serial.print(sensors3.getTempC(sensorsUnique[i])); Serial.println(F(" C"));
							break;
						case CONFIG_SENSOR_B_INPUT_GPIO_P4:
							Serial.print(sensors4.getTempC(sensorsUnique[i])); Serial.println(F(" C"));
							break;
						case CONFIG_SENSOR_B_INPUT_GPIO_P5:
							Serial.print(sensors5.getTempC(sensorsUnique[i])); Serial.println(F(" C"));
							break;
						case CONFIG_SENSOR_B_INPUT_GPIO_P6:
							Serial.print(sensors6.getTempC(sensorsUnique[i])); Serial.println(F(" C"));
							break;
					}
					break;
			}
		}
	}
}