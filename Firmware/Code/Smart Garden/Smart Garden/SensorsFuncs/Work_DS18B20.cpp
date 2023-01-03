

#include <Arduino.h>

#include "Work_DS18B20.h"

#include "../Configuration.h"
#include "../Sensors.h"
#include "../GSM.h"
#include "../ConfigSensors.h"



OneWire ds1(INPUT_GPIO_P1);
OneWire ds2(INPUT_GPIO_P2);
OneWire ds3(INPUT_GPIO_P3);
OneWire ds4(INPUT_GPIO_P4);
OneWire ds5(INPUT_GPIO_P5);
OneWire ds6(INPUT_GPIO_P6);
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


void StartMeasurementIndicationsDS18B20(){											// �-��� ������� ��������� ���������� ���������		
	for(byte NumberGPIO = 1; NumberGPIO <= QuantityInputGPIO; NumberGPIO++){		// ���������� ������� �� ��������� ����������� ������ �������������������� ��������
		switch(Init_DS18B20[NumberGPIO]){
			case 1:
				sensors1.requestTemperatures();
				break;
			case 2:
				sensors2.requestTemperatures();
				break;
			case 3:
				sensors3.requestTemperatures();
				break;
			case 4:
				sensors4.requestTemperatures();
				break;
			case 5:
				sensors5.requestTemperatures();
				break;
			case 6:
				sensors6.requestTemperatures();
				break;
		}
	}
	_delay_ms(500);																	// �������� ��� ���������
}


