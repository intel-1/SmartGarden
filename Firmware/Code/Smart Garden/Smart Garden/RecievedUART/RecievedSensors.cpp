#include <./Arduino.h>
#include <./EEPROM.h>

#include "RecievedSensors.h"
#include "../EEPROM_ADR.h"
#include "../ParsingDataOnSerial.h"
#include "../Sensors.h"
#include "../Configuration.h"
#include "../DigitalPorts.h"
#include "RegisterSensors.h"



// -------------------------------------------------------------------------
// -------------- Номера ячеек в массиве InputFromSerial0[] ----------------
// -------------------------------------------------------------------------
#define INPUT_BYTE_NUMBER_SENSOR					InputFromSerial0[0]
#define INPUT_BYTE_STATUS_SENSOR					InputFromSerial0[2]
#define INPUT_BYTE_TYPE_A_SENSOR					InputFromSerial0[3]
#define INPUT_BYTE_TYPE_B_SENSOR					InputFromSerial0[4]
#define INPUT_BYTE_SGB_A							InputFromSerial0[5]
#define INPUT_BYTE_SGB_B							InputFromSerial0[6]
#define INPUT_BYTE_SGB_C							InputFromSerial0[7]
#define INPUT_BYTE_ADDRESS_SENSOR					InputFromSerial0[8]
#define INPUT_BYTE_ALLOW_MANUAL_INTERVAL_SENSOR		InputFromSerial0[16]
#define INPUT_BYTE_SENSOR_POLL_INTERVAL				InputFromSerial0[17]
#define INPUT_BYTE_ALLOW_SENSOR_CONTROLL_PORT		InputFromSerial0[18]
#define INPUT_BYTE_SENSOR_CONTROLL_PORT				InputFromSerial0[19]
#define INPUT_BYTE_DELAY_RUN_CONTROLL_PORT			InputFromSerial0[20]
#define INPUT_BYTE_CONFIG_SENSOR_A					InputFromSerial0[21]
#define INPUT_BYTE_CONFIG_SENSOR_B					InputFromSerial0[22]
#define INPUT_BYTE_CONFIG_SENSOR_C					InputFromSerial0[23]
#define INPUT_BYTE_CONFIG_SENSOR_D					InputFromSerial0[24]
#define INPUT_BYTE_NAME_SENSOR						InputFromSerial0[25]
#define INPUT_BYTE_QUANTITY_READ_SENSORS			InputFromSerial0[44]
#define INPUT_BYTE_QUANTITY_ERROR_READ_SENSOR		InputFromSerial0[45]
#define INPUT_BYTE_QUANTITY_ERRORS					InputFromSerial0[46]
#define INPUT_BYTE_REACT_TO_MISTAKES_EXT			InputFromSerial0[47]
#define INPUT_BYTE_REACT_TO_MISTAKES_SD				InputFromSerial0[48]
#define INPUT_BYTE_REACT_TO_MISTAKES_LED			InputFromSerial0[49]
// -------------------------------------------------------------------------




