
#include "Sensors.h"



boolean Init_DS18B20[7];				// Номер Wire шины которая уже проинициализирована
boolean Init_HTU21D = false;
boolean Init_MAX44009[2];
boolean Init_bme280[2];
boolean Init_bmp280[2];

byte bme280_sensors[2];					// Адреса bme280 с привязкой к номеру датчика (доступные адреса 0x76 и 0x77)




// ==========================================================================================================================================
boolean InitializingDS18B20(byte AdressSensor, byte NumberSensor){
	Serial.print(F("\tInitializing DS18B20..."));	Serial.println(F("done"));
	
	switch(EEPROM.read(E_ConfigSensor_B + NumberSensor)){
		case 1:
			if(!Init_DS18B20[1]){
				pinMode(INPUT_A_S1, INPUT);
				//sensors1.begin();
				Init_DS18B20[1] = true;
			}
			break;
		case 2:
			if(!Init_DS18B20[2]){
				pinMode(INPUT_A_S2, INPUT);
				//sensors2.begin();
				Init_DS18B20[2] = true;
			}
			break;
		case 3:
			if(!Init_DS18B20[3]){
				pinMode(INPUT_A_S3, INPUT);
				//sensors3.begin();
				Init_DS18B20[3] = true;
			}
			break;
		case 4:
			if(!Init_DS18B20[4]){
				pinMode(INPUT_A_S4, INPUT);
				//sensors4.begin();
				Init_DS18B20[4] = true;
			}
			break;
		case 5:
			if(!Init_DS18B20[5]){
				pinMode(INPUT_A_S5, INPUT);
				//sensors5.begin();
				Init_DS18B20[5] = true;
			}
			break;
		case 6:
			if(!Init_DS18B20[6]){
				pinMode(INPUT_A_S6, INPUT);
				//sensors6.begin();
				Init_DS18B20[6] = true;
			}
			break;
		default:
			if (OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("...port is not configured"));
			}
	}
	StartMeasurementIndicationsDS18B20();				// Сразу отдаем датчикам комманду измерять температуру 
}


// ==========================================================================================================================================
boolean InitAM2302(byte AdressSensor, byte NumberSensor){
	
}


// ==========================================================================================================================================
boolean InitializingBME280(byte AdressSensor, byte NumberSensor){
	byte InitNumber;
	switch(AdressSensor){
		case 0x76:
			Serial.print(F("\tInitializing BME280 0x76..."));
			InitNumber = 0;
			break;
		case 0x77:
			Serial.print(F("\tInitializing BME280 0x77..."));
			InitNumber = 1;
			break;
	}
	if(!Init_bme280[InitNumber]){					// Если не инициализировался
		if(bme280[InitNumber].begin(AdressSensor)){			
			bme280[InitNumber].setSampling(	BME280::MODE_NORMAL,     /* Режим работы: 	SLEEP  – Режим сна.
																						FORCED – Дали команду на измерение, подождали, считали значения и датчик перешел в спящий режим.
																						NORMAL – Загоняет датчик в циклическую работу. Он самостоятельно через определенное время, выходит из режима сна, выполняет измерения и сохраняет данные в регистры памяти и вновь засыпает на указанное время.*/
											BME280::SAMPLING_X1,     /* Точность измерения температуры (1, 2, 4, 8, 16 бит) */
											BME280::SAMPLING_X16,	 /* Точность измерения влажности воздуха (1, 2, 4, 8, 16 бит) */
											BME280::SAMPLING_X1,     /* Точность измерения давления (1, 2, 4, 8, 16 бит) */
											BME280::FILTER_OFF,      /* Коэффициент фильтрации (OFF, 2, 4, 8, 16) */
											BME280::STANDBY_MS_500); /* Время ожидания датчика между измерениями (1, 63, 125, 250, 500, 1000, 2000, 4000 мс) */
			Serial.println(F("done"));
		}
		else Serial.println(F("error"));
		Init_bme280[InitNumber] = true;				// Поднимаем флаг чтобы не было повторной инициализации у другого такого же датчика
	}
}


// ==========================================================================================================================================
boolean InitializingHTU21D(byte AdressSensor, byte NumberSensor){
	if(!Init_HTU21D){												// Если датчик еще не инициализировался
		Serial.print(F("\tInitializing HTU21D..."));
		if (!HTU21.begin()){
			Serial.println(F("sensor is failed or not connected"));
		}
		else Serial.println(F("done"));
		Init_HTU21D = true;
	}
}


// ==========================================================================================================================================
boolean InitializingBMP280(byte AdressSensor, byte NumberSensor){
	byte InitNumber;
	switch(AdressSensor){
		case 0x76:
			Serial.print(F("\tInitializing BMP280 0x76..."));
			InitNumber = 0;
			break;
		case 0x77:
			Serial.print(F("\tInitializing BMP280 0x77..."));
			InitNumber = 1;
			break;
	}
			
	if(!Init_bmp280[InitNumber]){					// Если не инициализировался
		if(bmp280[InitNumber].begin(AdressSensor)){			
			bmp280[InitNumber].setSampling(	BMP280::MODE_NORMAL,     /* Режим работы: 	SLEEP  – Режим сна.
																						FORCED – Дали команду на измерение, подождали, считали значения и датчик перешел в спящий режим.
																						NORMAL – Загоняет датчик в циклическую работу. Он самостоятельно через определенное время, выходит из режима сна, выполняет измерения и сохраняет данные в регистры памяти и вновь засыпает на указанное время.*/
											BMP280::SAMPLING_X1,     /* Точность измерения температуры (1, 2, 4, 8, 16 бит) */
											BMP280::SAMPLING_X1,     /* Точность измерения давления (1, 2, 4, 8, 16 бит) */
											BMP280::FILTER_OFF,      /* Коэффициент фильтрации (OFF, 2, 4, 8, 16) */
											BMP280::STANDBY_MS_500); /* Время ожидания датчика между измерениями (1, 63, 125, 250, 500, 1000, 2000, 4000 мс) */
			Serial.println(F("done"));
		}
		else Serial.println(F("error"));
		Init_bmp280[InitNumber] = true;				// Поднимаем флаг чтобы не было повторной инициализации у другого такого же датчика
	}
}


