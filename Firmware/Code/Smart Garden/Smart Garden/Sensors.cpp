#include <Arduino.h>
#include <EEPROM.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <stdio.h>
#include "lib\SD.h"

#include "Sensors.h"
#include "EEPROM_ADR.h"
//#include "SDcard.h"
#include "Warning.h"
#include "main.h"
#include "DigitalPorts.h"
#include "RecievedUART\RegisterSensors.h"


 
float CfCalcDC = 0;										// Поправочные коэфициенты для вычисления VCC
float VCC = 0.0;										// Текущее напряжение питания
float Ti = 0;											// Температура встроенного температурного датчика (LM75A)
int RealValueADC[QUANTITY_SENSORS + 1];					// Текущие значения аналоговых портов
float RealValueSensors[QUANTITY_SENSORS + 1][3];		// Текущие значения датчиков (Для удобства счет идет с единицы, а не с нуля, для этого увеличили размер массива)
float OldValueSensors[QUANTITY_SENSORS + 1][3];			// Старые значения датчиков (нужны для запуска мониторинга групп, сравнивается с текущими и если различаются, запускается мониторинг)
byte SensorsError[QUANTITY_SENSORS + 1][3];				// Ошибки датчиков
//boolean DebugRepet_1;									// Повторять ли вывод в консоль
int LoopTimeRunCalculateSensor[QUANTITY_SENSORS + 1];	// Временные интервалы измерения сенсоров


struct StructQuantityCalcSensors QuantityCalcSensors;


byte ViewMaxLongValue(byte NameSensor, int Value){
	float ValueSensor = RealValueSensors[NameSensor][Value];
	byte NumberValue = 0;
	while(abs(ValueSensor) >= 10){			// Берем модуль Value
		ValueSensor = ValueSensor / 10;
		NumberValue ++;
	}
	NumberValue += 4;				// Добавляем к длинне два нуля после запятой, саму запятую  и служебную единицу (итого 4 символа)
	if(ValueSensor < 0.0){
		return NumberValue + 1;
	}
	else return NumberValue;
}


void React_to_Error_Calculate_Value(byte NumberSensor,byte TypeMeasurement, byte TypeValue){
	String Text = "";
	boolean Send = false;
	switch(TypeMeasurement){
		case S_DS18B20:
			break;
		case S_AM2302:
			break;
		case S_HTU21D /*|| S_SI7013 || S_SI7020 || S_SI7021 || S_SHT21*/:
			switch(TypeMeasurement){
				case TEMP_AIR_VALUE:
					SensorsError[NumberSensor][VALUE_1] = 1;					// Иначе поднимаем флаг ошибочности данных
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.println(F("...error"));
					}
					break;
				case HUMM_AIR_VALUE:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.println(F("...error"));
					}
					SensorsError[NumberSensor][VALUE_2] = 1;					// Иначе поднимаем флаг ошибочности данных
					break;
				case HUMM_AND_TEMP_VALUE:
					break;
			}
			break;	
		case S_BME280:
			switch(TypeMeasurement){
				case TEMP_AIR_VALUE:																// Температура
					if (TypeValue == -139.98){
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
				case ATMOSPHERIC_PRESSURE:							// Атмосферное давление
					if (TypeValue == 905.76){
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
				//Send_SMS(String(F("Error of reading ")) + Text + String(F(" on sensor ")) + NameSensor[NumberSensor], GSM_ERROR_SMS);
			}
			break;	
		case S_BMP280:
			break;
		case S_INA219:
			break;
		case S_ANALOG_SENSOR:
			break;
		case S_TSL2561:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("...error"));
			}
			SensorsError[NumberSensor][VALUE_2] = 1;				// Поднимаем ошибку чтения данных датчиком
			RealValueSensors[NumberSensor][VALUE_2] = 0;			// Обнуляем значение датчика в массиве
			break;
		case S_BH1750:
			SensorsError[NumberSensor][VALUE_2] = 1;						// Поднимаем ошибку чтения
			RealValueSensors[NumberSensor][VALUE_2] = 0;					// Обнуляем значение в массиве
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("...error or reading less than zero"));
			}
			break;
		case S_MAX44009:
			break;
		case S_LM75:
			break;
		case S_INA3221:
			break;	
		case S_AHT25:
			break;
	}
}


