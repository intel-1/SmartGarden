#include <./Arduino.h>
#include <./EEPROM.h>

#include "RecievedConfigController.h"
#include "../ParsingDataOnSerial.h"
#include "../EEPROM_ADR.h"
#include "../Configuration.h"
#include "../Sensors.h"
#include "RecievedDebug.h"


// ========================================================================================
// =========================== Конфигурация контроллера ===================================
// ========================================================================================
void RecievedConfigController(){
	boolean flag;
	for (byte i = 1; i<sizeof(InputFromSerial0)/sizeof(int); i ++){
		if (InputFromSerial0[i] == 0){
			flag = false;
		}
		else{
			flag = true;
			break;
		}
	}
	
	if (flag){
		EEPROM.update(E_ControllVCC, InputFromSerial0[1]);							// Контролировать или нет напряжение питания
		// -----------------------------------------------------------------------------------------------
		EEPROM.update(E_MinInputVCC, InputFromSerial0[2]);							// Минимальное напряжение питания		
		// -----------------------------------------------------------------------------------------------
		EEPROM.update(E_SentSMSorStartController, InputFromSerial0[3]);				// Отправлять или нет СМС о старте контроллера (Да\Нет, 1\0)
		// -----------------------------------------------------------------------------------------------
		if(0 <= InputFromSerial0[4] || InputFromSerial0[4] <= 5){					// Разрешенное значение 0-5
			EEPROM.write(E_LOGING_TO_SERIAL, InputFromSerial0[4]);					// Уровень логирования в Serial
		}
		// -----------------------------------------------------------------------------------------------
		if(0 <= InputFromSerial0[5] || InputFromSerial0[5] <= 3){					// Разрешенное значение 0-3
			EEPROM.update(E_ENABLE_LOGING_TO_SD, InputFromSerial0[5]);				// Глобальная настройка. Писать на SD карту или нет.
		}
		// -----------------------------------------------------------------------------------------------
		//EEPROM.update(E_ModeController, InputFromSerial0[8]);						// Автоматический\ручной режим работы контроллера
		// -----------------------------------------------------------------------------------------------
		EEPROM.update(E_ConfigPowerBluetooth, InputFromSerial0[9]);					// Режим подачи питания на Bluetooth модуль
		// -----------------------------------------------------------------------------------------------
		EEPROM.put(E_BluetoothTimeOff, InputFromSerial0[10]);						// (тип int) Время отключения Bluetooth модуля при бездействии (сек)
		// -----------------------------------------------------------------------------------------------
		EEPROM.put(E_LoopLCDLightTime, InputFromSerial0[11]);						// Интервал выключения подсветки экрана
		// -----------------------------------------------------------------------------------------------
		EEPROM.put(E_LoopSensorTime, InputFromSerial0[12]);							// Интервал измерения показаний датчиков
		// -----------------------------------------------------------------------------------------------
		EEPROM.put(E_LoopWriteValueSensorOnSD, InputFromSerial0[13]);				// Интервал логирования данных на карту памяти
		// -----------------------------------------------------------------------------------------------
		EEPROM.put(E_LoopChannelRun, InputFromSerial0[14]);							// Интервал включения управления группами
		// -----------------------------------------------------------------------------------------------
		if(0 <= InputFromSerial0[15] || InputFromSerial0[15] <= 3 || InputFromSerial0[15] == 13){	// Разрешенные значения 0-3 или 23
			EEPROM.update(E_ReactToMinVCC, InputFromSerial0[15]);					// Реакция на понижение напряжения питания
		}
		// -----------------------------------------------------------------------------------------------
		if(0 <= InputFromSerial0[16] || InputFromSerial0[16] <= 1){					// Разрешенное значение 0-1
			EEPROM.update(E_GSM_STATE_MIN_VCC, InputFromSerial0[16]);				// Что делать с GSM модулем есть питание контроллера ниже минимального (0 - выключен, 1 - включен)
		}
		else EEPROM.update(E_GSM_STATE_MIN_VCC, 1);									// По умолчанию "1"
		// -----------------------------------------------------------------------------------------------
		if(0 <= InputFromSerial0[17] || InputFromSerial0[17] <= 254){				// Разрешенные значения 0-254
			EEPROM.update(E_ResetTimeGSM, InputFromSerial0[17]);					// Время перезагрузки модуля
		}
		// -----------------------------------------------------------------------------------------------
		if(1 <= InputFromSerial0[18] || InputFromSerial0[18] <= 10){				// Разрешенные значения 1-10
			EEPROM.update(E_MaximumTimeResponseGSM, InputFromSerial0[18]);			// Максимальное время ожидания ответа от модуля
		}
		else EEPROM.update(E_MaximumTimeResponseGSM, 10);							// По умолчанию "10"
		// -----------------------------------------------------------------------------------------------
		if(0 <= InputFromSerial0[19] || InputFromSerial0[19] <= 254){				// Разрешенные значения 0-254
			EEPROM.update(E_IntervalCheckRegistrationGSM, InputFromSerial0[19]);	// Интервал проверки регистрации GSM
		}
		// -----------------------------------------------------------------------------------------------
		if(0 <= InputFromSerial0[20] || InputFromSerial0[20] <= 2){					// Разрешенное значение 0-2
			EEPROM.write(E_WorkSIM800, InputFromSerial0[20]);						// Режим питания SIM800
			if(InputFromSerial0[20] == 1){
				gsm_vcc_on();
			}
			else gsm_vcc_off(); 
		}
		// -----------------------------------------------------------------------------------------------
		if(0 <= InputFromSerial0[21] || InputFromSerial0[21] <= 1){					// Разрешенное значение 0-1
			EEPROM.write(E_AllowGPRS, InputFromSerial0[21]);						// Режим работы GPRS
		}
		// -----------------------------------------------------------------------------------------------
		if(0 <= InputFromSerial0[22] || InputFromSerial0[22] <= 1){					// Разрешенное значение 0-1
			EEPROM.update(E_Mode_OS_INT_LM75, InputFromSerial0[22]);				// Режим работы встроенное выходного порта
		}
		// -----------------------------------------------------------------------------------------------
		if(0 <= InputFromSerial0[23] || InputFromSerial0[23] <= 254){				// Разрешенное значение 0-254
			EEPROM.update(E_INT_LM75_TOS, InputFromSerial0[23]);					// Значение бита TOS встроенного LM75
			LM75_Tos(ADDRESS_INPUT_LM75, InputFromSerial0[23]);						// Обновляем данные в датчике температуры
		}
		// -----------------------------------------------------------------------------------------------
		if(0 <= InputFromSerial0[24] || InputFromSerial0[24] <= 254){				// Разрешенное значение 0-254
			EEPROM.update(E_INT_LM75_THYST, InputFromSerial0[24]);					// Значение бита Thyst встроенного LM75
			LM75_Thyst(ADDRESS_INPUT_LM75, InputFromSerial0[24]);					// Обновляем данные в датчике температуры
		}	
		// -----------------------------------------------------------------------------------------------	
		for(byte i = 0; i <= 18; i ++){
			EEPROM.write(E_NameController + i, InputFromSerial0[25 + i]);			// Имя контроллера
		}
		// -----------------------------------------------------------------------------------------------
		Serial.println();
		Serial.println(F("Конфигурация обновлена"/*"Data config in EEPROM is updated"*/));
	}
	else{
		switch(InputFromSerial0[0]){
			case 0:
				Serial.println(F("======= Конфигурация контроллера ======="/*"======= Configuration Controller ======="*/));
				Serial.print(F("Название контроллера: ")); /*Serial.println(ReadNameController());*/	
				for(byte i = 0; i <= 19; i++){
					byte Symbol = EEPROM.read(E_NameController + i);
					if(Symbol != 0){
						Serial.write(Symbol);
					}
					if(i == 19){
						//Serial.println();
					}
				}
				Serial.println();
				// =================================================
				Serial.print(F("Управление напряжением питания: ")); 
				switch(EEPROM.read(E_ControllVCC)){
					case 0:
						Serial.println(F("Нет"));
						break;
					case 1:
						Serial.println(F("Да"));
						break;
					default:
						Serial.println();
				}
				if(EEPROM.read(E_ControllVCC) != 0){
					Serial.print(F("\tМинимальное VCC: ")); Serial.println(float(EEPROM.read(E_MinInputVCC)) / 10);
					Serial.print(F("\tРеакция на снижение VCC: "));
					switch(EEPROM.read(E_ReactToMinVCC)){
						case REACTION_ERORR_VCC_DONT_REACT:
							Serial.println(F("Не реагировать"));
							break;
						case REACTION_ERORR_VCC_SEND_SMS:
							Serial.print(F("Отправлять СМС"));
							if(EEPROM.read(E_WorkSIM800) == ON){								// Если GSM модуль настроен на постоянную работу
								Serial.println();
							}
							else Serial.println(F(" (GSM модуль отключен)"));							
							break;
						case REACTION_ERORR_VCC_WRITE_ON_SD:
							Serial.print(F("Писать на SD карту"));
							if(EEPROM.read(E_ENABLE_LOGING_TO_SD) == OFF){						// Если разрешена запись на SD карту
								Serial.println(F(" (Запись на SD карту отключена)"));
							}
							else Serial.println();
							break;
						case REACTION_ERORR_VCC_SEND_GET:
							Serial.print(F("Отправлять GET запрос"));
							if(EEPROM.read(E_WorkSIM800) == ON){								// Если GSM модуль настроен на постоянную работу
								Serial.println();
							}
							else Serial.println(F(" (GSM модуль отключен)"));
						case REACTION_ERORR_VCC_SEND_SMS_AND_GET:
							Serial.print(F("Отправлять СМС и GET запрос"));
							if(EEPROM.read(E_WorkSIM800) == ON){								// Если GSM модуль настроен на постоянную работу
								Serial.println();
							}
							else Serial.println(F(" (GSM модуль отключен)"));
						default:
							Serial.println();
							break;
					}
				}
				// ============================================================================
				if(EEPROM.read(E_WorkSIM800) == ON){									// Если GSM модуль настроен на постоянную работу
					Serial.print(F("Отправка СМС о старте контроллера: ")); 
					switch(EEPROM.read(E_SentSMSorStartController)){
						case 0:
							Serial.println(F("Нет"));
							break;
						case 1:
							Serial.println(F("Да"));
							break;
						default:
							Serial.println();
					}
					if(EEPROM.read(E_SentSMSorStartController) == 1){
						Serial.print(F("\tТекст СМС: ")); Serial.println(TextOfStartController);
					}
				}
				// ============================================================================
				Serial.print(F("Логирование в терминал: "));
				
				ViewStateLoginToSerial(EEPROM.read(E_LOGING_TO_SERIAL));
				
				// ============================================================================		
				Serial.print(F("Запись на SD карту: "));		
				if(EEPROM.read(E_ENABLE_LOGING_TO_SD) == 1){
					Serial.println(F("Разрешена"));
				}	
				else Serial.println(F("Запрещена"));
				// ============================================================================
				Serial.print(F("Bluetooth модуль: "));	
				switch(EEPROM.read(E_ConfigPowerBluetooth)){
					case 1:
						Serial.println(F("Всегда включен"));
						break;
					case 2:
						Serial.println(F("Включать по требованию"));
						Serial.print(F("\tТаймаут выключения: ")); Serial.print(EEPROM_int_read(E_BluetoothTimeOff)); Serial.println(F("cек"));
						break;
					default:
						Serial.println();
				}
				// ============================================================================
				Serial.print(F("GSM модуль: "));
				switch(EEPROM.read(E_WorkSIM800)){
					case OFF:
						Serial.println(F("Выключен"));
						break;
					case ON:
						Serial.println(F("Включен"));
						Serial.print(F("\tИнтервал проверки регистрации: ")); Serial.print(EEPROM.read(E_IntervalCheckRegistrationGSM)); Serial.println(F("сек")); 
						Serial.print(F("\tМаксимальное время ожидания ответа: ")); Serial.print(EEPROM.read(E_MaximumTimeResponseGSM)); Serial.println(F("сек")); 
						Serial.print(F("\tВремя перезагрузки модуля: ")); Serial.print(EEPROM.read(E_ResetTimeGSM)); Serial.println(F("сек")); 
						Serial.print(F("\tGPRS: ")); 
						switch(EEPROM.read(E_AllowGPRS)){
							case ON: 
								Serial.println(F("Включен"));
								break;
							case OFF:
								Serial.println(F("Выключен"));
								break;
							default:
								Serial.println();
						}
						Serial.print(F("\tСостояние при снижении VCC контроллера: ")); Serial.print(EEPROM.read(E_GSM_STATE_MIN_VCC));
						switch(EEPROM.read(E_GSM_STATE_MIN_VCC)){
							case ON:
								Serial.println(F("Включен"));
								break;
							case OFF:
								Serial.println(F("Выключение"));
								break;
							case SLEEP:
								Serial.println(F("Сон"));
								break;
							default:
								Serial.println();
						}
						break;
					default:
						Serial.println();
				}
				// ============================================================================
				Serial.println(F("Интервалы запуска ф-ций:"));			
					Serial.print(F("\tВыключение подсветки экрана: "));						Serial.print(EEPROM_int_read(E_LoopLCDLightTime));			Serial.println(F(" сек"));
					Serial.print(F("\tОпрос датчиков: "));									Serial.print(EEPROM_int_read(E_LoopSensorTime));			Serial.println(F(" сек"));
				if(EEPROM.read(E_ENABLE_LOGING_TO_SD) == ON){								// Если разрешена запись на SD карту
					Serial.print(F("\tЦиклическая запись на SD показаний датчиков: "));		Serial.print(EEPROM_int_read(E_LoopWriteValueSensorOnSD));	Serial.println(F(" сек"));
				}				
				Serial.println(F("Параметры встроенного датчика температуры:"));
					Serial.print(F("\tРежим выходного порта: "));							
					switch(EEPROM.read(E_Mode_OS_INT_LM75)){
						case 0:
							Serial.println(F("Comparator mode"));
							break;
						case 1:
							Serial.println(F("Interrupt mode"));
							break;
						default:
							Serial.println();
					}
					Serial.print(F("\tЗначение Tos: "));	Serial.print(EEPROM.read(E_INT_LM75_TOS));		Serial.println(F("*C"));
					Serial.print(F("\tЗначение Thyst: "));	Serial.print(EEPROM.read(E_INT_LM75_THYST));	Serial.println(F("*C"));
				Serial.println(F("========================================"));
				break;
			case 1:
				Serial.print(F("c 0"));											Serial.print(F(" "));		// Служебный байт
				Serial.print(EEPROM.read(E_ControllVCC));						Serial.print(F(" "));		// Контролировать или нет напряжение питания контроллера (если нет, то оно просто измеряется и выводится у UART и LCD)
				Serial.print(EEPROM.read(E_MinInputVCC));						Serial.print(F(" "));		// Минимальное напряжение питания (используется если контролируется)
				Serial.print(EEPROM.read(E_SentSMSorStartController));			Serial.print(F(" "));		// Отправлять или нет СМС о старте контроллера
				Serial.print(EEPROM.read(E_LOGING_TO_SERIAL));					Serial.print(F(" "));		// Уровень логиравания в терминал
				Serial.print(EEPROM.read(E_ENABLE_LOGING_TO_SD));				Serial.print(F(" "));		// Уровень логирования на SD карту (0 - молчать, 1 - только ошибки, 2 - только данные, 3 - все)
				Serial.print(F("0"));											Serial.print(F(" "));
				Serial.print(F("0"));											Serial.print(F(" "));
				Serial.print(EEPROM.read(E_ModeController));					Serial.print(F(" "));		// Автоматический\ручной режим работы контроллера
				Serial.print(EEPROM.read(E_ConfigPowerBluetooth));				Serial.print(F(" "));		// Режим подачи питания на Bluetooth модуль:
				Serial.print(EEPROM_int_read(E_BluetoothTimeOff));				Serial.print(F(" "));		// (тип int) Время отключения модуля при бездействии (сек). Работает только если E_ConfigPowerBluetooth выставлен в "2"
				Serial.print(EEPROM_int_read(E_LoopLCDLightTime));				Serial.print(F(" "));
				Serial.print(EEPROM_int_read(E_LoopSensorTime));				Serial.print(F(" "));
				Serial.print(EEPROM_int_read(E_LoopWriteValueSensorOnSD));		Serial.print(F(" "));
				Serial.print(EEPROM_int_read(E_LoopChannelRun));				Serial.print(F(" "));
				Serial.print(EEPROM.read(E_ReactToMinVCC));						Serial.print(F(" "));		
				Serial.print(EEPROM.read(E_GSM_STATE_MIN_VCC));					Serial.print(F(" "));		// Что делать с GSM модулем есть питание контроллера ниже минимального (0 - выключен, 1 - включен)
				Serial.print(EEPROM.read(E_ResetTimeGSM));						Serial.print(F(" "));		// Время перезагрузки модуля
				Serial.print(EEPROM.read(E_MaximumTimeResponseGSM));			Serial.print(F(" "));		// Максимальное время ожидания ответа от модуля
				Serial.print(EEPROM.read(E_IntervalCheckRegistrationGSM));		Serial.print(F(" "));		// Интервал проверки регистрации GSM
				Serial.print(EEPROM.read(E_WorkSIM800));						Serial.print(F(" "));		// Режим питания SIM800
				Serial.print(EEPROM.read(E_AllowGPRS));							Serial.print(F(" "));		// Разрешение работы GPRS
				Serial.print(EEPROM.read(E_Mode_OS_INT_LM75));					Serial.print(F(" "));		// Режим работы встроенное выходного порта
				Serial.print(EEPROM.read(E_INT_LM75_TOS));						Serial.print(F(" "));		// Значение бита TOS встроенного LM75
				Serial.print(EEPROM.read(E_INT_LM75_THYST));					Serial.print(F(" "));		// Значение бита Thyst встроенного LM75
				for (byte i = 0; i < 19; i++){
					Serial.print(EEPROM.read(E_NameController + i));
					Serial.print(F(" "));
				}
				Serial.println();
				break;
		}
	}
		
	CleanInputFromSerial0();

	recievedFlag_config_controller = false;
	flag = false;
}