#include <./Arduino.h>
#include <./EEPROM.h>

#include "RecievedSensors.h"
#include "../EEPROM_ADR.h"
#include "../ParsingDataOnSerial.h"
#include "../Sensors.h"
#include "../Configuration.h"
#include "../DigitalPorts.h"
#include "RegisterSensors.h"


void SentConfigSensorsUART(){
	Serial.print(F("========== Конфигурация датчика '")); Serial.print(InputFromSerial0[0]); Serial.println(F("' =========="));
	// ========================================================
	Serial.print(F("Статус: "/*"Status: "*/));
	if(EEPROM.read(E_StatusSensor + InputFromSerial0[0]) == 1){
		Serial.println(F("Включен"/*"on"*/));
	}
	else{
		Serial.println(F("Выключен"/*"off"*/));
	}
	// ========================================================
	Serial.print(F("Имя: "/*"Name: "*/));
	for(byte i = 0; i <= 19; i++){
		byte Symbol = EEPROM.read(E_NameSensor + (InputFromSerial0[0] - 1) * 20 + i);
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
		if(EEPROM.read(E_SBG + InputFromSerial0[0]*3 + i) != 0){
			Serial.print(F(" Значение_"/*" Value_"*/)); Serial.print(i + 1); Serial.print(F(": ")); Serial.print(EEPROM.read(E_SBG + (InputFromSerial0[0]*3) + i)); Serial.print(F("\t"));
		}
		if(i == 2){
			Serial.println();
		}
	}
	// ========================================================
	Serial.print(F("Ошибки показаний: "/*"Errors: "*/));
	for(byte i = 0; i < 3; i ++){
		Serial.print(SensorsError[InputFromSerial0[0]-1][i]); Serial.print(F(" "));
	}
	Serial.println();
	// ========================================================
	Read_ETAS_register(InputFromSerial0[0]);							// Выводим наименование и адрес датчика
	if(EEPROM.read(E_Type_A_Sensor + InputFromSerial0[0]) != 7){		// Если датчик не настроен как аналоговый
		Read_E_Type_B_Sensor_register(InputFromSerial0[0], 0);				// Выводим тип измеряемых данных
	}
	// ========================================================
	
	// ========================================================
	Serial.print(F("Интервал измерения показаний: "/*"Sensor poll interval: "*/));
	if(EEPROM.read(E_AllowManualIntervalSensor + InputFromSerial0[0]) == 1){		// Если включен ручной интервал опроса датчика
		Serial.println(F("Ручной"/*"manual"*/));
		Serial.print(F("\tИнтервал измерения: "/*"Interval poll: "*/)); Serial.print(EEPROM_int_read(E_SensorPollInterval + InputFromSerial0[0] * 2)); Serial.println(F("сек"/*"sec"*/));
	}
	else Serial.println(F("Автоматический"/*"automatic"*/));
	// ========================================================
	Serial.print(F("Использование порта управления: "/*"Allow the control port: "*/));
	if(EEPROM.read(E_AllowSensorControllPort + InputFromSerial0[0]) == 1){		// Если включен порт управления
		Serial.println(F("Включено"/*"on"*/));
		Serial.print(F("\tПорт управления: "/*"Control Port: "*/));
		if(DigitalPort(EEPROM.read(E_SensorControllPort + InputFromSerial0[0]), 0, 1) != 255){
			Serial.println();
		}		
		else Serial.println(F("Порт не сконфигурирован!!!"/*"The port isn't configured"*/));	
		Serial.print(F("\tЗадержка после включения порта: "/*"Delay for run the control port: "*/)); Serial.print(EEPROM_int_read(E_DelayToRunControllPort + (InputFromSerial0[0]*2))); Serial.println(F("ms"));	
	}
	else Serial.println(F("Выключено"/*"off"*/));
	
	// ========================================================	
	switch(EEPROM.read(E_Type_A_Sensor + InputFromSerial0[0])){
		case 1:																			// Если DS18B20
			Serial.print(F("Разрешение датчика: ")); Serial.print(EEPROM.read(E_ConfigSensor_A + InputFromSerial0[0])); Serial.println(F("бит"));
			break;
		case 4:																			// Если BMP280
			break;
		case 5:																			// Если BME280
			break;
		case 7:																			// Analog port
			Serial.print(F("Единицы измерения: "/*"\tUnits of measure: "*/));			// Выводим единицы измерения датчика
			switch(EEPROM.read(E_ConfigSensor_A + InputFromSerial0[0])){
				case 0:
					Serial.println(F("Проценты"/*"Percent"*/));							// Проценты
					break;
				case 1:
					Serial.println(F("Значение АЦП"/*"Absolute values"*/));				// Аналоговые значения
					break;	
				default:
					Serial.println();
			}
			if(EEPROM.read(E_ConfigSensor_A + InputFromSerial0[0]) == 0){				// Если измерения идут в процентах
				if(EEPROM.read(E_ConfigSensor_A + InputFromSerial0[0]) == 0){
					Serial.print(F("Значение АЦП для 0%: "/*"The value for 0% "*/)); Serial.println(EEPROM_int_read(E_ConfigSensor_B + InputFromSerial0[0]*2));
				}
				if(EEPROM.read(E_ConfigSensor_A + InputFromSerial0[0]) == 0){
					Serial.print(F("Значение АЦП для 100%: "/*"The value for 100% "*/)); Serial.println(EEPROM_int_read(E_ConfigSensor_C + InputFromSerial0[0]*2));
				}
			}
			break;
		case 8:																			// Если датчик TSL2561
			Serial.print(F("Спектр измерений: "));
			switch(EEPROM.read(E_ConfigSensor_A + InputFromSerial0[0])){
				case 1:
					Serial.println(F("Видимый спектр"/*"Visible spectrum"*/));			// Видимый спектр
					break;
				case 2:
					Serial.println(F("Полный спектр"/*"Full spectrum"*/));				// Полный спектр
					break;
				case 3:
					Serial.println(F("IR спектр"/*"IR spectrum"*/));					// IR спектр
					break;
				default:
					Serial.println();
			}
			Serial.print(F("Единицы измерения: "/*"Units of measure: "*/));
			switch(EEPROM_int_read(E_ConfigSensor_B + InputFromSerial0[0]*2)){			// Выводим единицы измерения освещенности
				case 0:
					Serial.println(F("Люмены"/*"Lum"*/));
					break;
				case 1:
					Serial.println(F("Люксы"/*"Lux"*/));
					break;
				default:
					Serial.println(F("Не настроено (default: Люмены)"));
			}
			Serial.print(F("Величина усиления: "));
			switch(EEPROM_int_read(E_ConfigSensor_C + InputFromSerial0[0]*2)){			// Выводим единицы измерения освещенности
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
			switch(EEPROM_int_read(E_ConfigSensor_D + InputFromSerial0[0]*2)){			// Время конвертации
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
		case 10:																		// Если датчик MAX44009
			break;
	}		
	// ========================================================
	Serial.print(F("Количество измерений до фиксации показаний: "/*"Quantity of measurements: "*/));						// Количество измерений пока они не запишутся в основной массив (для увеличения точности измерения)
	Serial.println(EEPROM.read(E_QuantityReadSensors + InputFromSerial0[0]));
	// ========================================================
	Serial.print(F("Количество ошибок измерения показаний: "/*"The current quantity of errors: "*/));						// Количество ошибочных измерений (сбрасывается командой "s 'номер датчика' 2")
	Serial.println(EEPROM_int_read(E_QuantityErrors + (InputFromSerial0[0]*2)));
	// ========================================================
	Serial.print(F("Количество ошибочных измерений до реакции: "/*"The maximum quantity of errors for reaction: "*/));		// Количество ошибочных измерений до реакции
	Serial.println(EEPROM.read(E_QuantityErrorReadSensor + InputFromSerial0[0]));	
	//========================================================
	Serial.println(F("Реакция на ошибки: "/*"Reaction to mistakes: "*/));
	Serial.print(F("\t\tВнешние: "/*"\tExt mail: "*/));
	switch(EEPROM.read(E_ReactToMistakes_Ext + InputFromSerial0[0])){
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
		switch(EEPROM.read(E_ReactToMistakes_SD + InputFromSerial0[0])){
			case 0:
				Serial.println(F("Выключена"/*"off"*/));
				break;
			case 1:
				Serial.println(F("Включена"/*"on"*/));			
				break;
			default:
				Serial.println();
		}
	}
	else (Serial.println(F("Отключена глобально!!!")));
	
	Serial.print(F("\t\tLED панель: "/*"\t\t\tLED: "*/));
	switch(EEPROM.read(E_ReactToMistakes_LED + InputFromSerial0[0])){
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
	byte EmptySymbol = 35;				// Символ '#'
	Serial.print(InputFromSerial0[0]);													Serial.print(F(" "));
	Serial.print(F("0"));																Serial.print(F(" "));
	Serial.print(EEPROM.read(E_StatusSensor + InputFromSerial0[0]));					Serial.print(F(" "));
	Serial.print(EEPROM.read(E_Type_A_Sensor + InputFromSerial0[0]));					Serial.print(F(" "));
	Serial.print(EEPROM.read(E_Type_B_Sensor + InputFromSerial0[0]));					Serial.print(F(" "));
	Serial.print(EEPROM.read(E_SBG + (InputFromSerial0[0]*3) + 0));						Serial.print(F(" "));
	Serial.print(EEPROM.read(E_SBG + (InputFromSerial0[0]*3) + 1));						Serial.print(F(" "));
	Serial.print(EEPROM.read(E_SBG + (InputFromSerial0[0]*3) + 2));						Serial.print(F(" "));
	for(byte i = 0; i < 8; i++){
		Serial.print(EEPROM.read(E_Address_Sensor + (10*InputFromSerial0[0]) + i));		Serial.print(F(" "));
	}
	Serial.print(EEPROM.read(E_AllowManualIntervalSensor + InputFromSerial0[0]));		Serial.print(F(" "));
	Serial.print(EEPROM_int_read(E_SensorPollInterval + InputFromSerial0[0] * 2));		Serial.print(F(" "));
	Serial.print(EEPROM.read(E_AllowSensorControllPort + InputFromSerial0[0]));			Serial.print(F(" "));
	Serial.print(EEPROM.read(E_SensorControllPort + InputFromSerial0[0]));				Serial.print(F(" "));
	Serial.print(EEPROM_int_read(E_DelayToRunControllPort + (InputFromSerial0[0]*2)));	Serial.print(F(" "));
	Serial.print(EEPROM.read(E_ConfigSensor_A + InputFromSerial0[0]));					Serial.print(F(" "));
	Serial.print(EEPROM_int_read(E_ConfigSensor_B + InputFromSerial0[0]*2));			Serial.print(F(" "));
	Serial.print(EEPROM_int_read(E_ConfigSensor_C + InputFromSerial0[0]*2));			Serial.print(F(" "));
	Serial.print(EEPROM_int_read(E_ConfigSensor_D + InputFromSerial0[0]*2));			Serial.print(F(" "));
	for(byte i = 0; i <= 19; i++){
		NameSymbol = EEPROM.read(E_NameSensor + (InputFromSerial0[0] - 1) * 20 + i);
		if(NameSymbol != 0){
			Serial.write(NameSymbol);													Serial.print(F(" "));
		}
		else{
			Serial.write(EmptySymbol);													Serial.print(F(" "));
		}
	}
	Serial.print(EEPROM.read(E_QuantityReadSensors + InputFromSerial0[0]));				Serial.print(F(" "));
	Serial.print(EEPROM.read(E_QuantityErrorReadSensor + InputFromSerial0[0]));			Serial.print(F(" "));
	Serial.print(EEPROM_int_read(E_QuantityErrors + (InputFromSerial0[0]*2)));			Serial.print(F(" "));
	Serial.print(EEPROM.read(E_ReactToMistakes_Ext + InputFromSerial0[0]));				Serial.print(F(" "));							
	Serial.print(EEPROM.read(E_ReactToMistakes_SD + InputFromSerial0[0]));				Serial.print(F(" "));
	Serial.print(EEPROM.read(E_ReactToMistakes_LED + InputFromSerial0[0]));				Serial.print(F(" "));
}


void WriteConfigSensors(){							// Сохранение данных из UART
	EEPROM.update(E_StatusSensor + InputFromSerial0[0], InputFromSerial0[2]);
	if(InputFromSerial0[1] == 0){					// Если прилетела команда выключить датчик
		byte Sensor = InputFromSerial0[0];
		for(byte i = 0; i < 3; i ++){
			RealValueSensors[Sensor-1][i] = 0;				// Удаляем старые показания датчиков из массива RealValueSensors[][]
			SensorsError[Sensor-1][i] = 0;					// и затираем ошибки датчика
		}
	}
	// --------------------- Имя датчика ---------------------
	for(byte i = 0; i < 19; i ++){
		EEPROM.update(E_NameSensor + ((InputFromSerial0[0] - 1) * 20) + i, InputFromSerial0[25 + i]);
		NameSensor[InputFromSerial0[0]-1][i] = InputFromSerial0[24 + i];	// Сразу пишем имя в массив чтобы не надо было ребутить контроллер
	}
	// ----------------- Наименование датчика ----------------
	if(1 <= InputFromSerial0[3] && InputFromSerial0[3] <= 12){		// Разрешенные значения от 1 до 12-и
		EEPROM.update(E_Type_A_Sensor + InputFromSerial0[0], InputFromSerial0[3]);
	}
	// ---------------- Тип измеряемых данных ----------------
	boolean AllowSaveETypeB = false;
	if(1 <= InputFromSerial0[4] && InputFromSerial0[4] <= 8){
		AllowSaveETypeB = true;
	}
	else if(InputFromSerial0[4] == 12 ||
			InputFromSerial0[4] == 15 ||
			InputFromSerial0[4] == 125 ||
			InputFromSerial0[4] == 67){
				AllowSaveETypeB = true;
			}
	if(AllowSaveETypeB){														// Если введенные данные в разрешенном диаппазоне
		EEPROM.update(E_Type_B_Sensor + InputFromSerial0[0], InputFromSerial0[4]);
	}
	// --------------- Привязка датчика к группе ----------------
	// Защита от дурака!!! нельзя привязывать одно и тоже значение датчика одновременно к разным группам управления
	// Запишется только первое из одинаковых параметров, остальные затрутся нулями!!!
	if(InputFromSerial0[5] == InputFromSerial0[6] && InputFromSerial0[5] == InputFromSerial0[7]){
		InputFromSerial0[6] = 0;
		InputFromSerial0[7] = 0;
	}
	else if(InputFromSerial0[5] == InputFromSerial0[6]){
		InputFromSerial0[6] = 0;
	}
	else if(InputFromSerial0[5] == InputFromSerial0[6]){
		InputFromSerial0[7] = 0;
	}
	else if(InputFromSerial0[6] == InputFromSerial0[7]){
		InputFromSerial0[7] = 0;
	}
	for(byte i = 0; i < 3; i ++){												// записываем полученные из UART данные в 3 байта конфигурации в EEPROM
		for(byte y = 1; y <= 16; y ++){											// но перед этим ищем во всех байтах конфигурации данные совпадающие с введенным из UART значением (защита от дурака)
			if(EEPROM.read(E_SBG + y*3 + i) == InputFromSerial0[5 + i]){		// и если находим
				EEPROM.update(E_SBG + y*3 + i, 0);								// то затираем ее нулями чтобы данные одного датчика не привязались двум разным группам
			}
		}
		EEPROM.update(E_SBG + (InputFromSerial0[0]*3) + i, InputFromSerial0[5 + i]);// и теперь заливаем уже настройку в нужный байт
	}
	// ---------------- Адрес датчика ----------------
	switch(InputFromSerial0[3]){
		case 1:																	// Датчик DS18B20 (адрес 8 бит)
			for(byte i = 0; i < 8; i++){
				EEPROM.update(E_Address_Sensor + (10*InputFromSerial0[0]) + i, InputFromSerial0[i + 8]);
			}
			break;
		case 7:																	// Аналоговый датчик (в качестве адреса записывается номер порта)
			if(AllowDigitalPort(InputFromSerial0[8])){					// Если порт находится в числе разрешенных
				CleaningDuplicatedPorts(InputFromSerial0[8]);			// Проверяем чтобы порт не оказался настроен у другого датчика или модуля
				EEPROM.update(E_Address_Sensor + (10*InputFromSerial0[0]), InputFromSerial0[8]);
			}
			break;
		default:																// i2c датчик (адрес 1 бит)
			EEPROM.update(E_Address_Sensor + (10*InputFromSerial0[0]), InputFromSerial0[8]);
	}
	// ---------------- Ручной или автоматический интервал опроса датчика ----------------
	if(0 <= InputFromSerial0[16] && InputFromSerial0[16] <= 1){		// Разрешенные значения 0 и 1
		EEPROM.update(E_AllowManualIntervalSensor + InputFromSerial0[0], InputFromSerial0[16]);
	}
	// ---------------- Интервал опроса датчика (сек)
	if(0 <= InputFromSerial0[17] && InputFromSerial0[17] <= 255){		// Разрешенные значения от 0 до 255
		EEPROM_int_write(E_SensorPollInterval + (InputFromSerial0[0] * 2), InputFromSerial0[17]);
	}
	// ---------------- Вкл\выкл порт управления ----------------
	if(0 <= InputFromSerial0[18] && InputFromSerial0[18] <= 1){		// Разрешенные значения 0 и 1
		EEPROM.update(E_AllowSensorControllPort + InputFromSerial0[0], InputFromSerial0[18]);
	}
	// ---------------- Адрес порта управления датчика ----------------
	if(AllowDigitalPort(InputFromSerial0[19])){								// Если порт находится в числе разрешенных
		CleaningDuplicatedPorts(InputFromSerial0[19]);						// Проверяем чтобы порт не оказался настроен у другого датчика или модуля
		EEPROM.update(E_SensorControllPort + InputFromSerial0[0], InputFromSerial0[19]);
	}
	// ---------------- Задержка для вкдлючения порта управления датчика (ms) ----------------
	if(0 <= InputFromSerial0[20] && InputFromSerial0[20] <= 65535){		// Разрешенные значения от 0 до 65536
		EEPROM_int_write(E_DelayToRunControllPort + InputFromSerial0[0] * 2, InputFromSerial0[20]);
	}
	// ---------------- Конфигурационный 'A' бит датчика -----------------
	switch(InputFromSerial0[3]){
		case 1:			// DS18B20
			if(9 <= InputFromSerial0[21] && InputFromSerial0[21] <= 12){
				EEPROM.update(E_ConfigSensor_A + InputFromSerial0[0], InputFromSerial0[21]);	// Разрешение датчика
			}
			break;
		case 7:			// Analog Port
			if(0 <= InputFromSerial0[21] && InputFromSerial0[21] <= 1){							
				EEPROM.update(E_ConfigSensor_A + InputFromSerial0[0], InputFromSerial0[21]);	// Единицы измерения датчика
			}
			
			EEPROM_int_write(E_ConfigSensor_C + InputFromSerial0[0]*2, InputFromSerial0[23]);
			break;
		case 8:			// TSL2561
			if(1 <= InputFromSerial0[21] && InputFromSerial0[21] <= 3){
				EEPROM.update(E_ConfigSensor_A + InputFromSerial0[0], InputFromSerial0[21]);	// Тип измеряемого спектра
			}
			break;
	}
	// ---------------- Конфигурационный 'B' бит датчика -----------------
	switch(InputFromSerial0[3]){
		case 7:			// Analog Port
			if(0 <= InputFromSerial0[22] && InputFromSerial0[22] <= 1024){
				EEPROM_int_write(E_ConfigSensor_B + InputFromSerial0[0]*2, InputFromSerial0[22]);
			}
			break;
		case 8:			// TSL2951
			if(0 <= InputFromSerial0[22] && InputFromSerial0[22] <= 1){
				EEPROM_int_write(E_ConfigSensor_B + InputFromSerial0[0]*2, InputFromSerial0[22]);
			}
			break;
	}
	// ---------------- Конфигурационный 'C' бит датчика -----------------
	switch(InputFromSerial0[3]){
		case 7:			// Analog Port
			if(0 <= InputFromSerial0[23] && InputFromSerial0[23] <= 1024){
				EEPROM_int_write(E_ConfigSensor_C + InputFromSerial0[0]*2, InputFromSerial0[23]);
			}
			break;
		case 8:			// TSL2951
			if(1 <= InputFromSerial0[23] && InputFromSerial0[23] <= 3){
				EEPROM_int_write(E_ConfigSensor_C + InputFromSerial0[0]*2, InputFromSerial0[23]);
			}
			break;
	}
	// ---------------- Конфигурационный 'D' бит датчика -----------------
	switch(InputFromSerial0[3]){
		case 8:			// TSL2951
			if(1 <= InputFromSerial0[23] && InputFromSerial0[23] <= 3){
				EEPROM_int_write(E_ConfigSensor_D + InputFromSerial0[0]*2, InputFromSerial0[24]);
			}
			break;
	}
	// ---------------- Количество измерений показаний -----------------
	EEPROM.update(E_QuantityReadSensors + InputFromSerial0[0], InputFromSerial0[44]);			// Количество измерений показаний (для увеличения точности)
	// ---------------- Количество ошибок чтения -----------------
	EEPROM.update(E_QuantityErrorReadSensor + InputFromSerial0[0], InputFromSerial0[45]);		// Количество ошибок чтения, после которой идет реакция
	// ----------------------------------------------------
	// ----------------------------------------------------
	// ---------------- Реакция на ошибки -----------------
	if(0 <= InputFromSerial0[47] && InputFromSerial0[47] <= 3){									// Разрешенное значение от 0, до 3
		EEPROM.update(E_ReactToMistakes_Ext + InputFromSerial0[0], InputFromSerial0[47]);		
	}
	if(0 <= InputFromSerial0[48] && InputFromSerial0[48] <= 1){									// Разрешенные значения 0 и 1
		EEPROM.update(E_ReactToMistakes_SD + InputFromSerial0[0], InputFromSerial0[47]);
	}
	if(0 <= InputFromSerial0[49] && InputFromSerial0[49] <= 5){									// Разрешенные значения от 0 до 5
		EEPROM.update(E_ReactToMistakes_LED + InputFromSerial0[0], InputFromSerial0[48]);	
	}	
	// ---------------- ---------------- ---------------- ----------------
	Serial.print(F("Sensor ")); Serial.print(InputFromSerial0[0]); Serial.println(F(" is configuring in EEPROM          "));
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
	
	if(1 <= InputFromSerial0[0] && InputFromSerial0[0] <= QuantitySensors || InputFromSerial0[0] == 255){
		if (flag){
			if(InputFromSerial0[0] != 255){
				WriteConfigSensors();				// Сохраняем конфигурацию
			}
		}
		else{
			switch (InputFromSerial0[1]){
				case 2:								// Сброс ошибок чтения показаний датчиком
					EEPROM_int_write(E_QuantityErrors + InputFromSerial0[0]*2, 0);
					Serial.println(F("Errors is cleaning!!!"));
					break;
				case 1:								// Вывод конфигации датчика в UART со всеми описаниями
					if(InputFromSerial0[0] != 255){
						Serial.print(F("s "));
						SentConfigSensorsExtApp();
						Serial.println();
					}
					else{												// Выводим полную инфу для отладки
						//Serial.print(F("s "));
						for(byte Sensor = 1; Sensor <= QuantitySensors; Sensor ++){
							Serial.print(F("s "));
							InputFromSerial0[0] = Sensor;							
							SentConfigSensorsExtApp();
						}
						Serial.println();
					}
					break;
				case 0:													// Выводим только байты конфигурации
					if(InputFromSerial0[0] != 255){
						SentConfigSensorsUART();
					}
					else{												// Выводим полную инфу для отладки
						for(byte Sensor = 1; Sensor <= QuantitySensors; Sensor ++){
							InputFromSerial0[0] = Sensor;
							SentConfigSensorsUART();
						}
					}
					break;
			}
		}
	}
// 	for (byte i=0; i<sizeof(InputFromSerial0)/sizeof(int); i++){			// Затираем массив после работы
// 		InputFromSerial0[i] = 0;
// 	}
	CleanInputFromSerial0();
	recievedFlag_sensors = false;
	flag = false;
}