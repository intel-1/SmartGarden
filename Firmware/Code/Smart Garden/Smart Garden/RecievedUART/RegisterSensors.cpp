#include <./Arduino.h>
#include <./EEPROM.h>

#include "RecievedConfigController.h"
#include "../ParsingDataOnSerial.h"
#include "../EEPROM_ADR.h"
#include "../DigitalPorts.h"



// ==============================================================================
// ============ Регистры, названия датчиков и исполнительных модулей ============
// ==============================================================================
void Read_ETAS_register(byte Sensor){				// Чтение регистра ETAS (Type A Sensor)
	boolean I2C_Sensor = false;
	Serial.print(F("Наименование: "/*"Sensor: "*/));
	switch(EEPROM.read(E_Type_A_Sensor + Sensor)){
		case S_DS18B20:
			Serial.println(F("DS18B20"));
			Serial.print(F("Адрес: "/*"Address: "*/));
			for(byte i = 1; i <= 8; i++){
				Serial.print(EEPROM.read(E_Address_Sensor + (10 * Sensor) + (i - 1)), HEX);
				if(i < 8){
					Serial.print(F(":"));
				}
			}
			Serial.println();
			break;
		case S_AM2302:
			Serial.println(F("AM2320"));
			I2C_Sensor = true;
			break;
		case S_HTU21D:
			Serial.println(F("Si7021, HTU21D"));
			I2C_Sensor = true;
			break;
		case S_BME280:
			Serial.println(F("BME280"));
			I2C_Sensor = true;
			break;
		case S_BMP280:
			Serial.println(F("BMP280"));
			I2C_Sensor = true;
			break;
		case S_INA219:
			Serial.println(F("INA219"));
			I2C_Sensor = true;
			break;
		case S_ANALOG_SENSOR:
			Serial.println(F("Analog Sensor"));
			Serial.print(F("Порт: "));
			DigitalPort(EEPROM.read(E_Address_Sensor + (Sensor * 10)), DIGITAL_PORT_OFF, DIGITAL_PORT_RETURN_NAME_PORT, NO_LOG_TO_UART);
			Serial.println(F(""));
			break;
		case S_TSL2561:
			Serial.println(F("TSL2561"));
			I2C_Sensor = true;
			break;
		case S_BH1750:
			Serial.println(F("BH1750"));
			I2C_Sensor = true;
			break;
		case S_MAX44009:
			Serial.println(F("MAX44009"));
			I2C_Sensor = true;
			break;
		case S_LM75:
			Serial.println(F("LM75"));
			I2C_Sensor = true;
			break;
		case S_INA3221:
			Serial.println(F("INA3221"));
			I2C_Sensor = true;
			break;
		case S_AHT25:
			Serial.println(F("AHT25"));
			I2C_Sensor = true;
			break;
		default:
			Serial.println();
			break;
	}
	if(I2C_Sensor){
		Serial.print(F("Адрес: "/*"Address: "*/)); Serial.print(F("0x")); Serial.print(EEPROM.read(E_Address_Sensor + (Sensor * 10)), HEX);
		Serial.print(F(" (DEC: ")); Serial.print(EEPROM.read(E_Address_Sensor + (Sensor * 10)), DEC); Serial.println(F(")"));
	}
}


void Read_E_Type_B_Sensor_register(byte Sensor, byte ViewText){
	//ViewText = 1;
	Serial.print(F("Тип измерения: "/*"Type: "*/));
	switch(EEPROM.read(E_Type_B_Sensor + Sensor)){
		case TEMP_AIR_VALUE: 			// Измерение температуры
			switch(ViewText){
				case 1:
					Serial.println(F("Температура"/*"Temp"*/));
					break;
				case 2:
					Serial.println(F("Температуры"/*"Temp"*/));
					break;
				default:
					Serial.println();
			}
			break;
		case HUMM_AIR_VALUE: 			// Измерение влажности воздуха
			switch(ViewText){
				case 1:
					Serial.println(F("Влажность воздуха"/*"Humidity Air"*/));
					break;
				case 2:
					Serial.println(F("Влажности воздуха"/*"Humidity Air"*/));
					break;
				default:
					Serial.println();
			}
			break;
		case HUMM_SOIL: 			// Измерение влажности почвы
			switch(ViewText){
				case 1:
					Serial.println(F("Влажность почвы"/*"Humidity Soil"*/));
					break;
				case 2:
					Serial.println(F("влажности почвы"/*"Humidity Soil"*/));
					break;
				default:
					Serial.println();
			}
			break;
		case LIGHT: 			// Измерение интенсивноски солнечного света
			switch(ViewText){
				case 1:
					Serial.println(F("Освещение"/*"Light Sensor"*/));
					break;
				case 2:
					Serial.println(F("Освещения"/*"Light Sensor"*/));
					break;
				default:
					Serial.println();
			}
			break;
		case ATMOSPHERIC_PRESSURE: 			// Измерение атмосферного давления
			switch(ViewText){
				case 1:
					Serial.println(F("Атмосферное давление"/*"Atmospheric Pressure"*/));
					break;
				case 2:
					Serial.println(F("Атмосферного давления"/*"Atmospheric Pressure"*/));
					break;
				default:
					Serial.println();
			}
			break;
		case TENSION: 			// Измерение напряжения солнечной панели
			switch(ViewText){
				case 1:
					Serial.println(F("Напряжение"/*"External Tension"*/));
					break;
				case 2:
					Serial.println(F("Напряжения"/*"External Tension"*/));
					break;
				default:
					Serial.println();
			}
			break;
		case CURRENT: 			// Измерение тока
			switch(ViewText){
				case 1:
					Serial.println(F("Ток"/*"External Current"*/));
					break;
				case 2:
					Serial.println(F("Тока"/*"External Current"*/));
					break;
				default:
					Serial.println();
			}
			break;
		case ANALOG_VALUE:				// Аналоговое значение 
			switch(ViewText){
				case 1:
					Serial.println(F("Аналоговые измерения"/*"Analog Value"*/));
					break;
				case 2:
					Serial.println(F("Аналогово измерения"/*"Analog Value"*/));
					break;
				default:
					Serial.println();
			}
			break;
		case HUMM_AND_TEMP_VALUE: 		// Multi Sensor - температура и влажность воздуха
			Serial.println(F("Температура и влажность воздуха"/*"Multi(Temp and Humm air)"*/));
			break;
		case TEMP_AIR_AND_PRESS: 		// Multi Sensor - температура и атмосферное давление
			Serial.println(F("Температура и атм. давление"/*"Multi(Temp and Atmospheric Pressure)"*/));
			break;
		case 25: 			// Multi Sensor - атм. давление и влажность воздуха
			Serial.println(F("Атм. давление и влажность воздуха"/*"Multi(Press and Humm Air)"*/));
			break;
		case TEMP_PRESS_AND_HUMM_AIR: 	// Multi Sensor - температура, влажность и атмосферное давление
			Serial.println(F("Температура, влажность воздухв и атм. давление"/*"Multi(Temp, Press and Humm Air)"*/));
			break;
		case TENS_And_CURR: 			// Multi Sensor - напряжение и ток солнечной панели
			Serial.println(F("Напряжение и ток"/*"Multi(Ext Tens and Curr)"*/));
			break;
		default:
			Serial.println();
			break;
	}
}