void ViewValueAllSensors(){									// Вывод в консоль измеренные значения всех датчиков
	byte maxLongName = 0;
	byte maxLongValue[3];
	byte LongLinesValue;
	byte maxLongState = 0;
	
	maxLongValue[VALUE_1] = 0;
	maxLongValue[VALUE_2] = 0;
	maxLongValue[VALUE_3] = 0;
	
	for(byte NumberSensor = 1; NumberSensor < QUANTITY_SENSORS; NumberSensor++){				// Ищем самое длинное название датчика
		for(byte Value = 0; Value < 3; Value ++){
			byte BufferLongValue = ViewMaxLongValue(NumberSensor, Value);
			if(maxLongValue[Value] < BufferLongValue){
				maxLongValue[Value] = BufferLongValue;
			}
		}
	}
	
	Serial.println(F("   Название   |  Статус  |  Измеренные показания  | Ошибки"));
	Serial.println(F("---------------------------------------------------------"));
	
	for(byte NumberSensor = 1; NumberSensor <= QUANTITY_SENSORS; NumberSensor++){
		Serial.print(NameSensor[NumberSensor]);
		byte LongLines = strlen(NameSensor[NumberSensor]);
		LongLines = LongLines - 1;

		if(LongLines == 255) LongLines = 0;
		while (LongLines <= 16){
			LongLines ++;
			Serial.print(F(" "));
		}

		Serial.print(F(" "));		
		switch(EEPROM.read(E_StatusSensor + NumberSensor)){
			case 0:
				Serial.print(F("ВЫКЛ"));
				break;
			case 1:
				Serial.print(F("ВКЛ "));
				break;
		}
		Serial.print(F("     "));		
					
		for(byte Value = 0; Value < 3; Value++){
			if(Value == 0){
				Serial.print(F("  ")); Serial.print(RealValueSensors[NumberSensor][Value]); 
			}
			else{
				LongLinesValue = ViewMaxLongValue(NumberSensor, Value - 1);
				if(LongLinesValue < maxLongValue[Value - 1]){
					byte SpaceValue = maxLongValue[Value - 1] - LongLinesValue;
					for(byte i = 1; i <= SpaceValue; i++){
						Serial.print(F(" "));
					}	
				}
				Serial.print(F("  ")); Serial.print(RealValueSensors[NumberSensor][Value]); 
			}
		}
		Serial.print(F("  "));
		LongLinesValue = ViewMaxLongValue(NumberSensor, VALUE_3);
		if(LongLinesValue < maxLongValue[VALUE_3]){
			byte SpaceValue = maxLongValue[VALUE_3] - LongLinesValue;
			for(byte i = 1; i <= SpaceValue; i++){
				Serial.print(F(" "));
			}
		}
		for(byte Value = 0; Value < 3; Value++){
			Serial.print(F(" ")); Serial.print(SensorsError[NumberSensor][Value]);
		}
		Serial.println();
	}
	Serial.println(F("========================================================="));
	Serial.println();
}


// ===========================================================================================
// ============================ Ф-ции измерения данных датчиками =============================
// ===========================================================================================
void  ReadValueAnalogPort(byte NumberSensor, byte NumberADC){
	switch(EEPROM.read(E_ConfigSensor_A + NumberSensor)){
		case 0:
			RealValueSensors[NumberSensor][0] = map(RealValueADC[NumberADC], EEPROM_int_read(E_ConfigSensor_B + NumberSensor*2), EEPROM_int_read(E_ConfigSensor_C + NumberSensor*2), 0, 100);
			break;
		case 1:
			RealValueSensors[NumberSensor][0] = RealValueADC[NumberADC];
			break;
		default:
			RealValueSensors[NumberSensor][0] = map(RealValueADC[NumberADC], EEPROM_int_read(E_ConfigSensor_B + NumberSensor*2), EEPROM_int_read(E_ConfigSensor_C + NumberSensor*2), 0, 100);
			break;
	}	
}