void CalculateDS18B20(byte NumberSensor){
	float RealValue;
	DeviceAddress AddresSensor = {	EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 0),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 1),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 2),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 3),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 4),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 5),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 6),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 7)};
		
	ControllPort(NumberSensor, 1);													// �������� ���������� Controll ������
	
	switch(EEPROM_int_read(E_ConfigSensor_B + NumberSensor*2)){
		case 1:
			RealValue = sensors1.getTempC(AddresSensor);							// �������� ���������� ��������
			break;
		case 2:
			RealValue = sensors2.getTempC(AddresSensor);
			break;
		case 3:
			RealValue = sensors3.getTempC(AddresSensor);
			break;
		case 4:
			RealValue = sensors4.getTempC(AddresSensor);
			break;
		case 5:
			RealValue = sensors5.getTempC(AddresSensor);
			break;
		case 6:
			RealValue = sensors6.getTempC(AddresSensor);
			break;
	}
	
	if (OUTPUT_LEVEL_UART_SENSOR){
		Serial.print(F("\t\t\t...measurement Temp"));
	}
	
	switch((int)RealValue){																// ����������� ��������� � int ��� ����������� ���������� ��������
		case -127:																		// ������ �� ��������
			SensorsError[NumberSensor][VALUE_1] = 1;									// ������ ������ ������ ��������
			EEPROM.put(E_QuantityErrors + NumberSensor, EEPROM_int_read(E_QuantityErrors + NumberSensor*2) + 1);	// ����������� ������� ���������� ������
			ViewError();
			if(EEPROM.read(E_ReactToMistakes_Ext + NumberSensor) == 1){					// ���� ��������� �� ������� ��� ��� ������ ������
				if(EEPROM.read(E_SensorOff_SMS + NumberSensor) != 1){					// ���� �� ������������ ���
					
// 					MessageGSM.Arg_1_a = 1;
// 					MessageGSM.Arg_1_b = 2;
// 					MessageGSM.Arg_2_a = 2;
// 					MessageGSM.Arg_2_b = NumberSensor;
// 					MessageGSM.Arg_3_a = 1;
// 					MessageGSM.Arg_3_b = 3;
// 					MessageGSM.PhoneNumber = 0;
// 					WriteToQueueGSM(MessageGSM);
					
					//Send_SMS(String(F("Sensor ")) + NameSensor[NumberSensor] + (F(" is off")), GSM_ERROR_SMS);  
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


#define TEMPERATURE_PRECISION 12			// �������� ��������� � ����� (�� ��������� 12)

DeviceAddress deviceAddresses[16];			// ������� ������ ��� �������� ������� ��������


void printAddress(DeviceAddress deviceAddress, byte Notation, bool ViewLogs){			// ������� ������ �������������� �������
	/*
		ViewLogs:	0 - ����� ������ ��� ���������� ��� ��������� 
					1 - ����� ������ ��� UART 
					
		Notation -	� ����� ������� �������� �����:
													0 - HEX
													1 - DEC
	*/
	for (uint8_t i = 0; i < 8; i++)  {
		if (deviceAddress[i] < 16) Serial.print(F("0"));
		switch(Notation){
			case 0:
				Serial.print(deviceAddress[i], HEX);
				break;
			case 1:
				Serial.print(deviceAddress[i], DEC);
				break;
		}
		switch(ViewLogs){
			case 0:
				if(i != 7) Serial.print(F(" "));
				break;
			case 1:
				if(i != 7) Serial.print(F(":"));
				break;
		}
	}
}

void printTemperature(DeviceAddress deviceAddress, byte Number_Input_GPIO_Port){		// ������� ������ ����������� ������� �� ��� ��������������
	float tempC;
	switch(Number_Input_GPIO_Port){
		case 1:
			tempC = sensors1.getTempC(deviceAddress);
			break;
		case 2:
			tempC = sensors2.getTempC(deviceAddress);
			break;
		case 3:
			tempC = sensors3.getTempC(deviceAddress);
			break;
		case 4:
			tempC = sensors4.getTempC(deviceAddress);
			break;
		case 5:
			tempC = sensors5.getTempC(deviceAddress);
			break;
		case 6:
			tempC = sensors6.getTempC(deviceAddress);
			break;
	}
	Serial.print(tempC);
	Serial.print(F(" C"));
}


void DS18B20_scaner(byte ViewLogs){
	/*
		ViewLogs:	0 - ����� ������ ��� ��������
					1 - ����� ������ � ��������� 
	*/
	
	#define HEX 0
	#define DEC 1
	
	Serial.println(F("\tDS18B20 sensors:"));
	
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
	
	delay(1500);
	
	for (byte Number_Input_GPIO_Port = 1; Number_Input_GPIO_Port <= 6; Number_Input_GPIO_Port ++){		
		Serial.print("Number_Input_GPIO_Port: "); Serial.println(Number_Input_GPIO_Port);
		byte deviceCount = 0;								// ���������� ��� �������� ���������� ��������
		
		// =========================== �������� ���������� �������� �������� �� ���� ===========================
		switch(Number_Input_GPIO_Port){
			case 1:
				deviceCount = sensors1.getDeviceCount();
				break;
			case 2:
				deviceCount = sensors2.getDeviceCount();
				break;
			case 3:
				deviceCount = sensors3.getDeviceCount();
				break;
			case 4:
				deviceCount = sensors4.getDeviceCount();
				break;
			case 5:
				deviceCount = sensors5.getDeviceCount();
				break;
			case 6:
				deviceCount = sensors6.getDeviceCount();
				break;
		}
		
		
		if(deviceCount > 0){														// ���� ����� ������� �� �����
			Serial.println("\t\t" + String(deviceCount) + " sensors were found on the port input GPIO.P" + String(Number_Input_GPIO_Port));		// ���������� ���������� ��������� ��������
			
			// =========================== �������� ����� ������� �������� - ���������� ��� ������� ===========================
			Serial.print(F("\t\tParasite power is: "));									
			switch(Number_Input_GPIO_Port){ 
				case 1:
					Serial.println("\t\t" + sensors1.isParasitePowerMode() ? "ON" : "OFF");
					break;
				case 2:
					Serial.println("\t\t" + sensors2.isParasitePowerMode() ? "ON" : "OFF");
					break;
				case 3:
					Serial.println("\t\t" + sensors3.isParasitePowerMode() ? "ON" : "OFF");
					break;
				case 4:
					Serial.println("\t\t" + sensors4.isParasitePowerMode() ? "ON" : "OFF");
					break;
				case 5:
					Serial.println("\t\t" + sensors5.isParasitePowerMode() ? "ON" : "OFF");
					break;
				case 6:
					Serial.println("\t\t" + sensors6.isParasitePowerMode() ? "ON" : "OFF");
					break;
			}
		
			for (int i = 0; i < deviceCount; i++) {											// ��������� ������ �������� ���������
				bool Device_found = false;
				switch(Number_Input_GPIO_Port){
					case 1:
						if (!sensors1.getAddress(deviceAddresses[i], i)) {												// ���� �� ������� �������� �������������
							Serial.println(F("\t\tUnable to find address for Device ")); Serial.println(String(i));		// ������ ��������� �� ����
						}
						else Device_found = true;
						break;
					case 2:
						if (!sensors2.getAddress(deviceAddresses[i], i)) {												// ���� �� ������� �������� �������������
							Serial.println(F("\t\tUnable to find address for Device ")); Serial.println(String(i));		// ������ ��������� �� ����
						}
						else Device_found = true;
						break;
					case 3:
						if (!sensors3.getAddress(deviceAddresses[i], i)) {												// ���� �� ������� �������� �������������
							Serial.println(F("\t\tUnable to find address for Device ")); Serial.println(String(i));		// ������ ��������� �� ����
						}
						else Device_found = true;
						break;
					case 4:
						if (!sensors4.getAddress(deviceAddresses[i], i)) {												// ���� �� ������� �������� �������������
							Serial.println(F("\t\tUnable to find address for Device ")); Serial.println(String(i));		// ������ ��������� �� ����
						}
						else Device_found = true;
						break;
					case 5:
						if (!sensors5.getAddress(deviceAddresses[i], i)) {												// ���� �� ������� �������� �������������
							Serial.println(F("\t\tUnable to find address for Device ")); Serial.println(String(i));		// ������ ��������� �� ����
						}
						else Device_found = true;
						break;
					case 6:
						if (!sensors6.getAddress(deviceAddresses[i], i)) {												// ���� �� ������� �������� �������������
							Serial.println(F("\t\tUnable to find address for Device ")); Serial.println(String(i));		// ������ ��������� �� ����
						}
						else Device_found = true;
						break;
				}

				if(Device_found){
   					Serial.println("\t\tSensor " + String(i + 1) + ": ");
					Serial.print(F("\t\t\tAddress (HEX): "));
					printAddress(deviceAddresses[i], HEX, 1);					// ������� ����� ������� ���������� � HEX
					Serial.println();
					
					Serial.print(F("\t\t\tAddress (DEC): "));
					printAddress(deviceAddresses[i], DEC, 1);					// ������� ����� ������� ���������� � DEC
					Serial.println();			
											
					Serial.print(F("\t\t\tTemperature: "));
					printTemperature(deviceAddresses[i], Number_Input_GPIO_Port);
					Serial.println();
				}
			}
		}
	}
}