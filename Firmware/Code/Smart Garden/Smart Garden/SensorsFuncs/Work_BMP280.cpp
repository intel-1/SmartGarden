
#include <Arduino.h>

#include "Work_BMP280.h"

#include "../Configuration.h"
#include "../Sensors.h"
#include "../GSM.h"

 
BMP280 bmp280[2];



void ProcessingErrors(float Value, byte SGB, byte NumberSensor){	
	String Text = "";
	boolean Send = false;
	switch(SGB){
		case 1:							// Температура
			if (Value == -139.98){
				if(EEPROM.read(E_ReactToMistakes_Ext + NumberSensor) == 1){					// Если настроено на отпраку СМС при ошибке чтения
					if(EEPROM.read(E_ErrorReadSensor_SMS + NumberSensor) != 1){				// Если не отправлялось СМС
						Send = true;
						Text = F("temperature");
						EEPROM.write(E_ErrorReadSensor_SMS + NumberSensor, 1);
					}
				}				
			}
			else{
				EEPROM.write(E_ErrorReadSensor_SMS + NumberSensor, 0);
			}
			break;
		case 5:							// Атмосферное давление
			if (Value == 905.76){
				if(EEPROM.read(E_ReactToMistakes_Ext + NumberSensor) == 1){					// Если настроено на отпраку СМС при ошибке чтения
					if(EEPROM.read(E_ErrorReadSensor_SMS + NumberSensor) != 1){				// Если не отправлялось СМС
						Send = true;
						Text = F("pressure");
						EEPROM.write(E_ErrorReadSensor_SMS + NumberSensor, 1);
					}
				}				
			}
			else{
				EEPROM.write(E_ErrorReadSensor_SMS + NumberSensor, 0);
			}
			break;
	}
	if(Send){
		SendSMS(String(F("Error of reading ")) + Text + String(F(" on sensor ")) + NameSensor[NumberSensor], 4);
	}
}


float ReadValueBMP280(byte Address, byte TypeDataSensor){
	switch (Address){
		case 0x76:
			switch(TypeDataSensor){
				case 1:
					return bmp280[0].readTemperature();
					break;
				case 5:
					return bmp280[0].readPressure() /*/ 100.0F*/ * 0.0075;		// мм рт. столба
					break;
			}
			break;
		case 0x77:
			switch(TypeDataSensor){
				case 1:
					return bmp280[1].readTemperature();
					break;
				case 5:
					return bmp280[1].readPressure() /*/ 100.0F*/ * 0.0075;		// мм рт. столба
					break;
			}
			break;
	}
}


void CalculateBMP280(byte NumberSensor, byte TypeDataSensor){
	byte AdressSensor = EEPROM.read(E_Address_Sensor + (NumberSensor * 10));
	
	ControllPort(NumberSensor, 1);								// Включаем управление Controll портом
	switch(TypeDataSensor){
		case 1:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp")); Serial.println(F("...done"));
			}			
			
			RealValueSensors[NumberSensor][VALUE_1] = ReadValueBMP280(AdressSensor, 1);
			ProcessingErrors(RealValueSensors[NumberSensor][VALUE_1], 1, NumberSensor);		// Обработка ошибок чтения показаний
			break;
		case 5:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Press")); Serial.println(F("...done"));
			}
			RealValueSensors[NumberSensor][VALUE_3] = ReadValueBMP280(AdressSensor, 5);
			ProcessingErrors(RealValueSensors[NumberSensor][VALUE_3], 5, NumberSensor);		// Обработка ошибок чтения показаний
			break;
		case 15:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement Temp")); Serial.println(F("...done"));
				Serial.print(F("\t\t\t...measurement Press")); Serial.println(F("...done"));
			}
			RealValueSensors[NumberSensor][VALUE_1] = ReadValueBMP280(AdressSensor, 1);
			RealValueSensors[NumberSensor][VALUE_3] = ReadValueBMP280(AdressSensor, 5);
			ProcessingErrors(RealValueSensors[NumberSensor][VALUE_1], 1, NumberSensor);		// Обработка ошибок чтения показаний
			ProcessingErrors(RealValueSensors[NumberSensor][VALUE_3], 5, NumberSensor);		// Обработка ошибок чтения показаний
			break;
	}
	ControllPort(NumberSensor, 0);								// Выключаем управление Controll портом
}