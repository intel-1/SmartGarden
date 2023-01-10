
#include "Sensors.h"
#include "GSM.h"
#include "LCDdisplay.h"
#include "Configuration.h"



#define ERROR 0
#define DONE 1
#define INIT 2


bool Init_DS18B20[8];		// Номер Wire шины которая уже проинициализирована
bool Init_HTU21D = false;	// 
bool Init_BH1750[2];		// 2 доступных адреса
bool Init_MAX44009[2];		// 2 доступных адреса (0x4a, 0x4b)
bool Init_bme280[2];		// 2 доступных адреса (0x76, 0x77)
bool Init_bmp280[2];		//
bool Init_ina219[4];		// 4 доступных адреса (0x40, 0x41, 0x42, 0x43)
bool Init_TSL2561[3];		// 3 доступных адреса (0x29, 0x39, ox49)

//byte bme280_sensors[2];		// Адреса bme280 с привязкой к номеру датчика (доступные адреса 0x76 и 0x77)



void View_Name_Work_Port_DS18B20(byte _NumberPort, byte State){
	switch(State){
		case 1:
			Serial.print(F("\t\tConfiguration Wire InputGPIO.P")); Serial.print(_NumberPort);
			Serial.println(F("...done"));
			Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + GSM_GET_Tab_2 + (F("Configuration Wire InputGPIO.P")) + _NumberPort + (F("...done")) + (F("\"")), GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
			
			Clean_LCD(LCD_LINE_4, LCD_START_SYMBOL_1);
			WriteToLCD(String(F("Wire InputGPIO.P")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
			WriteToLCD(String(_NumberPort), LCD_LINE_4, LCD_START_SYMBOL_19, LCD_SCREEN_REFRESH_DELAY);
			break;
		case 0:
			Serial.print(F("Wire InputGPIO.P")); Serial.print(_NumberPort); Serial.println(F(" has been configured before"));
			break;
	}
}
// -------------------------------------------------------------------------------------------------------------------------------------------


void Output_Text_To_LCD(byte NumberSensor, String Text){
	WriteToLCD(Text, LCD_LINE_3, LCD_START_SYMBOL_2, LCD_NO_SCREEN_REFRESH_DELAY);
	WriteToLCD(String(F("S_")), LCD_LINE_3, LCD_START_SYMBOL_17, LCD_NO_SCREEN_REFRESH_DELAY);
	WriteToLCD(String(NumberSensor), LCD_LINE_3, LCD_START_SYMBOL_19, LCD_NO_SCREEN_REFRESH_DELAY);
}
// -------------------------------------------------------------------------------------------------------------------------------------------	


void Output_Text_To_LCD_and_UART(byte _Text, byte _Start_Symbol){
	switch(_Text){
		case 0:
			Serial.println(F("error"));
			WriteToLCD(String(F("...error")), LCD_LINE_4, _Start_Symbol, LCD_SCREEN_REFRESH_DELAY);
			break;
		case 1:
			Serial.println(F("done"));
			WriteToLCD(String(F("...done")), LCD_LINE_4, _Start_Symbol, LCD_SCREEN_REFRESH_DELAY);
			break;
		case 2:
			WriteToLCD(String(F("...init")), LCD_LINE_4, _Start_Symbol, LCD_SCREEN_REFRESH_DELAY);
			break;
	}
}


// ==========================================================================================================================================
// void printAlarms(uint8_t deviceAddress[], byte Number_Input_GPIO_Port){				// Функция вывода информации о параметрах тревоги
// 	char temp_A_H;
// 	char temp_A_L;
// 	switch(Number_Input_GPIO_Port){
// 		case 1:
// 			temp_A_H = sensors1.getHighAlarmTemp(deviceAddress);
// 			temp_A_L = sensors1.getLowAlarmTemp(deviceAddress);
// 			break;
// 		case 2:
// 			temp_A_H = sensors2.getHighAlarmTemp(deviceAddress);
// 			temp_A_L = sensors2.getLowAlarmTemp(deviceAddress);
// 			break;
// 		case 3:
// 			temp_A_H = sensors3.getHighAlarmTemp(deviceAddress);
// 			temp_A_L = sensors3.getLowAlarmTemp(deviceAddress);
// 			break;
// 		case 4:
// 			temp_A_H = sensors4.getHighAlarmTemp(deviceAddress);
// 			temp_A_L = sensors4.getLowAlarmTemp(deviceAddress);
// 			break;
// 		case 5:
// 			temp_A_H = sensors5.getHighAlarmTemp(deviceAddress);
// 			temp_A_L = sensors5.getLowAlarmTemp(deviceAddress);
// 			break;
// 		case 6:
// 			temp_A_H = sensors6.getHighAlarmTemp(deviceAddress);
// 			temp_A_L = sensors6.getLowAlarmTemp(deviceAddress);
// 			break;
// 	}
// 	Serial.print(F("High Alarm: "));
// 	Serial.print(temp_A_H, DEC);
// 	Serial.print(F("C"));
// 	Serial.print(F(" | Low Alarm: "));
// 	Serial.print(temp_A_L, DEC);
// 	Serial.print(F("C"));
// }


boolean InitializingDS18B20(byte AdressSensor, byte NumberSensor){
	#define STATE_ERROR 0
	#define STATE_DONE 1
		
	ina219.begin(ADDRESS_INPUT_INA);
	byte Config_Sensor_B = EEPROM_int_read(E_ConfigSensor_B + NumberSensor*2);

	Serial.print(F("\tInitializing DS18B20 (Sensor ")); Serial.print(NumberSensor); Serial.println(F(")..."));
	
	DeviceAddress AddresSensor = {	EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 0),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 1),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 2),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 3),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 4),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 5),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 6),
									EEPROM.read((E_Address_Sensor + NumberSensor * 10) + 7)};
	
	// ---------------------------------------------------------------
	switch(Config_Sensor_B){
		case 1:
			if(!Init_DS18B20[Config_Sensor_B]){
				pinMode(INPUT_GPIO_P1, INPUT);
				sensors1.begin();
				Init_DS18B20[Config_Sensor_B] = true;
				View_Name_Work_Port_DS18B20(Config_Sensor_B, STATE_DONE);
			}
			else View_Name_Work_Port_DS18B20(Config_Sensor_B, STATE_ERROR);
			break;
		case 2:
			if(!Init_DS18B20[Config_Sensor_B]){
				pinMode(INPUT_GPIO_P2, INPUT);
				sensors2.begin();
				Init_DS18B20[Config_Sensor_B] = true;
				View_Name_Work_Port_DS18B20(Config_Sensor_B, STATE_DONE);
			}
			else View_Name_Work_Port_DS18B20(Config_Sensor_B, STATE_ERROR);
			break;
		case 3:
			if(!Init_DS18B20[Config_Sensor_B]){
				pinMode(INPUT_GPIO_P3, INPUT);
				sensors3.begin();
				Init_DS18B20[Config_Sensor_B] = true;
				View_Name_Work_Port_DS18B20(Config_Sensor_B, STATE_DONE);
			}
			else View_Name_Work_Port_DS18B20(Config_Sensor_B, STATE_ERROR);
			break;
		case 4:
			if(!Init_DS18B20[Config_Sensor_B]){
				pinMode(INPUT_GPIO_P4, INPUT);
				sensors4.begin();
				Init_DS18B20[Config_Sensor_B] = true;
				View_Name_Work_Port_DS18B20(Config_Sensor_B, STATE_DONE);
			}
			else View_Name_Work_Port_DS18B20(Config_Sensor_B, STATE_ERROR);
			break;
		case 5:
			if(!Init_DS18B20[Config_Sensor_B]){
				pinMode(INPUT_GPIO_P5, INPUT);
				sensors5.begin();
				Init_DS18B20[Config_Sensor_B] = true;
				View_Name_Work_Port_DS18B20(Config_Sensor_B, STATE_DONE);
			}
			else View_Name_Work_Port_DS18B20(Config_Sensor_B, STATE_ERROR);
			break;
		case 6:
			if(!Init_DS18B20[Config_Sensor_B]){
				pinMode(INPUT_GPIO_P6, INPUT);
				sensors6.begin();
				Init_DS18B20[Config_Sensor_B] = true;
				View_Name_Work_Port_DS18B20(Config_Sensor_B, STATE_DONE);
			}
			else View_Name_Work_Port_DS18B20(Config_Sensor_B, STATE_ERROR);
			break;
		default:
			Serial.print(F("\t\tSensor ")); Serial.print(NumberSensor);
			Serial.println(F(": Work port is not configured"));
			WriteToLCD(String(F("port is not config")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_SCREEN_REFRESH_DELAY);
			Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + GSM_GET_Tab_2 + (F("Sensor ")) + NumberSensor + (F(": Work port is not configured")) + (F("\"")), GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
	}						
	
	// ================================ Задаем точность измерения ================================
	switch(Config_Sensor_B){
		case 1:
			sensors1.setResolution(AddresSensor, EEPROM.read(E_ConfigSensor_A + NumberSensor));
			Serial.print(F("\t\t\tResolution: "));
			Serial.println(sensors1.getResolution(AddresSensor), DEC);
			//sensors1.setHighAlarmTemp(AddresSensor, 20);
			//sensors1.setLowAlarmTemp(AddresSensor, -10);
			break;
		case 2:
			sensors2.setResolution(AddresSensor, EEPROM.read(E_ConfigSensor_A + NumberSensor));
			Serial.print(F("\t\t\tResolution: "));
			Serial.println(sensors2.getResolution(AddresSensor), DEC);
			//sensors2.setHighAlarmTemp(AddresSensor, 20);
			//sensors2.setLowAlarmTemp(AddresSensor, -10);
			break;
		case 3:
			sensors3.setResolution(AddresSensor, EEPROM.read(E_ConfigSensor_A + NumberSensor));
			Serial.print(F("\t\t\tResolution: "));
			Serial.println(sensors3.getResolution(AddresSensor), DEC);
			//sensors3.setHighAlarmTemp(AddresSensor, 20);
			//sensors3.setLowAlarmTemp(AddresSensor, -10);
			break;
		case 4:
			sensors4.setResolution(AddresSensor, EEPROM.read(E_ConfigSensor_A + NumberSensor));
			Serial.print(F("\t\t\tResolution: "));
			Serial.println(sensors4.getResolution(AddresSensor), DEC);
			//sensors4.setHighAlarmTemp(AddresSensor, 20);
			//sensors4.setLowAlarmTemp(AddresSensor, -10);
			break;
		case 5:
			sensors5.setResolution(AddresSensor, EEPROM.read(E_ConfigSensor_A + NumberSensor));
			Serial.print(F("\t\t\tResolution: "));
			Serial.println(sensors5.getResolution(AddresSensor), DEC);
			//sensors5.setHighAlarmTemp(AddresSensor, 20);
			//sensors5.setLowAlarmTemp(AddresSensor, -10);
			break;
		case 6:
			sensors6.setResolution(AddresSensor, EEPROM.read(E_ConfigSensor_A + NumberSensor));
			Serial.print(F("\t\t\tResolution: "));
			Serial.println(sensors6.getResolution(AddresSensor), DEC);
			//sensors6.setHighAlarmTemp(AddresSensor, 20);
			//sensors6.setLowAlarmTemp(AddresSensor, -10);
			break;
	}
	
	//StartMeasurementIndicationsDS18B20();				// Сразу отдаем датчикам команду измерять температуру 
}