void ReadAnalogPort(byte NumberSensor, byte TypeDataSensor){
 	if (OUTPUT_LEVEL_UART_SENSOR){
		Serial.println();
 	}

	ControllPort(NumberSensor, 1);													// Включаем управление Controll портом		
	
	SensorsError[NumberSensor][0] = 0;										// Сбрасываем возможные ошибки чтения данных
	switch(EEPROM.read(E_Address_Sensor + (NumberSensor * 10))){	
		case PORT_INPUT_GPIO_P1:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement ANALOG_S1")); Serial.println(F("...done"));
			}
			ReadValueAnalogPort(NumberSensor, 0);
			break;
		case PORT_INPUT_GPIO_P2:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement ANALOG_S2")); Serial.println(F("...done"));
			}
			ReadValueAnalogPort(NumberSensor, 1);
			break;
		case PORT_INPUT_GPIO_P3:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement ANALOG_S3")); Serial.println(F("...done"));
			}	
			ReadValueAnalogPort(NumberSensor, 7);		
			break;
		case PORT_INPUT_GPIO_P4:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement ANALOG_S4")); Serial.println(F("...done"));
			}
			ReadValueAnalogPort(NumberSensor, 3);
			break;
		case PORT_INPUT_GPIO_P5:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t\t\t...measurement ANALOG_S5")); Serial.println(F("...done"));
			}
			ReadValueAnalogPort(NumberSensor, 4);
			break;
		default:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("\t\t\t...the port does not support analog measurements"));
			}
	}
	ControllPort(NumberSensor, 0);											// Включаем управление Controll портом	
}

void DefinitionSensor(byte NumberSensor, byte TypeDataSensor){				// Определяем наименование датчика и запускам нужные ф-ции измерения
	/*	
		NumberSensor	- порядковый номер датчика
		TypeDataSensor	- тип данных которые измерить датчик (температуры, влажности воздуха, ....)
	*/
	switch (EEPROM.read(E_Type_A_Sensor + NumberSensor)){					// Читаем регистр E_Type_A_Sensor и определяем наименование датчика
		case S_DS18B20:							/* - DS18B20 */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("DS18B20"));
			}
			CalculateDS18B20(NumberSensor);
			break;
		case S_AM2302:									/* AM2302 */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("AM2302"));
			}
			CalculateAM2302(NumberSensor, TypeDataSensor);
			break;
		case S_SI7013:									/* Si7013, Si7020, Si7021, HTU21D, SHT21 */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("HTU21D"));
			}
			CalculateHTU21D(NumberSensor, TypeDataSensor);
			break;
		case S_BME280:									/* BME280 */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("BME280"));
			}
			CalculateBME280(NumberSensor, TypeDataSensor);
			break;
		case S_BMP280:									/* BMP280 */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("BMP280"));
			}
			CalculateBMP280(NumberSensor, TypeDataSensor);
			break;
		case S_INA219:									/* INA219 */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("INA219"));
			}
			CalculateINA219(NumberSensor, TypeDataSensor);
			break;
		case S_ANALOG_SENSOR:							/* Аналоговой датчик */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("Analog Port"));
			}
			ReadAnalogPort(NumberSensor, TypeDataSensor);
			break;
		case S_TSL2561:									/* TSL2561 */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("TSL2561"));
			}
			CalculateTSL2561(NumberSensor, TypeDataSensor);
			break;
		case S_BH1750:									/* BH1750 */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("BH1750"));
			}
			CalculateBH1750(NumberSensor, TypeDataSensor);
			break;
		case S_MAX44009:								/* MAX44009 */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("MAX44009"));
			}
			CalculateMAX44009(NumberSensor, TypeDataSensor);
			break;
		case S_LM75:									/* LM75 */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("LM75"));
			}
			//CalculateLM75(NumberSensor);
			break;
		case S_INA3221:									/* INA3221 */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("INA3221"));
			}
			CalculateINA3221(NumberSensor, TypeDataSensor);
			break;
		case S_AHT25:									/* AHT25 */
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("AHT25"));
			}
			CalculateAHT25(NumberSensor, TypeDataSensor);
			break;
		default:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println();
			}
	}
}