void SentConfigSensorsUART(){
	Serial.print(F("========== Конфигурация датчика '")); Serial.print(INPUT_BYTE_NUMBER_SENSOR); Serial.println(F("' =========="));
	// ========================================================
	Serial.print(F("Статус: "/*"Status: "*/));
	if(EEPROM.read(E_StatusSensor + INPUT_BYTE_NUMBER_SENSOR) == 1){
		Serial.println(F("Включен"/*"on"*/));
	}
	else{
		Serial.println(F("Выключен"/*"off"*/));
	}
	// ========================================================
	Serial.print(F("Имя: "/*"Name: "*/));
	for(byte i = 0; i <= 19; i++){
		byte Symbol = EEPROM.read((E_NameSensor + INPUT_BYTE_NUMBER_SENSOR-1) + i + 20 * (INPUT_BYTE_NUMBER_SENSOR-1));
		if(Symbol != 0){
			Serial.write(Symbol);
		}
		if(i == 18){
			Serial.println();
		}		
	}
	// ========================================================
	Serial.print(F("Привязка к группе: "/*"Channel: "*/));
	for(byte i = 0; i < 3; i ++){
		if(EEPROM.read(E_SBG + INPUT_BYTE_NUMBER_SENSOR*3 + i) != 0){
			Serial.print(F(" Значение_"/*" Value_"*/)); Serial.print(i + 1); Serial.print(F(": ")); Serial.print(EEPROM.read(E_SBG + (INPUT_BYTE_NUMBER_SENSOR*3) + i)); Serial.print(F("\t"));
		}
		if(i == 2){
			Serial.println();
		}
	}
	// ========================================================
	Serial.print(F("Ошибки показаний: "/*"Errors: "*/));
	for(byte i = 0; i < 3; i ++){
		Serial.print(SensorsError[INPUT_BYTE_NUMBER_SENSOR-1][i]); Serial.print(F(" "));
	}
	Serial.println();
	// ========================================================
	Read_ETAS_register(INPUT_BYTE_NUMBER_SENSOR);							// Выводим наименование и адрес датчика
	if(EEPROM.read(E_Type_A_Sensor + INPUT_BYTE_NUMBER_SENSOR) != 7){		// Если датчик не настроен как аналоговый
		Read_E_Type_B_Sensor_register(INPUT_BYTE_NUMBER_SENSOR, 1);			// Выводим тип измеряемых данных
	}
	// ========================================================
	Serial.print(F("Интервал измерения показаний: "/*"Sensor poll interval: "*/));
	if(EEPROM.read(E_AllowManualIntervalSensor + INPUT_BYTE_NUMBER_SENSOR) == 1){		// Если включен ручной интервал опроса датчика
		Serial.println(F("Ручной"/*"manual"*/));
		Serial.print(F("\tИнтервал измерения: "/*"Interval poll: "*/)); Serial.print(EEPROM_int_read(E_SensorPollInterval + INPUT_BYTE_NUMBER_SENSOR * 2)); Serial.println(F("сек"/*"sec"*/));
	}
	else Serial.println(F("Автоматический"/*"automatic"*/));
	// ========================================================
	Serial.print(F("Использование порта управления: "/*"Allow the control port: "*/));
	if(EEPROM.read(E_AllowSensorControllPort + INPUT_BYTE_NUMBER_SENSOR) == 1){		// Если включен порт управления
		Serial.println(F("Включено"/*"on"*/));
		Serial.print(F("\tПорт управления: "/*"Control Port: "*/));
		if(DigitalPort(EEPROM.read(E_SensorControllPort + INPUT_BYTE_NUMBER_SENSOR), DIGITAL_PORT_OFF, DIGITAL_PORT_RETURN_NAME_PORT, LOG_TO_UART) != 255){
			Serial.println();
		}		
		else Serial.println(F("Порт не сконфигурирован!!!"/*"The port isn't configured"*/));	
		Serial.print(F("\tЗадержка после включения порта: "/*"Delay for run the control port: "*/)); Serial.print(EEPROM_int_read(E_DelayToRunControllPort + (INPUT_BYTE_NUMBER_SENSOR*2))); Serial.println(F("ms"));	
	}
	else Serial.println(F("Выключено"/*"off"*/));
	
	// ========================================================	
	switch(EEPROM.read(E_Type_A_Sensor + INPUT_BYTE_NUMBER_SENSOR)){
		case S_DS18B20:																			// Если DS18B20
			Serial.print(F("Разрешение датчика: ")); Serial.print(EEPROM.read(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR)); Serial.println(F("бит"));
			Serial.print(F("Порт подключения: "));
			switch(EEPROM_int_read(E_ConfigSensor_B + INPUT_BYTE_NUMBER_SENSOR*2)){			// Имя порта к которому подключене датчик
				case CONFIG_SENSOR_B_INPUT_GPIO_P1:
					Serial.println(F("Input GPIO. P1"));
					break;
				case CONFIG_SENSOR_B_INPUT_GPIO_P2:
					Serial.println(F("Input GPIO. P2"));
					break;
				case CONFIG_SENSOR_B_INPUT_GPIO_P3:
					Serial.println(F("Input GPIO. P3"));
					break;
				case CONFIG_SENSOR_B_INPUT_GPIO_P4:
					Serial.println(F("Input GPIO. P4"));
					break;
				case CONFIG_SENSOR_B_INPUT_GPIO_P5:
					Serial.println(F("Input GPIO. P5"));
					break;
				case CONFIG_SENSOR_B_INPUT_GPIO_P6:
					Serial.println(F("Input GPIO. P6"));
					break;
				default:
					Serial.println(F("Ошибка. Не корректно настроен"));
			}
			break;
		case S_BME280:																			// Если BMP280
			break;
		case S_BMP280:																			// Если BME280
			break;
		case S_ANALOG_SENSOR:																			// Analog port
			Serial.print(F("Единицы измерения: "/*"\tUnits of measure: "*/));			// Выводим единицы измерения датчика
			switch(EEPROM.read(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR)){
				case 0:
					Serial.println(F("Проценты"/*"Percent"*/));							// Проценты
					break;
				case 1:
					Serial.println(F("Значение АЦП"/*"Absolute values"*/));				// Аналоговые значения
					break;	
				default:
					Serial.println();
			}
			if(EEPROM.read(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR) == 0){		// Если измерения идут в процентах
				if(EEPROM.read(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR) == 0){
					Serial.print(F("Значение АЦП для 0%: "/*"The value for 0% "*/)); Serial.println(EEPROM_int_read(E_ConfigSensor_B + INPUT_BYTE_NUMBER_SENSOR*2));
				}
				if(EEPROM.read(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR) == 0){
					Serial.print(F("Значение АЦП для 100%: "/*"The value for 100% "*/)); Serial.println(EEPROM_int_read(E_ConfigSensor_C + INPUT_BYTE_NUMBER_SENSOR*2));
				}
			}
			break;
		case S_TSL2561:																				// Если датчик TSL2561
			Serial.print(F("Спектр измерений: "));
			switch(EEPROM.read(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR)){
				case 1:
					Serial.println(F("Видимый спектр"/*"Visible spectrum"*/));				// Видимый спектр
					break;
				case 2:
					Serial.println(F("Полный спектр"/*"Full spectrum"*/));					// Полный спектр
					break;
				case 3:
					Serial.println(F("IR спектр"/*"IR spectrum"*/));						// IR спектр
					break;
				default:
					Serial.println();
			}
			Serial.print(F("Единицы измерения: "/*"Units of measure: "*/));
// 			switch(EEPROM_int_read(E_ConfigSensor_B + INPUT_BYTE_NUMBER_SENSOR*2)){			// Выводим единицы измерения освещенности
// 				case 0:
// 					Serial.println(F("Люмены"/*"Lum"*/));
// 					break;
// 				case 1:
// 					Serial.println(F("Люксы"/*"Lux"*/));
// 					break;
// 				default:
// 					Serial.println(F("Не настроено (default: Люмены)"));
// 			}
			Serial.print(F("Величина усиления: "));
			switch(EEPROM_int_read(E_ConfigSensor_C + INPUT_BYTE_NUMBER_SENSOR*2)){			// Выводим единицы измерения освещенности
				case 1:
					Serial.println(F("16x"));
					break;
				case 2:
					Serial.println(F("1x"));
					break;
				default:
					Serial.println(F("Не настроено (default: авто усиление)"));
			}
			Serial.print(F("Время конвертации: "));
			switch(EEPROM_int_read(E_ConfigSensor_D + INPUT_BYTE_NUMBER_SENSOR*2)){			// Время конвертации
				case 1:
					Serial.println(F("13ms"));
					break;
				case 2:
					Serial.println(F("101ms"));
					break;
				case 3:
					Serial.println(F("402ms"));
					break;
				default:
					Serial.println(F("Не настроено (default: 13ms)"));		
			}
		case S_BH1750:																						// Если BH1750
			Serial.print(F("Режим измерения: "));
			switch(EEPROM.read(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR)){					// Имя порта к которому подключене датчик
				case 1:
					Serial.println(F("Measurement at 1 lux resolution. Measurement time is approx 120ms."));
					break;
				case 2:
					Serial.println(F("Measurement at 0.5 lux resolution. Measurement time is approx 120ms."));
					break;
				case 3:
					Serial.println(F("Measurement at 4 lux resolution. Measurement time is approx 16ms."));
					break;
				case 4:
					Serial.println(F("Measurement at 1 lux resolution. Measurement time is approx 120ms."));
					break;
				case 5:
					Serial.println(F("Measurement at 0.5 lux resolution. Measurement time is approx 120ms."));
					break;
				case 6:
					Serial.println(F("Measurement at 4 lux resolution. Measurement time is approx 16ms."));
					break;
				default:
					Serial.println(Text_Error_Configuration);
			}
			break;
		case S_MAX44009:																					// Если датчик MAX44009
			break;
	}		
	// ========================================================
	Serial.print(F("Кол-во измерений до фиксации показаний: "/*"Quantity of measurements: "*/));						// Количество измерений пока они не запишутся в основной массив (для увеличения точности измерения)
	Serial.println(EEPROM.read(E_QuantityReadSensors + INPUT_BYTE_NUMBER_SENSOR));
	// ========================================================
	Serial.print(F("Счетчик текущих ошибок: "/*"The current quantity of errors: "*/));									// Количество ошибочных измерений (сбрасывается командой "s 'номер датчика' 2")
	Serial.println(EEPROM_int_read(E_QuantityErrors + INPUT_BYTE_NUMBER_SENSOR*2));
	// ========================================================
	Serial.print(F("Кол-во ошибочных измерений до реакции: "/*"The maximum quantity of errors for reaction: "*/));		// Количество ошибочных измерений до реакции
	Serial.println(EEPROM.read(E_QuantityErrorReadSensor + INPUT_BYTE_NUMBER_SENSOR));	
	//========================================================
	Serial.println(F("Реакция на ошибки: "/*"Reaction to mistakes: "*/));
	Serial.print(F("\t\tВнешние: "/*"\tExt mail: "*/));
	switch(EEPROM.read(E_ReactToMistakes_Ext + INPUT_BYTE_NUMBER_SENSOR)){
		case 1:
			Serial.println(F("Отправить СМС"/*"Send SMS"*/));
			break;
		case 2:
			Serial.println(F("Отправить по GPRS"/*"Send for GPRS"*/));
			break;
		default:
			Serial.println(F("Ничего"/*"None"*/));	
	}
	Serial.print(F("\t\tЗапись на SD карту: "/*"\t\t\tSD card: "*/));
	if(EEPROM.read(E_ENABLE_LOGING_TO_SD) == 1){					// Если глобально разрешено писать на карту
		switch(EEPROM.read(E_ReactToMistakes_SD + INPUT_BYTE_NUMBER_SENSOR)){
			case 0:
				Serial.println(F("Выключена"/*"off"*/));
				break;
			case 1:
				Serial.println(F("Включена"/*"on"*/));			
				break;
			default:
				Serial.println(Text_Error_Configuration);
		}
	}
	else (Serial.println(F("Отключена глобально!!!")));
	
	Serial.print(F("\t\tLED панель: "/*"\t\t\tLED: "*/));
	switch(EEPROM.read(E_ReactToMistakes_LED + INPUT_BYTE_NUMBER_SENSOR)){
		case 1:
			Serial.println(F("Группа 1"));
			break;
		case 2:
			Serial.println(F("Группа 2"));
			break;
		case 3:
			Serial.println(F("Группа 3"));
			break;
		case 4:
			Serial.println(F("Группа 4"));
			break;
		case 5:
			Serial.println(F("Группа 5"));
			break;
		default:
			Serial.println(F("Ничего"/*"None"*/));
	}
	Serial.println(F("======================================="));
}