// ==========================================================================================================================================
boolean InitializingINA219(byte AdressSensor, byte NumberSensor){
	Serial.print(F("\tInitializing INA219..."));
	ina219.begin(0x40);
	Serial.println(F("done"));
}


// ==========================================================================================================================================
boolean InitializingTSL2561(byte AdressSensor, byte NumberSensor){
	switch(AdressSensor){
		case 0x39:
			Serial.print(F("\tInitializing TSL2561 0x39..."));
			#define tsl tsl1
			break;
		case 0x29:
			Serial.print(F("\tInitializing TSL2561 0x29..."));
			#define tsl tsl2
			break;
		case 0x49:
			Serial.print(F("\tInitializing TSL2561 0x49..."));
			#define tsl tsl3
			break;
		default:
			Serial.println(F("\tError address"));
	}
	
	if(tsl.begin()){
		switch(EEPROM_int_read(E_ConfigSensor_C + NumberSensor)){	// Настройка усиления
			case 1:													// Тусклый свет
				tsl.setGain(TSL2561_GAIN_16X);						// set 16x gain (for dim situations)
				break;
			case 2:													// Яркий свет
				tsl.setGain(TSL2561_GAIN_1X);						// set no gain (for bright situtations)
				break;
			case 3:							
				tsl.enableAutoRange(true);
				break;
			default:												
				tsl.enableAutoRange(true);					
				
		}
		switch(EEPROM_int_read(E_ConfigSensor_D + NumberSensor)){			// Времени конвертации
			case 1:
				tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);		// shortest integration time (bright light)
				break;
			case 2:
				tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);		// shortest integration time (bright light)
				break;
			case 3:	
				tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);		// longest integration time (dim light)
				break;
			default:
				tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);		// shortest integration time (bright light)
		}
		Serial.println(F("done"));
	}
	else Serial.println(F("error"));
}


// ==========================================================================================================================================
boolean InitializingBH1750(byte AdressSensor, byte NumberSensor){
	switch(AdressSensor){
		case 0x23:
			Serial.print(F("\tInitializing BH1750 0x23..."));
			if(BH1750_1.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)){
				Serial.println(F("done"));
			}
			else {
				Serial.println(F("error"));
			}
			break;
		case 0x5c:
			Serial.print(F("\tInitializing BH1750 0x5C..."));
			if(BH1750_2.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)){
				Serial.println(F("done"));
			}
			else {
				Serial.println(F("error"));
			}
			break;
	}
}


// ==========================================================================================================================================
boolean InitializingMAX44009(byte AdressSensor, byte NumberSensor){
	byte InitNumber;
	Serial.print(F("\tInitializing MAX44009 "));
	switch(AdressSensor){
		case 0x4a:
			Serial.print(F("0x4a..."));
			InitNumber = 0;
			break;
		case 0x4b:
			Serial.print(F("0x4b..."));
			InitNumber = 1;
		break;
	}
	if(Init_MAX44009[InitNumber] == false){
		if(!MAX44009_begin(AdressSensor, NumberSensor)){
			Serial.println("Could not find a valid MAX44009 sensor");
		}
		else Serial.println("done");
		Init_MAX44009[InitNumber] = true;
	}
}





// ==========================================================================================================================================
// ==========================================================================================================================================
// ==========================================================================================================================================
void ConfigSensor(byte NumberSensor){										// Функция инициализации датчиков
	byte InitNumber;
	byte AdressSensor = EEPROM.read(E_Address_Sensor + (NumberSensor * 10));

	/* Адреса встроенных устройств: 
									- DS3231 0х68
									- 24C32 0х50-0х57
									- LCD PCF8574 адрес 0x23 */
	switch(EEPROM.read(E_Type_A_Sensor + NumberSensor)){
		case 1:																// DS18B20
			if(!Init_DS18B20){												// Если Wire шина еще не инициализировалась 
				InitializingDS18B20(AdressSensor, NumberSensor);
			}
			break;
		case 2:																// AM2302
			break;
		case 3:																// Si7013, Si7020, Si7021, HTU21D, SHT21 (адрес только 0x40)
			InitializingHTU21D(AdressSensor, NumberSensor);
			break;
		case 4:																// BME280 (0x76, 0x77)
			InitializingBME280(AdressSensor, NumberSensor);
			break;
		case 5:																// BMP280 (0x76, 0x77)
			InitializingBMP280(AdressSensor, NumberSensor);
			break;
		case 6:																// INA219 (0x40, 0x41, 0x44, 0x45)
			InitializingINA219(AdressSensor, NumberSensor);
			break;
		case 8:																// TSL2561 (0x29, 0x39, 0x49)
			InitializingTSL2561(AdressSensor, NumberSensor);
			break;
		case 9:																// BH1750 (0x23, 0x5c)
			InitializingBH1750(AdressSensor, NumberSensor);
			break;
		case 10:															// MAX44009 (0х4a, ...)
			InitializingMAX44009(AdressSensor, NumberSensor);
			break;
		case 11:
			break;
		case 12:
			break;
	}
}