// ==========================================================================================================================================
boolean InitAM2302(byte AdressSensor, byte NumberSensor){
	
}


// ==========================================================================================================================================
boolean InitializingBME280(byte AdressSensor, byte NumberSensor){
	byte InitNumber;
	Serial.print(F("\tInitializing BME280 (Sensor ")); Serial.print(NumberSensor); Serial.print(F(")"));
	switch(AdressSensor){
		case 0x76:
			Serial.print(F("...0x76..."));
			WriteToLCD(String(F("...0x76...")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
			InitNumber = 0;
			break;
		case 0x77:
			Serial.print(F("...0x77..."));
			WriteToLCD(String(F("...0x77...")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
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
			Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
		}
		else{ 
			Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_13);
		}
		Init_bme280[InitNumber] = true;				// Поднимаем флаг чтобы не было повторной инициализации у другого такого же датчика
	}
}


// ==========================================================================================================================================
boolean InitializingHTU21D(byte AdressSensor, byte NumberSensor){
	if(!Init_HTU21D){												// Если датчик еще не инициализировался
		Serial.print(F("\tInitializing HTU21D (Sensor ")); Serial.print(NumberSensor); Serial.print(F(")..."));
		if (!HTU21.begin()){
			WriteToLCD(String(F("...is not connected")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_SCREEN_REFRESH_DELAY);
			Serial.println(F("sensor is failed or not connected"));
		}
		else{ 
			Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_4);
		}
		Init_HTU21D = true;
	}
	else Serial.println(F("Previously initialized"));
}


// ==========================================================================================================================================
boolean InitializingBMP280(byte AdressSensor, byte NumberSensor){
	byte InitNumber;
	
	Serial.print(F("\tInitializing BMP280 (Sensor ")); Serial.print(NumberSensor); Serial.print(F(")"));
	
	switch(AdressSensor){
		case 0x76:
			Serial.print(F("...0x76..."));
			WriteToLCD(String(F("...0x76...")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
			InitNumber = 0;
			break;
		case 0x77:
			Serial.print(F("...0x77..."));
			WriteToLCD(String(F("...0x77...")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
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
			Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
		}
		else{
			Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_13);
		}
		Init_bmp280[InitNumber] = true;				// Поднимаем флаг чтобы не было повторной инициализации у другого такого же датчика
	}
}


// ==========================================================================================================================================
// ====================================================== Инициализация датчика INA219 ======================================================
// ==========================================================================================================================================
boolean InitializingINA219(byte AddressSensor, byte NumberSensor){
	bool _ErrorAdressSensor = false;	
	bool InitializSensor = false;
	
	Serial.print(F("\tInitializing INA219..."));
	
	switch(AddressSensor){
		case 0x40:
			if(Init_ina219[0]){
				Init_ina219[0] = true;
				_ErrorAdressSensor = true;
			}
			else{ 
				Serial.println(F("Previously initialized"));
				Output_Text_To_LCD_and_UART(INIT, LCD_START_SYMBOL_4);
			}
			break;
		case 0x41:
			if(Init_ina219[1]){
				Init_ina219[1] = true;
				_ErrorAdressSensor = true;
			}
			else{ 
				Serial.println(F("Previously initialized"));
				Output_Text_To_LCD_and_UART(INIT, LCD_START_SYMBOL_4);
			}
			break;
		case 0x42:
			if(Init_ina219[2]){
				Init_ina219[2] = true;
				_ErrorAdressSensor = true;
			}
			else{ 
				Serial.println(F("Previously initialized"));
				Output_Text_To_LCD_and_UART(INIT, LCD_START_SYMBOL_4);
			}
			break;
		case 0x43:
			if(Init_ina219[3]){
				Init_ina219[3] = true;
				_ErrorAdressSensor = true;
			}
			else{ 
				Serial.println(F("Previously initialized"));
				Output_Text_To_LCD_and_UART(INIT, LCD_START_SYMBOL_4);
			}
			break;
		default:
			Serial.print(F("...error address sensor - 0x")); Serial.println(AddressSensor);	
	}
	
	if(!_ErrorAdressSensor){											// Если верен адрес датчика
		ina219.begin(AddressSensor);
		Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_4);
	}
	else Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_4);
}


// ==========================================================================================================================================
// ====================================================== Инициализация датчика TSL2561 =====================================================
// ==========================================================================================================================================
void Config_Integration_Time(byte AddressSensor, byte _Integration_Time){
	switch(AddressSensor){
		case TSL2561_ADDR_LOW:
			switch (_Integration_Time){
				case TSL2561_INTEGRATIONTIME_13MS:
					tsl1.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
					break;
				case TSL2561_INTEGRATIONTIME_101MS:
					tsl1.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
					break;
				case TSL2561_INTEGRATIONTIME_402MS:
					tsl1.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);
					break;
			}
			break;
		case TSL2561_ADDR_FLOAT:
			switch (_Integration_Time){
				case TSL2561_INTEGRATIONTIME_13MS:
					tsl2.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
					break;
				case TSL2561_INTEGRATIONTIME_101MS:
					tsl2.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
					break;	
				case TSL2561_INTEGRATIONTIME_402MS:
					tsl2.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);
					break;
			}
			break;
		case TSL2561_ADDR_HIGH:
			switch (_Integration_Time){
				case TSL2561_INTEGRATIONTIME_13MS:
					tsl3.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
					break;
				case TSL2561_INTEGRATIONTIME_101MS:
					tsl3.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
					break;
				case TSL2561_INTEGRATIONTIME_402MS:
					tsl3.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);
					break;
			}
			break;
	}
}
void Config_Gain(byte AddressSensor, byte _Gain){
	switch(AddressSensor){
		case TSL2561_ADDR_LOW:
			switch (_Gain){
				case GAIN_16X:
					tsl1.setGain(TSL2561_GAIN_16X);
					break;
				case GAIN_1X:
					tsl1.setGain(TSL2561_GAIN_16X);
					break;
				case GAIN_AUTO:
					tsl1.setGain(TSL2561_GAIN_16X);
					break;
				break;
			}
			break;
		case TSL2561_ADDR_FLOAT:
			switch (_Gain){
				case GAIN_16X:
					tsl2.setGain(TSL2561_GAIN_16X);
					break;
				case GAIN_1X:
					tsl2.setGain(TSL2561_GAIN_16X);
					break;
				case GAIN_AUTO:
					tsl2.setGain(TSL2561_GAIN_16X);
					break;
				break;
			}
			break;
		case TSL2561_ADDR_HIGH:
			switch (_Gain){
				case GAIN_16X:
					tsl3.setGain(TSL2561_GAIN_16X);
					break;
				case GAIN_1X:
					tsl3.setGain(TSL2561_GAIN_16X);
					break;
				case GAIN_AUTO:
					tsl3.setGain(TSL2561_GAIN_16X);
					break;
				break;
			}
			break;
	}
}