void SentConfigSensorsExtApp(){
	byte NameSymbol;
	byte EmptySymbol = 35;							// Символ '#'
	Serial.print(INPUT_BYTE_NUMBER_SENSOR);													Serial.print(F(" "));
	Serial.print(F("0"));																	Serial.print(F(" "));
	Serial.print(EEPROM.read(E_StatusSensor + INPUT_BYTE_NUMBER_SENSOR));					Serial.print(F(" "));
	Serial.print(EEPROM.read(E_Type_A_Sensor + INPUT_BYTE_NUMBER_SENSOR));					Serial.print(F(" "));
	Serial.print(EEPROM.read(E_Type_B_Sensor + INPUT_BYTE_NUMBER_SENSOR));					Serial.print(F(" "));
	Serial.print(EEPROM.read(E_SBG + (INPUT_BYTE_NUMBER_SENSOR*3) + 0));					Serial.print(F(" "));
	Serial.print(EEPROM.read(E_SBG + (INPUT_BYTE_NUMBER_SENSOR*3) + 1));					Serial.print(F(" "));
	Serial.print(EEPROM.read(E_SBG + (INPUT_BYTE_NUMBER_SENSOR*3) + 2));					Serial.print(F(" "));
	for(byte i = 0; i < 8; i++){
		Serial.print(EEPROM.read(E_Address_Sensor + (10*INPUT_BYTE_NUMBER_SENSOR) + i));	Serial.print(F(" "));
	}
	Serial.print(EEPROM.read(E_AllowManualIntervalSensor + INPUT_BYTE_NUMBER_SENSOR));		Serial.print(F(" "));
	Serial.print(EEPROM_int_read(E_SensorPollInterval + INPUT_BYTE_NUMBER_SENSOR*2));		Serial.print(F(" "));
	Serial.print(EEPROM.read(E_AllowSensorControllPort + INPUT_BYTE_NUMBER_SENSOR));		Serial.print(F(" "));
	Serial.print(EEPROM.read(E_SensorControllPort + INPUT_BYTE_NUMBER_SENSOR));				Serial.print(F(" "));
	Serial.print(EEPROM_int_read(E_DelayToRunControllPort + INPUT_BYTE_NUMBER_SENSOR*2));	Serial.print(F(" "));
	Serial.print(EEPROM.read(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR));					Serial.print(F(" "));
	Serial.print(EEPROM_int_read(E_ConfigSensor_B + INPUT_BYTE_NUMBER_SENSOR*2));			Serial.print(F(" "));
	Serial.print(EEPROM_int_read(E_ConfigSensor_C + INPUT_BYTE_NUMBER_SENSOR*2));			Serial.print(F(" "));
	Serial.print(EEPROM_int_read(E_ConfigSensor_D + INPUT_BYTE_NUMBER_SENSOR*2));			Serial.print(F(" "));
	for(byte i = 0; i <= 19; i++){
		NameSymbol = EEPROM.read((E_NameSensor + INPUT_BYTE_NUMBER_SENSOR-1)+i+20*(INPUT_BYTE_NUMBER_SENSOR-1));
		if(NameSymbol != 0){
			Serial.write(NameSymbol);														Serial.print(F(" "));
		}
		else{
			Serial.write(EmptySymbol);														Serial.print(F(" "));
		}
	}
	Serial.print(EEPROM.read(E_QuantityReadSensors		+	INPUT_BYTE_NUMBER_SENSOR));		Serial.print(F(" "));
	Serial.print(EEPROM.read(E_QuantityErrorReadSensor	+	INPUT_BYTE_NUMBER_SENSOR));		Serial.print(F(" "));
	Serial.print(EEPROM.read(E_QuantityErrors			+	INPUT_BYTE_NUMBER_SENSOR*2));	Serial.print(F(" "));
	Serial.print(EEPROM.read(E_ReactToMistakes_Ext		+	INPUT_BYTE_NUMBER_SENSOR));		Serial.print(F(" "));							
	Serial.print(EEPROM.read(E_ReactToMistakes_SD		+	INPUT_BYTE_NUMBER_SENSOR));		Serial.print(F(" "));
	Serial.print(EEPROM.read(E_ReactToMistakes_LED		+	INPUT_BYTE_NUMBER_SENSOR));		Serial.print(F(" "));
}


