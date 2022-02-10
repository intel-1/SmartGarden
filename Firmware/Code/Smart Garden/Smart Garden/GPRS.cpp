#include <Arduino.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/wdt.h>


#include "GSM.h"
#include "GPRS.h"


// debug

// ========================================================================================================
// ============================= Проверка ответа от Web сервера на GET запрос =============================
// ========================================================================================================
void Answer_check_GET(String Text, bool LogView){
	
	StateGSM.Code_Error_Sent_GET = 255;				// default ошибка "100"
			
	if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL || ControllerSetup){
		if(LogView){
			Serial.print(F("Response from WEB server: "));
		}
	}

	String Response[] = {	(F("404")),				// Страница не найдена
							(F("200")),				// GET запрос успешно получен!!!
							(F("603"))};			// Сервер не доступен
	
	for(byte Pos = 0; Pos < sizeof(Response) / sizeof(Response[0]); Pos ++){						// Ищем в списке возможных ответов
		if(Text.lastIndexOf(Response[Pos]) != -1){				// И если нашли совпадение
			if(OUTPUT_LEVEL_UART_GSM){
				switch(Pos){
					case 0:
						StateGSM.Code_Error_Sent_GET = 404;
						if(LogView){
							Serial.println(F("Page not found"));
						}
						break;
					case 1:
						StateGSM.Code_Error_Sent_GET = 200;
						if(LogView){
							Serial.println(F("OK"));
						}
						break;
					case 2:
						StateGSM.Code_Error_Sent_GET = 603;
						if(LogView){
							Serial.println(F("Server not available"));
						}
						break;
					default:
						if(LogView){
							Serial.println();
						}
					}
			}
		}
	}
}


// ======================================================================================================
// ======================================================================================================
// ======================================================================================================
void ViewErrorConnectGPRS(byte ErrorConnectGPRS, byte lang){
	switch(ErrorConnectGPRS){
		case 0:
			Serial.print(F("The connection is established"/*"Соединение устанавливается"*/));
			break;
		case 1:
			Serial.print(F("GPRS is connected"/*"GPRS подключен"*/));
			break;
		case 2:
			Serial.print(F("The connection is closed"/*"Соединение закрывается"*/));
			break;
		case 3:
			Serial.print(F("No connection"/*"Нет соединения"*/));
			break;
		case 4:
			Serial.print(F("No data from GSM module"/*"Нет данных от GSM модуля"*/));
			break;
	}
}


// ======================================================================================================
// ========================================= Инициализация GPRS =========================================
// ======================================================================================================
void InitializingGPRS(){
	String GPRS_ATs[] = {										// массив АТ команд инициализации GPRS
		(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"")),				// Установка настроек подключения
		(F("AT+SAPBR=3,1,\"APN\",\"internet.beeline.ru\"")),	// Имя APN точки доступа
		(F("AT+SAPBR=3,1,\"USER\",\"beeline\"")),				// Имя пользователя
		(F("AT+SAPBR=3,1,\"PWD\",\"beeline\"")),				// Пароль
		(F("AT+SAPBR=1,1")),									// Установка GPRS соединения
		(F("AT+HTTPINIT")),										// Инициализация http сервиса
	};
	String Word;
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.println(F("============= Start GPRS ============="));
	}
	for (byte i = 0; i < sizeof(GPRS_ATs) / sizeof(GPRS_ATs[0]); i++) {
		wdt_reset();
		Word = sendATCommand(GPRS_ATs[i], YES, YES);			// Отправляем АТ команду, ждем, получаем и выводим ответ в Serial
		byte TimerCommand = 1;
		while(TimerCommand <= 4){								// Максимальное кол-во отправок команды
			if(Word.lastIndexOf("OK") != -1){					// Если ответ "OK"
				TimerCommand = 5;								// Останавливаем выполнение цикла while
				goto end_while;
			}
			else{
				Word = sendATCommand(GPRS_ATs[i], YES, NO);	// Повторно отправляем команду
				//Serial.println("WAIT");
				TimerCommand ++;
			}
		}
		end_while: ;
	}
	ConnectionGPRS();											// Проверяем подключение
}


