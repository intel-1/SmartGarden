#include <./Arduino.h>
#include <./EEPROM.h>

#include "RecievedDebug.h"
#include "../EEPROM_ADR.h"
#include "../ParsingDataOnSerial.h"
#include "../Sensors.h"
#include "../DigitalPorts.h"
#include "../main.h"
#include "../ExecModules.h"
#include "../DigitalPorts.h"
#include "../GSM.h"
#include "../LCDdisplay.h"


void ViewStateLoginToSerial(byte State){
	switch (EEPROM.read(E_LOGING_TO_SERIAL)){
		case UART_LOG_LEVEL_OFF:
			Serial.println(F("Выключено"));
			break;
		case UART_LOG_LEVEL_SENSOR:
			Serial.println(F("Датчики"));
			break;
		case UART_LOG_LEVEL_CHANNEL:
			Serial.println(F("Группы управления"));
			break;
		case UART_LOG_LEVEL_MODULE:
			Serial.println(F("Исполнительные модули"));
			break;
		case UART_LOG_LEVEL_GSM:
			Serial.println(F("GSM модуль"));
			break;
		case UART_LOG_LEVEL_ALL:
			Serial.println(F("Все"));
			break;
	}
}


// =========================== Изменение режима debug ===================================
void RecievedDebug(){
	if (0 <= InputFromSerial0[0] == 0 || InputFromSerial0[0] <= 6){		// Изменение режима вывода данных и ошибок в консоль
		EEPROM.update(E_LOGING_TO_SERIAL, InputFromSerial0[0]);			// Пишем в EEPROM
		LOGING_TO_SERIAL = InputFromSerial0[0];							// и сохраняем в переменную чтобы не перегружать контроллер
 		if(InputFromSerial0[1] == 0){									// Чтобы не выводить если хотим только узнасть например адреса i2c
 			Serial.print(F("Уровень логирования в UART: ")); ViewStateLoginToSerial(EEPROM.read(E_LOGING_TO_SERIAL));
 		}
	}
	else Serial.println(F("Data is error"));
	if(InputFromSerial0[0] == 0){				// Если запрещен вывод данных и ошибок в консоль
		switch(InputFromSerial0[1]){
			case 0:
				//DebugRepet_1 = false;			// Выключаем циклический вывод данных
				StateSwitch = false;
				break;
			case 1:
				StateSwitch = true;
				//DebugRepet_1 = true;
				ViewStateAllSwitch();
				break;
			case 2:
				i2c_scaner(ON);
				break;
			case 3:
				DS18B20_scaner(ON);		// Вывод результатов поиска с описаниями
				break;
			case 4:
				DS18B20_scaner(OFF);		// Вывод результатов поиска в виде байт
				Serial.println();
				break;
			case 5:
				View_DataEEPROM();
				break;
			case 6:									// Перезагрузка контроллера
				Serial.println(F("===== Перезагрузка ====="));
				while(1);				
				jmp_bootloader();					
				break;
			case 7:									// Перевод контроллера в ручной режим
				EEPROM.update(E_ModeController, 1);
				break;
			case 8:									// Сбрасываем таймер выключения Bluetooth
				LoopOffBluetoothTime = 0;
				status_led_blue_on();
				delay(500);
				status_led_blue_off();
				break;
			case 9:									// Включение режима конфигурирования контроллера
				//EEPROM.update(E_ConfigModeController, 1);
				//Reboot();							// И перезагрузка
				break;
			case 10:								// Выключение режима конфигурирования контроллера
				//EEPROM.update(E_ConfigModeController, 0);	
				break;
			case 11:								// Время работы контроллера (в виде байт)
				Serial.print(T_day);			Serial.print(F(" "));
				Serial.print(T_hour);			Serial.print(F(" "));
				Serial.print(T_min);			Serial.print(F(" "));
				Serial.println(TimerTsecond);
				break;	
			case 12:								// Время работы контроллера (с описаниями)
				Serial.print(F("Uptime: "));
				if (T_day > 0){
					Serial.print(T_day);
					Serial.print(F(" дней и "));
				}
				Serial.print(T_hour);
				Serial.print(F(":"));
				Serial.print(T_min);
				Serial.print(F(":"));
				Serial.println(TimerTsecond);
				break;
			// Блок управления GSM модемом
			case 13:
				Initializing_GSM(LCD_ALLOW_OTPUT_ON_SCREEN);	// Инициализация GSM модуля
				break;
			case 14:
				send_AT_Command(F("AT"), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("AT"));					// Готовность модуля к работе
				break;
			case 15:
				send_AT_Command(F("ATI"), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("ATI"));				// Запрос информации об устройстве
				break;
			case 16:
				send_AT_Command(F("AT+CSQ"), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("AT+CSQ"));				// Запрос качества связи
				break;
			case 17:
				send_AT_Command(F("AT+CREG?"), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("AT+CREG?"));		// Тип регистрации в сети
				break;
			case 18:
				send_AT_Command(F("AT+COPS?"), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("AT+COPS?"));		// Информация об операторе
				break;
			case 19:
				send_AT_Command(F("AT+CPAS"), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("AT+CPAS"));		// Статус телефона GSM-модуля
				break;
			case 20:
				send_AT_Command(F("AT+CGM"), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("AT+CGM"));		// Запрос версии установленного ПО
				break;
			case 21:
				send_AT_Command(F("AT+CCALR?"), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("AT+CCALR?"));	// Готовность модуля совершать звонки
				break;
			case 22:
				send_AT_Command(F("AT+GSN"), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("AT+GSN"));		// Запрос IMEI-модуля
				break;
			case 23:
				send_AT_Command(F("AT+GSV"), true, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("AT+GSV"));		// Запрос идентификационной информации модуля
				break;
			case 24:
				send_AT_Command(F("AT+CBC"), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("AT+CBC"));		// Напряжение питания
				break;
			case 25:
				send_AT_Command(F("AT+COPN"), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("AT+COPN"));		// Получение списка всех операторов
				break;
			case 26:
				send_AT_Command(F("AT+CFUN?"), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
				//Serial3.println(F("AT+CFUN?"));		// Проверка функциональности модуля
				break;
			case 27:
				AllowRunMotor = false;
				break;
			case 28:								// Поднимаем флаг что не нужно возвращать исполнительные модули в нули
				Serial.print(F("Выключение начального возврата исп. модулей в нули..."));
				EEPROM.write(E_ReturnModulesToZeros, 0);
				if(EEPROM.read(E_ReturnModulesToZeros) == 0){
					Serial.println(F("OK"));
				}
				else Serial.println(F("ERR"));
				break;
			case 30:
				Serial.println();
				Serial.print(F("t "));
				for(byte i = 0; i <= 4096; i++){
					Serial.print(EEPROM.read(i)); Serial.print(F(" "));
				}
				Serial.println("");
				break;
		}
		cleanEEPROM(InputFromSerial0[2]);			// Очистка настроек EEPROM
	}
	
	
// 	for (byte i=0; i<sizeof(InputFromSerial0)/sizeof(int); i++){			// Затираем массив после работы
// 		InputFromSerial0[i] = 0;
// 	}
	CleanInputFromSerial0();
	recievedFlag_debug = false;
}