void WriteConfigSensors(){																// Сохранение данных из UART
	EEPROM.update(E_StatusSensor + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_STATUS_SENSOR);
	if(InputFromSerial0[1] == 0){														// Если прилетела команда выключить датчик
		byte Sensor = INPUT_BYTE_NUMBER_SENSOR;
		for(byte i = 0; i < 3; i ++){
			RealValueSensors[Sensor-1][i] = 0;											// Удаляем старые показания датчиков из массива RealValueSensors[][]
			SensorsError[Sensor-1][i] = 0;												// и затираем ошибки датчика
		}
	}
	// --------------------- Имя датчика ---------------------
	for(byte i = 0; i < 19; i ++){
		EEPROM.update((E_NameSensor + INPUT_BYTE_NUMBER_SENSOR-1)+i+20*(INPUT_BYTE_NUMBER_SENSOR-1), InputFromSerial0[25 + i]);
		NameSensor[INPUT_BYTE_NUMBER_SENSOR][i] = InputFromSerial0[25 + i];				// Сразу пишем имя в массив чтобы не надо было ребутить контроллер		
	}
	// ----------------- Наименование датчика ----------------
	if(1 <= INPUT_BYTE_TYPE_A_SENSOR && INPUT_BYTE_TYPE_A_SENSOR <= 13){				// Разрешенные значения от 1 до 13-и
		EEPROM.update(E_Type_A_Sensor + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_TYPE_A_SENSOR);
	}
	// ---------------- Тип измеряемых данных ----------------
	boolean AllowSaveETypeB = false;
	if(1 <= INPUT_BYTE_TYPE_B_SENSOR && INPUT_BYTE_TYPE_B_SENSOR <= 8){
		AllowSaveETypeB = true;
	}
	else if(INPUT_BYTE_TYPE_B_SENSOR == 12 ||
			INPUT_BYTE_TYPE_B_SENSOR == 15 ||
			INPUT_BYTE_TYPE_B_SENSOR == 25 ||
			INPUT_BYTE_TYPE_B_SENSOR == 125 ||
			INPUT_BYTE_TYPE_B_SENSOR == 67){
				AllowSaveETypeB = true;
			}
	if(AllowSaveETypeB){														// Если введенные данные в разрешенном диаппазоне
		EEPROM.update(E_Type_B_Sensor + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_TYPE_B_SENSOR);
	}
	// --------------- Привязка датчика к группе ----------------
	// Защита от дурака!!! нельзя привязывать одно и тоже значение датчика одновременно к разным группам управления
	// Запишется только первое из одинаковых параметров, остальные затрутся нулями!!!
	if(INPUT_BYTE_SGB_A == INPUT_BYTE_SGB_B && INPUT_BYTE_SGB_A == INPUT_BYTE_SGB_C){
		INPUT_BYTE_SGB_B = 0;
		INPUT_BYTE_SGB_C = 0;
	}
	else if(INPUT_BYTE_SGB_A == INPUT_BYTE_SGB_B){
		INPUT_BYTE_SGB_B = 0;
	}
	else if(INPUT_BYTE_SGB_A == INPUT_BYTE_SGB_B){
		INPUT_BYTE_SGB_C = 0;
	}
	else if(INPUT_BYTE_SGB_B == INPUT_BYTE_SGB_C){
		INPUT_BYTE_SGB_C = 0;
	}
	for(byte i = 0; i < 3; i ++){												// записываем полученные из UART данные в 3 байта конфигурации в EEPROM
		for(byte y = 1; y <= 16; y ++){											// но перед этим ищем во всех байтах конфигурации данные совпадающие с введенным из UART значением (защита от дурака)
			if(EEPROM.read(E_SBG + y*3 + i) == InputFromSerial0[5 + i]){		// и если находим
				EEPROM.update(E_SBG + y*3 + i, 0);								// то затираем ее нулями чтобы данные одного датчика не привязались двум разным группам
			}
		}
		EEPROM.update(E_SBG + (INPUT_BYTE_NUMBER_SENSOR*3) + i, InputFromSerial0[5 + i]);			// и теперь заливаем уже настройку в нужный байт
	}
	// ---------------- Адрес датчика ----------------
	switch(INPUT_BYTE_TYPE_A_SENSOR){
		case S_DS18B20:																					// Датчик DS18B20 (адрес 8 бит)
			for(byte i = 0; i < 8; i++){
				EEPROM.update(E_Address_Sensor + (10*INPUT_BYTE_NUMBER_SENSOR) + i, InputFromSerial0[i + 8]);
			}
			break;
		case S_ANALOG_SENSOR:																			// Аналоговый датчик (в качестве адреса записывается номер порта)
			if(AllowDigitalPort(INPUT_BYTE_ADDRESS_SENSOR)){											// Если порт находится в числе разрешенных
				CleaningDuplicatedPorts(INPUT_BYTE_ADDRESS_SENSOR);										// Проверяем чтобы порт не оказался настроен у другого датчика или модуля
				EEPROM.update(E_Address_Sensor + (10*INPUT_BYTE_NUMBER_SENSOR), INPUT_BYTE_ADDRESS_SENSOR);
			}
			break;
		case S_AHT25:	
			EEPROM.update(E_Address_Sensor + (10*INPUT_BYTE_NUMBER_SENSOR), 0x38);						// Пишем default адрес поскольку он может быть только один
			break;
		case S_HTU21D:
			EEPROM.update(E_Address_Sensor + (10*INPUT_BYTE_NUMBER_SENSOR), 0x40);						// Пишем default адрес поскольку он может быть только один
			break;
		default:																						// i2c датчик (адрес 1 бит)
			EEPROM.update(E_Address_Sensor + (10*INPUT_BYTE_NUMBER_SENSOR), INPUT_BYTE_ADDRESS_SENSOR);
	}
	// ---------------- Ручной или автоматический интервал опроса датчика ----------------
	if(0 <= INPUT_BYTE_ALLOW_MANUAL_INTERVAL_SENSOR && INPUT_BYTE_ALLOW_MANUAL_INTERVAL_SENSOR <= 1){						// Разрешенные значения 0 и 1
		EEPROM.update(E_AllowManualIntervalSensor + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_ALLOW_MANUAL_INTERVAL_SENSOR);
	}
	// ---------------- Интервал опроса датчика (сек)
	if(0 <= INPUT_BYTE_SENSOR_POLL_INTERVAL && INPUT_BYTE_SENSOR_POLL_INTERVAL <= 255){					 // Разрешенные значения от 0 до 255
		EEPROM.put(E_SensorPollInterval + INPUT_BYTE_NUMBER_SENSOR * 2, INPUT_BYTE_SENSOR_POLL_INTERVAL);
	}
	// ---------------- Вкл\выкл порт управления ----------------
	if(0 <= INPUT_BYTE_ALLOW_SENSOR_CONTROLL_PORT && INPUT_BYTE_ALLOW_SENSOR_CONTROLL_PORT <= 1){					// Разрешенные значения 0 и 1
		EEPROM.update(E_AllowSensorControllPort + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_ALLOW_SENSOR_CONTROLL_PORT);
	}
	// ---------------- Адрес порта управления датчика ----------------
	if(AllowDigitalPort(INPUT_BYTE_SENSOR_CONTROLL_PORT)){													// Если порт находится в числе разрешенных
		CleaningDuplicatedPorts(INPUT_BYTE_SENSOR_CONTROLL_PORT);											// Проверяем чтобы порт не оказался настроен у другого датчика или модуля
		EEPROM.update(E_SensorControllPort + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_SENSOR_CONTROLL_PORT);
	}
	// ---------------- Задержка для включения порта управления датчика (ms) ----------------
	if(0 <= INPUT_BYTE_DELAY_RUN_CONTROLL_PORT && INPUT_BYTE_DELAY_RUN_CONTROLL_PORT <= 65535){					// Разрешенные значения от 0 до 65536
		EEPROM.put(E_DelayToRunControllPort + INPUT_BYTE_NUMBER_SENSOR * 2, INPUT_BYTE_DELAY_RUN_CONTROLL_PORT);
	}
	// ---------------- Конфигурационный 'A' бит датчика -----------------
	switch(INPUT_BYTE_TYPE_A_SENSOR){
		case 1:							// DS18B20
			if(9 <= INPUT_BYTE_CONFIG_SENSOR_A && INPUT_BYTE_CONFIG_SENSOR_A <= 12){
				EEPROM.update(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_CONFIG_SENSOR_A);	// Разрешение датчика
			}
			break;
		case 7:							// Analog Port
			if(0 <= INPUT_BYTE_CONFIG_SENSOR_A && INPUT_BYTE_CONFIG_SENSOR_A <= 1){							
				EEPROM.update(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_CONFIG_SENSOR_A);	// Единицы измерения датчика
			}
			break;
		case 8:							// TSL2561
			if(1 <= INPUT_BYTE_CONFIG_SENSOR_A && INPUT_BYTE_CONFIG_SENSOR_A <= 3){
				EEPROM.update(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_CONFIG_SENSOR_A);	// Тип измеряемого спектра
			}
			break;
		case 9:							// BH1750
			if(0 <= INPUT_BYTE_CONFIG_SENSOR_A && INPUT_BYTE_CONFIG_SENSOR_A <= 6){
				EEPROM.update(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_CONFIG_SENSOR_A);	// Режим измерения
			}
			break;
		default:
			EEPROM.update(E_ConfigSensor_A + INPUT_BYTE_NUMBER_SENSOR, 0);							// По default записываем в ячейку нули
	}
	// ---------------- Конфигурационный 'B' бит датчика -----------------
	switch(INPUT_BYTE_TYPE_A_SENSOR){
		case 1:							// DS18B20
			if(1 <= INPUT_BYTE_CONFIG_SENSOR_B &&INPUT_BYTE_CONFIG_SENSOR_B <= 6){
				EEPROM.put(E_ConfigSensor_B + INPUT_BYTE_NUMBER_SENSOR*2, INPUT_BYTE_CONFIG_SENSOR_B);
			}
			break;
		case 7:							// Analog Port
			if(0 <= INPUT_BYTE_CONFIG_SENSOR_B && INPUT_BYTE_CONFIG_SENSOR_B <= 1024){
				EEPROM.put(E_ConfigSensor_B + INPUT_BYTE_NUMBER_SENSOR*2, INPUT_BYTE_CONFIG_SENSOR_B);
			}
			break;
// 		case 8:							// TSL2951
// 			if(0 <= INPUT_BLOODHOUND_CONFIG_SENSOR_B && INPUT_BLOODHOUND_CONFIG_SENSOR_B <= 1){
// 				EEPROM.put(E_ConfigSensor_B + INPUT_BLOODHOUND_NUMBER_SENSOR*2, INPUT_BLOODHOUND_CONFIG_SENSOR_B);
// 			}
// 			break;
		default:
			EEPROM.put(E_ConfigSensor_B + INPUT_BYTE_NUMBER_SENSOR*2, 0);							// По default записываем в ячейку нули
	}
	// ---------------- Конфигурационный 'C' бит датчика -----------------
	switch(INPUT_BYTE_TYPE_A_SENSOR){
		case 7:			// Analog Port
			if(0 <= INPUT_BYTE_CONFIG_SENSOR_C && INPUT_BYTE_CONFIG_SENSOR_C <= 1024){
				EEPROM.put(E_ConfigSensor_C + INPUT_BYTE_NUMBER_SENSOR*2, INPUT_BYTE_CONFIG_SENSOR_C);
			}
			break;
		case 8:			// TSL2951
			if(1 <= INPUT_BYTE_CONFIG_SENSOR_C && INPUT_BYTE_CONFIG_SENSOR_C <= 3){
				EEPROM.put(E_ConfigSensor_C + INPUT_BYTE_NUMBER_SENSOR*2, INPUT_BYTE_CONFIG_SENSOR_C);
			}
			break;
		default:
			EEPROM.put(E_ConfigSensor_C + INPUT_BYTE_NUMBER_SENSOR*2, 0);							// По default записываем в ячейку нули
	}
	// ---------------- Конфигурационный 'D' бит датчика -----------------
	switch(INPUT_BYTE_TYPE_A_SENSOR){
		case 8:			// TSL2951
			if(1 <= INPUT_BYTE_CONFIG_SENSOR_D && INPUT_BYTE_CONFIG_SENSOR_D <= 3){
				EEPROM.put(E_ConfigSensor_D + INPUT_BYTE_NUMBER_SENSOR*2, INPUT_BYTE_CONFIG_SENSOR_D);
			}
			break;
		default:
			EEPROM.put(E_ConfigSensor_D + INPUT_BYTE_NUMBER_SENSOR*2, 0);										// По default записываем в ячейку нули
	}
	// ---------------- Количество измерений показаний -----------------
	EEPROM.update(E_QuantityReadSensors + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_QUANTITY_READ_SENSORS);			// Количество измерений показаний (для увеличения точности)
	// ---------------- Количество ошибок чтения -----------------
	EEPROM.update(E_QuantityErrorReadSensor + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_QUANTITY_ERROR_READ_SENSOR);	// Количество ошибок чтения, после которой идет реакция
	
	// ----------------------------------------------------
	// ----------------------------------------------------
	// ---------------- Реакция на ошибки -----------------
	if(0 <= INPUT_BYTE_REACT_TO_MISTAKES_EXT && INPUT_BYTE_REACT_TO_MISTAKES_EXT <= 3){															// Разрешенное значение от 0, до 3
		EEPROM.update(E_ReactToMistakes_Ext + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_REACT_TO_MISTAKES_EXT);		
	}
	if(0 <= INPUT_BYTE_REACT_TO_MISTAKES_SD && INPUT_BYTE_REACT_TO_MISTAKES_SD <= 1){						// Разрешенные значения 0 и 1
		EEPROM.update(E_ReactToMistakes_SD + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_REACT_TO_MISTAKES_SD);
	}
	if(0 <= InputFromSerial0[49] && INPUT_BYTE_REACT_TO_MISTAKES_LED <= 5){									// Разрешенные значения от 0 до 5
		EEPROM.update(E_ReactToMistakes_LED + INPUT_BYTE_NUMBER_SENSOR, INPUT_BYTE_REACT_TO_MISTAKES_LED);	
	}	
	// ---------------- ---------------- ---------------- ----------------
	Serial.println();
	Serial.print(F("Sensor ")); Serial.print(INPUT_BYTE_NUMBER_SENSOR); Serial.println(F(" is configuring in EEPROM"));
}