boolean AllowCalculateSensor(byte NumberSensor){
	if(!ControllerSetup && EEPROM.read(E_AllowManualIntervalSensor + NumberSensor) == 1){		// Если контроллер в рабочем режиме и включен ручной интервал опроса датчика
		if(T_second > (LoopTimeRunCalculateSensor[NumberSensor] + EEPROM_int_read(E_SensorPollInterval + NumberSensor * 2))){  // Определяем пришло ли время его опрашивать
			LoopTimeRunCalculateSensor[NumberSensor] = T_second;								// Сохраняем время запуска опроса датчика
			return true;																		// Если время опроса пришло, то поднимаем флаг что можно опрашивать датчик
		}
		else return false;
	}
	else{																						// Если интервал опроса датчика стоит в default режиме
		return true;																			// Поднимаем флаг что можно опрашивать датчик
	}
}


void CalculateSensors(){																	// Определяем какие показания хотим измерять (байт конфигурации E_Type_B_Sensor)
	for (byte NumberSensor = 1; NumberSensor <= QUANTITY_SENSORS; NumberSensor ++){			// Проходим по всем датчикам
		wdt_reset();
		if(EEPROM.read(E_StatusSensor + NumberSensor) == 1){								// Если датчик включен																								
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.print(F("\t...Датчик ")); Serial.print(NumberSensor); Serial.println(F(":"));
				Serial.print(F("\t\t...Наименование датчика: "));
			}
			byte Type_B_Sensor = EEPROM.read(E_Type_B_Sensor + NumberSensor);
			if(AllowCalculateSensor(NumberSensor)){																
				DefinitionSensor(NumberSensor, Type_B_Sensor);								// Измеряем показания
				for(byte SGB = 0; SGB < 3; SGB ++){											// Проходим по всем байтам привязки к круппам
					byte NumberChannel = EEPROM.read(E_SBG + (NumberSensor*3) + SGB);		// Получаем номер группы к которой привязано значение датчика
					if(NumberChannel != 0){													// Группа "0" служебная, управление по ней не идет
						if(EEPROM.read(E_StatusChannel + NumberChannel) == 1){				// Если группа включена	
							if(OUTPUT_LEVEL_UART_CHANNEL){
								Serial.println(F("\t\t==========================="));
								Serial.print(F("\t\t...the channel ")); Serial.print(NumberChannel); Serial.print(F(" is connected to value ")); Serial.print(SGB+1); Serial.println(F("..."));
							}
							TermostatFunc(NumberChannel);									// Запускаем ф-цию управления группами
						}	
					}
				}
			}
		}
	}
	//StartMeasurementIndicationsDS18B20();
}


// =============================================================================================================
// =================================== Блок управления Controll портом =========================================
// =============================================================================================================
boolean UpControllPort = false;

void ControllPort(byte NumberSensor, byte Controll){							// Управление Controll портом
	/*
		TypeControll:	0 - выключить
						1 - включить
	*/
	if(EEPROM.read(E_AllowSensorControllPort + NumberSensor) == 1){
		byte NumberPort = EEPROM.read(E_SensorControllPort + NumberSensor);				// Узнаем номер порта
		if (OUTPUT_LEVEL_UART_SENSOR){
			switch(Controll){
				case 0:
					Serial.print(F("\t\t\t...turn off the control port..."));
					break;
				case 1:
					Serial.print(F("\t\t\t...turn on the control port..."));
					break;
			}
		}
	
		if(DigitalPort(NumberPort, Controll, DIGITAL_PORT_RETURN_NAME_PORT, NO_LOG_TO_UART) != 255){	// Если порт правильно сконфигурирован
			DigitalPort(NumberPort, Controll, DIGITAL_PORT_SWITCH_PORT, NO_LOG_TO_UART);				// Включаем\выключаем порт
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("...done"));
			}
			switch(Controll){
				case 1:
					UpControllPort = true;												// Поднимаем флаг что включен управляющий порт
					break;
				case 0:
					UpControllPort = false;												// Поднимаем флаг что включен управляющий порт
					break;
			}
			if(Controll == 1){															// Включаем задержку только после включения порта
				delay(EEPROM_int_read(E_DelayToRunControllPort + NumberSensor * 2));	// Задержка чтобы успело стабилизоваться питание на Controll порту
			}
		}
		else{
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("isn't configured"));
			}
		}
	}
}
// =======================================================================================================
	

