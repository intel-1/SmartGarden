

#include <Arduino.h>

#include "Work_DS18B20.h"

#include "../Configuration.h"
#include "../Sensors.h"
#include "../GSM.h"
#include "../ConfigSensors.h"



OneWire ds1(INPUT_A_S1);			
OneWire ds2(INPUT_A_S2);
OneWire ds3(INPUT_A_S3);
OneWire ds4(INPUT_A_S4);
OneWire ds5(INPUT_A_S5);
OneWire ds6(INPUT_A_S6);
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


void StartMeasurementIndicationsDS18B20(){											// Ф-ция комманд измерения температур датчиками		
	for(byte NumberGPIO = 1; NumberGPIO <= QuantityInputGPIO; NumberGPIO++){		// Отправляем команду на измерение температуры только проинизиализированым датчикам
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
	delay(500);																		// Задержка для измерений
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
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 7) };
	sensors1.setResolution(AddresSensor, 9);
	
	ControllPort(NumberSensor, 1);								// Включаем управление Controll портом
	
	switch(EEPROM.read(E_ConfigSensor_B + NumberSensor)){
		case 1:
			RealValue = sensors1.getTempC(AddresSensor);									// Получаем измеренное значение
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

	switch((int)RealValue){																// Преобразуем показание в int для определения валидности значения
		case -127:																		// Датчик не доступен
			SensorsError[NumberSensor][VALUE_1] = 1;									// Ошибка чтения данных датчиком
			EEPROM_int_write(E_QuantityErrors + NumberSensor, EEPROM_int_read(E_QuantityErrors + NumberSensor*2) + 1);	// Увеличиваем счетчик количества ошибок
			ViewError();
			if(EEPROM.read(E_ReactToMistakes_Ext + NumberSensor) == 1){					// Если настроено на отпраку СМС при ошибке чтения
				if(EEPROM.read(E_SensorOff_SMS + NumberSensor) != 1){					// Если не отправлялось СМС
					
					MessageGSM.Arg_1_a = 1;
					MessageGSM.Arg_1_b = 2;
					MessageGSM.Arg_2_a = 2;
					MessageGSM.Arg_2_b = NumberSensor;
					MessageGSM.Arg_3_a = 1;
					MessageGSM.Arg_3_b = 3;
					MessageGSM.PhoneNumber = 0;
					WriteToQueueGSM(MessageGSM);
					
					SendSMS(String(F("Sensor ")) + NameSensor[NumberSensor] + (F(" is off")), 4);  
					EEPROM.write(E_SensorOff_SMS + NumberSensor, 1);	
				}
			}
			EEPROM.write(E_ReadSensorOK_SMS + NumberSensor, 0);
			break;
		case 85:																			// Ошибка чтения данных
			EEPROM_int_write(E_QuantityErrors + NumberSensor, EEPROM_int_read(E_QuantityErrors + NumberSensor*2) + 1);	// Увеличиваем счетчик количества ошибок
			SensorsError[NumberSensor][VALUE_1] = 1;										// Ошибка чтения данных датчиком
			ViewError();
			if(EEPROM.read(E_ReactToMistakes_Ext + NumberSensor) == 1){						// Если настроено на отправку СМС при ошибке чтения
				if(EEPROM.read(E_ErrorReadSensor_SMS + NumberSensor) != 1){					// Если не отправлялось СМС
					EEPROM.write(E_ErrorReadSensor_SMS + NumberSensor, 1);	 
				}
			}
			EEPROM.write(E_ReadSensorOK_SMS + NumberSensor, 0);
			break;
		default:																			// Все хорошо, показания датчика валидны
			QuantityCalcSensors.SumValue[NumberSensor][VALUE_1] += RealValue;
			SensorsError[NumberSensor][VALUE_1] = 0;										// Снимаем ошибки чтения датчиком тем самым помечаем что данные валидны
			
			if(EEPROM.read(E_ReactToMistakes_Ext + NumberSensor) == 1){						// Если настроено на отправку СМС при ошибках датчиков
				if(EEPROM.read(E_ReadSensorOK_SMS + NumberSensor) == 0){					// Если не отправлялось СМС
					EEPROM.write(E_ReadSensorOK_SMS + NumberSensor, 1);
				}
			}
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("...done"));
			}
			break;
	}
	ControllPort(NumberSensor, 0);								// Выключаем управление Controll портом
}



void DS18B20_scaner(byte ViewLogs){
	/*
		ViewLogs:	0 - Вывод данных без описаний
					1 - вывод данных с описанием 
	*/
	byte data[12];
	byte addr[8];
	int Temp;
	if(ViewLogs == ON){
		Serial.println(F("\tDS18B20 sensors:"));
	}/*
	if(ViewLogs == ON){
		Serial.print(F("f "));
	}*/
	switch (ViewLogs){
		case ON:
			for (byte NumberDS = 0; NumberDS <= 16; NumberDS ++){
				if (!ds1.search(addr)){
					ds1.reset_search();
					return;
				}
				Serial.print(F("\tAddress(HEX) = "));
				for (byte Number = 0; Number < 8; Number++){
					Serial.print(addr[Number], HEX);
					Serial.print(F(" "));
				}
				Serial.println();
				Serial.print(F("\tAddress(DEC) = "));
				for( byte Number = 0; Number < 8; Number ++) {
					Serial.print(addr[Number]);
					Serial.print(F(" "));
				}
				Serial.println();
	
				Temp=(data[1]<<8)+data[0];
	
				Temp=Temp;
				Serial.print(F("\t\tT="));
				Serial.print(Temp/16);
				Serial.print(F("."));
				Serial.print(((Temp%16)*100)/16);
				Serial.print(F("  "));
				Serial.print(F(" CRC="));
				Serial.print( OneWire::crc8( data, 8), HEX);
				Serial.println();
			}
			break;
		case OFF:									// Вывод данных в UART без описаний
			for (byte NumberDS = 0; NumberDS <= 16; NumberDS ++){
				if (!ds1.search(addr)){
					ds1.reset_search();
					return;
				}
				for( byte Number = 0; Number < 8; Number ++) {
					Serial.print(addr[Number]);
					Serial.print(F(" "));
				}
			}
			break;
	}
}