boolean InitializingTSL2561(byte AddressSensor, byte NumberSensor){
	bool _ErrorAdressSensor = false;
	
	Serial.print(F("\tInitializing TSL2561 (Sensor ")); Serial.print(NumberSensor); Serial.print(F(")"));
	switch(AddressSensor){
		case TSL2561_ADDR_FLOAT:
			Serial.print(F("...0x39..."));
			WriteToLCD(String(F("..0x39...")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
			break;
		case TSL2561_ADDR_LOW:
			Serial.print(F("...0x29..."));
			WriteToLCD(String(F("...0x29...")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
			break;
		case TSL2561_ADDR_HIGH:
			Serial.print(F("...0x49..."));
			WriteToLCD(String(F("...0x49...")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
			break;
		default:
			Serial.print(F("...error address sensor - 0x")); Serial.println(AddressSensor); 
			_ErrorAdressSensor = true;
	}
	// ================================== Блок инициализации датчика ==================================
	bool InitializSensor = false;	
	if(!_ErrorAdressSensor){											// Если верен адрес датчика
		switch(AddressSensor){
			case TSL2561_ADDR_LOW:
				if(!Init_TSL2561[0]){									// Если датчик еще не инициализировался
					if(tsl1.begin()){
						InitializSensor = true;
						Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
						Init_TSL2561[0] = true;
					}
					else Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_13);
				}
				else Serial.println(F("Previously initialized"));
				break;
			case TSL2561_ADDR_FLOAT:
				if(!Init_TSL2561[1]){									// Если датчик еще не инициализировался
					if(tsl2.begin()){
						InitializSensor = true;
						Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
						Init_TSL2561[1] = true;
					}
					else Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_13);
				}
				else Serial.println(F("Previously initialized"));
				break;
			case TSL2561_ADDR_HIGH:
				if(!Init_TSL2561[2]){									// Если датчик еще не инициализировался	
					if(tsl3.begin()){
						InitializSensor = true;
						Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
						Init_TSL2561[2] = true;
					}
					else Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_13);
				}
				else Serial.println(F("Previously initialized"));
				break;
		}
		// ================================== Блок конфигурирования датчика ==================================
		if(InitializSensor){											// Если до этого успешно проиниализировали датчик
			Serial.print(F("\t\t\tGain: "));
			switch(EEPROM_int_read(E_ConfigSensor_C + NumberSensor)){	// Настройка усиления
				case GAIN_16X:											// Тусклый свет
					Serial.println(F("16x"));
					Config_Gain(AddressSensor, GAIN_16X);
					break;
				case GAIN_1X:											// Яркий свет
					Serial.println(F("1x"));
					Config_Gain(AddressSensor, GAIN_1X);
					break;
				case GAIN_AUTO:		
					Serial.println(F("auto"));					
					Config_Gain(AddressSensor, GAIN_AUTO);
					break;
				default:												
					Serial.println(F("auto (default)"));	
					Config_Gain(AddressSensor, GAIN_AUTO);					
			}
			
			Serial.print(F("\t\t\tIntegration time: "));
			switch(EEPROM_int_read(E_ConfigSensor_D + NumberSensor)){			// Времени конвертации
				case INTEGRATIONTIME_13MS:
					Serial.println(F("13ms"));
					Config_Integration_Time(AddressSensor, TSL2561_INTEGRATIONTIME_13MS);	
					break;
				case INTEGRATIONTIME_101MS:
					Serial.println(F("101ms"));
					Config_Integration_Time(AddressSensor, TSL2561_INTEGRATIONTIME_101MS);					
					break;
				case INTEGRATIONTIME_402MS:
					Serial.println(F("402ms"));
					Config_Integration_Time(AddressSensor, TSL2561_INTEGRATIONTIME_402MS);	
					break;
				default:
					Serial.println(F("13ms (default)"));
					Config_Integration_Time(AddressSensor, TSL2561_INTEGRATIONTIME_13MS);	
			}
		}
	}
}


// ==========================================================================================================================================
// ===================================================== Инициализация датчика MAX44009 =====================================================
// ==========================================================================================================================================
boolean InitializingBH1750(byte AddressSensor, byte NumberSensor){
	bool _Error_Init_Sensor = false;
	Serial.print(F("\tInitializing BH1750 (Sensor ")); Serial.print(NumberSensor); Serial.print(F(")"));
	
	switch(AddressSensor){
		case 0x23:
			WriteToLCD(String(F("...0x23...")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
			Serial.print(F("...0x23..."));
			if(!Init_BH1750[0]){
				switch(EEPROM.read(E_ConfigSensor_A + NumberSensor)){
					case 1:
						if(BH1750_1.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)){
							_Error_Init_Sensor = true;
							Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
						}
						else Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_13);
						break;
					case 2:
						if(BH1750_1.begin(BH1750::CONTINUOUS_HIGH_RES_MODE_2)){
							_Error_Init_Sensor = true;							
							Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
						}
						else Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_13);
						break;
					case 3:
						if(BH1750_1.begin(BH1750::CONTINUOUS_LOW_RES_MODE)){
							_Error_Init_Sensor = true;							
							Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
						}
						else Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_13);
						break;
					default:
						if(BH1750_1.begin()){
							_Error_Init_Sensor = true;							
							Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
						}
						else{
							Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_13);
						}
				}
			}
			else Serial.println(F("Previously initialized"));
			break;
		case 0x5c:
			WriteToLCD(String(F("...0x5C...")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
			Serial.print(F("\t...0x5C..."));
			if(!Init_BH1750[1]){
				if(BH1750_2.begin()){
					_Error_Init_Sensor = true;
					Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
				}
				else{
					Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_13);
				}
			}
			else Serial.println(F("...Previously initialized"));
			break;
	}
	
	if(_Error_Init_Sensor){
		switch(EEPROM.read(E_ConfigSensor_A + NumberSensor)){												// Режим измерения
			case 1:
				Serial.println(F("\t\tMeasurement at 1 lux resolution."));
				Serial.println(F("\t\tMeasurement time is approx 120ms."));
				break;
			case 2:
				Serial.println(F("\t\tMeasurement at 0.5 lux resolution."));
				Serial.println(F("\t\tMeasurement time is approx 120ms."));
				break;
			case 3:
				Serial.println(F("\t\tMeasurement at 4 lux resolution."));
				Serial.println(F("\t\tMeasurement time is approx 16ms."));
				break;
			case 4:
				Serial.println(F("\t\tMeasurement at 1 lux resolution."));
				Serial.println(F("\t\tMeasurement time is approx 120ms."));
				break;
			case 5:
				Serial.println(F("\t\tMeasurement at 0.5 lux resolution."));
				Serial.println(F("\t\tMeasurement time is approx 120ms."));
				break;
			case 6:
				Serial.println(F("\t\tMeasurement at 4 lux resolution."));
				Serial.println(F("\t\tMeasurement time is approx 16ms."));
				break;
			default:
				Serial.println(F("\t\tMeasurement unconfigured"));
		}
	}
}


// ==========================================================================================================================================
// ===================================================== Инициализация датчика MAX44009 =====================================================
// ==========================================================================================================================================
boolean InitializingMAX44009(byte AddressSensor, byte NumberSensor){
	Serial.print(F("\tInitializing MAX44009 (Sensor ")); Serial.print(NumberSensor); Serial.print(F(")"));
	switch(AddressSensor){
		case 0x4a:
			WriteToLCD(String(F("...0x4a...")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
			Serial.print(F("...0x4a..."));
			Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
			break;
		case 0x4b:
			WriteToLCD(String(F("...0x4b...")), LCD_LINE_4, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
			Serial.print(F("...0x4b..."));
			Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
			break;
		default:
			Serial.println(F("...Error address"));
			Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_13);
	}
}


// ==========================================================================================================================================
// ====================================================== Инициализация датчика AHT25 =======================================================
// ==========================================================================================================================================
boolean InitializingHTA25(byte AddressSensor, byte NumberSensor){
	Serial.print(F("\tInitializing AHT25 (Sensor ")); Serial.print(NumberSensor); Serial.print(F(")..."));
	if (AHT25.begin()){
		Output_Text_To_LCD_and_UART(DONE, LCD_START_SYMBOL_13);
	}
	else{ 
		Output_Text_To_LCD_and_UART(ERROR, LCD_START_SYMBOL_13);
	}
}
	

// ==========================================================================================================================================
// ========================================================= Инициализации датчиков =========================================================
// ==========================================================================================================================================
void ConfigSensor(byte NumberSensor){									
	byte InitNumber;
	byte AdressSensor = EEPROM.read(E_Address_Sensor + (NumberSensor * 10));

	Clean_LCD(LCD_LINE_3, LCD_START_SYMBOL_1);
	Clean_LCD(LCD_LINE_4, LCD_START_SYMBOL_1);
	
	switch(EEPROM.read(E_Type_A_Sensor + NumberSensor)){
		case S_DS18B20:																// DS18B20
			Output_Text_To_LCD(NumberSensor, String(F("--- DS18B20:")));
			InitializingDS18B20(AdressSensor, NumberSensor);
			break;
		case S_AM2302:																// AM2302
			Output_Text_To_LCD(NumberSensor, String(F("--- AM2302:")));
			InitAM2302(AdressSensor, NumberSensor);
			break;
		case S_HTU21D:																// Si7013, Si7020, Si7021, HTU21D, SHT21 (адрес только 0x40)
			Output_Text_To_LCD(NumberSensor, String(F("--- HTU21D:")));
			InitializingHTU21D(AdressSensor, NumberSensor);
			break;
		case S_BME280:																// BME280 (0x76, 0x77)
			Output_Text_To_LCD(NumberSensor, String(F("--- BME280:")));
			InitializingBME280(AdressSensor, NumberSensor);
			break;
		case S_BMP280:																// BMP280 (0x76, 0x77)
			Output_Text_To_LCD(NumberSensor, String(F("--- BMP280:")));
			InitializingBMP280(AdressSensor, NumberSensor);
			break;
		case S_INA219:																// INA219 (0x40, 0x41, 0x44, 0x45)
			Output_Text_To_LCD(NumberSensor, String(F("--- INA219:")));
			InitializingINA219(AdressSensor, NumberSensor);
			break;
		case S_TSL2561:																// TSL2561 (0x29, 0x39, 0x49)
			Output_Text_To_LCD(NumberSensor, String(F("--- TSL2561:")));
			InitializingTSL2561(AdressSensor, NumberSensor);
			break;
		case S_BH1750:																// BH1750 (0x23, 0x5c)
			Output_Text_To_LCD(NumberSensor, String(F("--- BH1750:")));	
			InitializingBH1750(AdressSensor, NumberSensor);
			break;
		case S_MAX44009:															// MAX44009 (0х4a, ...)
			Output_Text_To_LCD(NumberSensor, String(F("--- MAX44009:")));	
			InitializingMAX44009(AdressSensor, NumberSensor);
			break;
		case S_LM75:																// LN75
			Output_Text_To_LCD(NumberSensor, String(F("--- LM75:")));
			break;
		case S_INA3221:																// INA3221
			Output_Text_To_LCD(NumberSensor, String(F("--- INA3221:")));
			break;
		case S_AHT25:																// AHT25
			Output_Text_To_LCD(NumberSensor, String(F("--- AHT25:")));
			InitializingHTA25(AdressSensor, NumberSensor);
			break;
	}
}