void RecievedSensors(){
	boolean flag;
	for (byte i = 2; i < sizeof(InputFromSerial0)/sizeof(int); i++){
		if (InputFromSerial0[i] == 0){
			flag = false;
		}
		else{
			flag = true;
			break;
		}
	}
	
	if(1 <= INPUT_BYTE_NUMBER_SENSOR && INPUT_BYTE_NUMBER_SENSOR <= QUANTITY_SENSORS || INPUT_BYTE_NUMBER_SENSOR == 255){
		if (flag){
			if(INPUT_BYTE_NUMBER_SENSOR != 255){
				WriteConfigSensors();				// Сохраняем конфигурацию
			}
		}
		else{
			switch (InputFromSerial0[1]){
				case 2:								// Сброс ошибок чтения показаний датчиком
					EEPROM.put(E_QuantityErrors + INPUT_BYTE_NUMBER_SENSOR*2, 0);
					Serial.println(F("Errors is cleaning!!!"));
					break;
				case 1:								// Вывод конфигации датчика в UART со всеми описаниями
					if(INPUT_BYTE_NUMBER_SENSOR != 255){
						Serial.print(F("s "));
						SentConfigSensorsExtApp();
						Serial.println();
					}
					else{												// Выводим полную инфу для отладки
						for(byte _Sensor = 1; _Sensor <= QUANTITY_SENSORS; _Sensor ++){
							Serial.print(F("s "));
							INPUT_BYTE_NUMBER_SENSOR = _Sensor;							
							SentConfigSensorsExtApp();
						}
						Serial.println();
					}
					break;
				case 0:													// Выводим только байты конфигурации
					if(INPUT_BYTE_NUMBER_SENSOR != 255){
						SentConfigSensorsUART();
					}
					else{												// Выводим полную инфу для отладки
						for(byte Sensor = 1; Sensor <= QUANTITY_SENSORS; Sensor ++){
							INPUT_BYTE_NUMBER_SENSOR = Sensor;
							SentConfigSensorsUART();
						}
					}
					break;
			}
		}
	}
	CleanInputFromSerial0();
	recievedFlag_sensors = false;
	flag = false;
}