// ======================================================================================================
// ===================================== Проверка регистрации GPRS ======================================
// ======================================================================================================
bool ConnectionGPRS(){
	StateGSM.GPRS_Connect = false;				// Сбрасываем флаг подключения
	StateGSM.IP_GPRS = "0";
	
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.print(F("State GPRS Connection: "));
	}
	String Response[] = {	(F("1,0")),			// Соединение устанавливается
							(F("1,1")),			// Соединение установлено
							(F("1,2")),			// Соединение закрывается
							(F("1,3"))};		// Нет соединения
	
	String Error = StateGSM.IP_GPRS = sendATCommand(F("AT+SAPBR=2,1"), true, false);	// Узнаем параметры соединения
	
	if(Error.lastIndexOf(F("+SAPBR")) != -1){					// Проверяем правильность ответа, что он именно на наш запрос
		
		// IP адрес
		StateGSM.IP_GPRS.remove(1,13);
		StateGSM.IP_GPRS.remove(StateGSM.IP_GPRS.length()-4);
		StateGSM.IP_GPRS.trim();
		StateGSM.IP_GPRS.remove(StateGSM.IP_GPRS.length()-1);
		//--//--//--//
	
		for(byte Pos = 0; Pos <= sizeof(Response) / sizeof(Response[0]); Pos ++){	// Ищем в списке возможных ответов
			if(Error.lastIndexOf(Response[Pos]) != -1){								// И если нашли совпадение
				StateGSM.Code_Connect_GPRS = Pos;									// Сохраняем код GPRS соединения
				switch(Pos){
					case 1:															// Если соединение установлено (ответ "+SAPBR: 1,1")
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.print(F("Established ("));
							Serial.print(StateGSM.IP_GPRS);
							Serial.println(F(")"));
							Serial.println(F("=============================================="));
						}
						StateGSM.GPRS_Connect = true;		// Поднимаем флаг. Сделано для удобства
						break;
				}
				Pos = 4;									// Увеличиваем счетчик чтобы прекратить выполнение цикла
			}
			if(Pos == 3){									// Если перебрали все ответы и не нашли совпадение, то считаем что модуль ничего не ответил
				StateGSM.Code_Connect_GPRS = 4;				// Поднимаем ошибку "4"
			}
		}
	}
	return StateGSM.GPRS_Connect;
}


// =====================================================================================================
// ======================================= Отправка GET запроса ========================================
// =====================================================================================================
void SendGETGPRS(String Text){
	wdt_reset();
	if(EEPROM.read(E_AllowGPRS) == ON){								// Если разрешена работа GPRS
		if(OUTPUT_LEVEL_UART_GSM){
			Serial.print(F("Sending a GET request: "));
		}
		sendATCommand(F("AT+HTTPPARA=\"CID\",1"), true, false);		// Установка CID параметра для http сессии
		String Answer = sendATCommand(Text, true, false);			// Шлем сам запрос
		if(Answer.lastIndexOf(F("OK")) != -1){						// Если запрос успешно отправлен	
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("OK"));
			}
			// ========================= Проверяем ответ от сервера ==========================================
			String val = waitResponse(ON);					// Ждем ответ от Web сервера
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(val);
			}
			if (val.indexOf(F("+HTTPACTION")) > -1) {		// Ответ на GET запрос
				Answer_check_GET(val, ON);					// Проверяем ответ и выводим лог в Serial
			}
		}
		else{
			StateGSM.Error_Sent_GET = true;					// Поднимаем флаг, что GET не отправлен
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("ERROR"));
			}
		}
		sendATCommand(F("AT+HTTPACTION=0"), true, false);	// Закрытие http сессии		
	}
}