void i2c_scaner(boolean LogView){
	byte error;
	byte address;
	int nDevices;
	Serial.println(F("\tI2C sensors: "));
	nDevices = 0;
	for (address = 1; address < 127; address++ ){
		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		if (error == 0){
			String stringOne =  String(address, HEX);
			Serial.print(F("\t\t0x")); Serial.print(stringOne); Serial.print(F(" - "));
			
			if(stringOne == "0a") Serial.println(F("'Motor Driver'"));
			if(stringOne == "0f") Serial.println(F("'Motor Driver'"));
			if(stringOne == "1d") Serial.println(F("'ADXL345 Input 3-Axis Digital Accelerometer'"));
			if(stringOne == "1e") Serial.println(F("'HMC5883 3-Axis Digital Compass'"));
			if(stringOne == "3f") Serial.println(F("'LCD module'"));
			if(stringOne == "5a") Serial.println(F("'Touch Sensor'"));
			if(stringOne == "5b") Serial.println(F("'Touch Sensor'"));
			if(stringOne == "5C") Serial.println(F("'BH1750FVI Light Sensor' OR 'Touch Sensor"  ));
			if(stringOne == "5d") Serial.println(F("'Touch Sensor'"));
			if(stringOne == "20") Serial.println(F("'PCF8574 8-Bit I/O Expander' OR 'LCD module' "));
			if(stringOne == "21") Serial.println(F("'PCF8574 8-Bit I/O Expander'"));
			if(stringOne == "22") Serial.println(F("'PCF8574 8-Bit I/O Expander'"));
			if(stringOne == "23") Serial.println(F("'PCF8574 8-Bit I/O Expander' OR 'BH1750FVI Light Sensor'"));
			if(stringOne == "24") Serial.println(F("'PCF8574 8-Bit I/O Expander'"));
			if(stringOne == "25") Serial.println(F("'PCF8574 8-Bit I/O Expander'"));
			if(stringOne == "26") Serial.println(F("'PCF8574 8-Bit I/O Expander'"));
			if(stringOne == "27") Serial.println(F("'PCF8574 8-Bit I/O Expander' OR 'LCD module'"));
			if(stringOne == "29") Serial.println(F("'TSL2561 Light Sensor'"));
			if(stringOne == "39") Serial.println(F("'TSL2561 Light Sensor'"));
			if(stringOne == "40") Serial.println(F("'BMP180, Si7013, Si7020, Si7021, HTU21D, SHT21, INA219'" ));
			if(stringOne == "41") Serial.println(F("'INA219'" ));
			if(stringOne == "44") Serial.println(F("'INA219'" ));
			if(stringOne == "45") Serial.println(F("'INA219'" ));
			if(stringOne == "48") Serial.println(F("'ADS1115 Module 16-Bit' OR 'LM75'"));
			if(stringOne == "49") Serial.println(F("'ADS1115 Module 16-Bit' OR 'SPI-to-UART' OR 'TSL2561 Light Sensor'"));
			if(stringOne == "4a") Serial.println(F("'ADS1115 Module 16-Bit' OR 'MAX44009'"));
			if(stringOne == "4b") Serial.println(F("'ADS1115 Module 16-Bit' OR 'MAX44009'"));
			if(stringOne == "4c") Serial.println(F("'LM75'"));
			if(stringOne == "50") Serial.println(F("'AT24C32 EEPROM'"));
			if(stringOne == "53") Serial.println(F("'ADXL345 Input 3-Axis Digital Accelerometer'"));
			if(stringOne == "68") Serial.println(F("'DS3231 real-time clock' OR 'MPU-9250'"));
			if(stringOne == "7a") Serial.println(F("'LCD OLED 128x64'"));
			if(stringOne == "76") Serial.println(F("'BMP180, BMP280, BME280 barometric pressure sensor'"));
			if(stringOne == "77") Serial.println(F("'BMP180, BMP280, BME280 barometric pressure sensor'"));
			if(stringOne == "78") Serial.println(F("'LCD OLED 128x64'" ));
			
			nDevices++;
		}
		else if (error == 4){
			Serial.print(F("\tUnknown error at address 0x"));
			if (address < 16){
				Serial.print(F("0"));
			}
			Serial.println(address,HEX);
		}
	}
	if (nDevices == 0){
		Serial.println(F("\tNo I2C devices found\n"));
	}
}