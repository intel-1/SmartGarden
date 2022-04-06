#include <Arduino.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include "GSM.h"
#include "main.h"
#include "Sensors.h"
#include "Warning.h"
#include "LCDdisplay.h"


String Link_LogWebServer;
String Link_LogDataWebServer;

uint8_t BalanceStringLen = 22;										// Количество символов от начала строки которые нужно переслать в смс сообщении при получении USSD ответа о балансе
String RingPhone = "";
String GSM_Tean_Name[10] = "";
byte TurnOfMessagesGSM[ExtentOfTurn][QuantityParametersInMessage];	// Очередь СМС сообщений (максимально 10 исходящих сообщений)

struct StructGPRSConnection StateGSM;

	
void WriteToQueueGSM(struct MessageQueueGSM Par){
	for(byte Place = 0; Place < ExtentOfTurn; Place ++){				// Проходим по всем ячейчам очереди сообщений
		if(TurnOfMessagesGSM[Place][0] == 0){							// Когда найдена свободная
			TurnOfMessagesGSM[Place][0] = 1;							// Помечаем занятой строку очереди
			TurnOfMessagesGSM[Place][1] = Par.Arg_1_a;			
			TurnOfMessagesGSM[Place][2] = Par.Arg_1_b;				
			TurnOfMessagesGSM[Place][3] = Par.Arg_2_a;				
			TurnOfMessagesGSM[Place][4] = Par.Arg_2_b;			
			TurnOfMessagesGSM[Place][5] = Par.Arg_3_a;				
			TurnOfMessagesGSM[Place][6] = Par.Arg_3_b;			
			TurnOfMessagesGSM[Place][7] = Par.Arg_4_a;
			TurnOfMessagesGSM[Place][8] = Par.Arg_4_b;
			TurnOfMessagesGSM[Place][QuantityParametersInMessage-1] = 0;	// Номер телефона "9139045925"
			Place = ExtentOfTurn;
		}
	}
	// Очищаем структуру от старых данных
	Par.Arg_1_a = 0;
	Par.Arg_1_b = 0;
	Par.Arg_2_a = 0;
	Par.Arg_2_b = 0;
	Par.Arg_3_a = 0;
	Par.Arg_3_b = 0;
	Par.Arg_4_a = 0;
	Par.Arg_4_b = 0;
	Par.PhoneNumber = 0;
}



// =========================================================================================================
// ==================================== Включение\выключание GSM модуля ====================================
// =========================================================================================================
void Power_GSM(byte _State){
	switch(_State){
		case RESET:						// Перезагрузка модуля
			gsm_vcc_off();
			_delay_ms(500);
			gsm_vcc_on();				// Включение DC-DC
			gsm_pwr_key();				// Включение самого модуля
			_delay_ms(3000);			// Ожидание запуска. По datasheet 3 сек нужно для получения ответов по Serial
			break;
		case OFF:						// Выключение питания модуля
			gsm_vcc_off();
			break;
		case ON:						// Включение питания
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("Inclusion VCC...OK"));
			}
			gsm_vcc_on();				// Включение DC-DC
			gsm_pwr_key();				// Включение самого модуля
			_delay_ms(3000);			// Ожидание запуска. По datasheet 3 сек нужно для получения ответов по Serial
			break;
	}
}


// =========================================================================================================
// =========================================== Инициализация GSM ===========================================
// =========================================================================================================
void InitializingGSM(){
	wdt_reset();
	String _GSMATs[] = {								// Массив АТ команд инициализации GSM
						//(F("ATE0V0+CMEE=1;&W")),		// Для рабочего режима (выключены Эхо, уменьшен уровель логирования)
						(F("ATE0")),					// Выключаем эхо
						(F("ATE1V1+CMEE=2;&W")),		// Средний уровень логирования (для тестового режима работы)
						(F("AT+CLIP=1")),				// Включаем АОН
						(F("AT+CMGF=1")),				// Формат СМС = ASCII текст
						(F("AT+CSCS=\"GSM\"")),			// Режим кодировки текста = GSM (только англ.)
						//(F("AT+CNMI=2,2")),
	};
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.println(F("================= Start initializing GSM module  ================="));
	}
	if(ControllerSetup){
		WriteToLCD(String(F("- AT: ")), LCD_LINE_4, LCD_START_SYMBOL_2, false);
	}
	for (byte i = 0; i < sizeof(_GSMATs) / sizeof(_GSMATs[0]); i++) {
		wdt_reset();
 		if(ControllerSetup){
 			WriteToLCD(String(F("*")), LCD_LINE_4, LCD_START_SYMBOL_8 + i, false);
 		}
		sendATCommand(_GSMATs[i], true, true);					// Отправляем АТ команду, ждем ответ и выводим ответ в Serial
	}
	if(ControllerSetup){
		WriteToLCD(String(F("OK")), LCD_LINE_4, LCD_START_SYMBOL_19, false);
	}
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.println(F("We resolve sleep mode...OK"));
	}
	Serial3.println(F("AT+CSCLK=1"));							// Разрешаем спящий режим
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.println(F("================= Initializing complete ================="));
	}
	gsm_dtr_on();												// Усыпляем модуль
}


// ======================================================================================================
// ========================================= Инициализация GPRS =========================================
// ======================================================================================================
void InitializingGPRS(){
	String _GPRS_ATs[] = {															// массив АТ команд инициализации GPRS
							(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"")),				// Установка настроек подключения
							(F("AT+SAPBR=3,1,\"APN\",\"internet.beeline.ru\"")),	// Имя APN точки доступа
							(F("AT+SAPBR=3,1,\"USER\",\"beeline\"")),				// Имя пользователя
							(F("AT+SAPBR=3,1,\"PWD\",\"beeline\"")),				// Пароль
							(F("AT+SAPBR=1,1")),									// Установка GPRS соединения
							(F("AT+HTTPINIT")),										// Инициализация http сервиса
	};
	String _Word;
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.println(F("============= Start GPRS ============="));
	}
	if(ControllerSetup){
		WriteToLCD(String(F("- AT: ")), LCD_LINE_3, LCD_START_SYMBOL_2, false);
		WriteToLCD(String(F("                    ")), LCD_LINE_4, LCD_START_SYMBOL_1, false);
	}
	for (byte i = 0; i < sizeof(_GPRS_ATs) / sizeof(_GPRS_ATs[0]); i++) {
		wdt_reset();
		if(ControllerSetup){
			WriteToLCD(String(F("*")), LCD_LINE_3, LCD_START_SYMBOL_8 + i, false);
		}
		_Word = sendATCommand(_GPRS_ATs[i], YES, YES);					// Отправляем АТ команду, ждем, получаем и выводим ответ в Serial
		byte TimerCommand = 1;
		while(TimerCommand <= 4){										// Максимальное кол-во отправок команды
			if(_Word.lastIndexOf(F("OK")) != -1){						// Если ответ "OK"
				TimerCommand = 5;										// Останавливаем выполнение цикла while
				goto end_while;
			}
			else{
				_Word = sendATCommand(_GPRS_ATs[i], YES, NO);			// Повторно отправляем команду
				TimerCommand ++;
			}
		}
		end_while: ;
	}
	if(ControllerSetup){
		WriteToLCD(String(F("OK")), LCD_LINE_3, LCD_START_SYMBOL_19, true);
	}
	CheckConnectionGPRS();												// Проверяем подключение
	if(ControllerSetup){
		WriteToLCD(String(F("- IP: ")), LCD_LINE_4, LCD_START_SYMBOL_2, false);
		WriteToLCD(StateGSM.IP_GPRS, LCD_LINE_4, LCD_START_SYMBOL_7, true);
		
	}
}


// =========================================================================================================
// ======================= Функция ожидания ответа и возврата полученного результата =======================
// =========================================================================================================
String waitResponse(bool _logView) {
	String _resp = "";															// Переменная для хранения результата
	long _timeout = millis() + EEPROM.read(E_MaximumTimeResponseGSM) * 1000;	// Переменная для отслеживания таймаута (10 секунд)
	while (!Serial3.available() && millis() < _timeout){						// Ждем ответа 10 секунд и обнуляем собаку чтобы не перезагрузить контроллер,
		wdt_reset();															// сбрасываем собаку чтобы она не сбросила контроллер во время ожидания
	};
	//delay(1000);																// если пришел ответ или наступил таймаут, то...
	if (Serial3.available()) {													// Если есть, что считывать
		_resp = Serial3.readString();											// считываем и запоминаем
	}
	else {																		// Если пришел таймаут, то
		if(_logView){
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("Timeout..."));								// ... оповещаем об этом и
			}
		}
	}
	return _resp;																// ... возвращаем результат. Пусто, если проблемма
}


// ========================================================================================================
// ==================================== Отправка AT команды GSM модулю ====================================
// ========================================================================================================
String sendATCommand(String _Command, bool _waiting, bool _logView) {
	wdt_reset();
	String _resp = "";											// Переменная для хранения результата
	if(OUTPUT_LEVEL_UART_GSM){
		if(_logView){
			Serial.println(_Command);								// Дублируем команду в монитор порта
		}
	}
	Serial3.println(_Command);										// Отправляем команду модулю
	if (_waiting) {												// Если необходимо дождаться ответа...
		_resp = waitResponse(_logView);							// ... ждем, когда будет передан ответ
		//Если Echo Mode выключен (ATE0), то эти 3 строки можно закомментировать
		if (_resp.startsWith(_Command)) {							// Убираем из ответа дублирующуюся команду
			_resp = _resp.substring(_resp.indexOf("\r", _Command.length()) + 2);
		}
	}
	if(OUTPUT_LEVEL_UART_GSM){
		if(_logView){
			Serial.println(_resp);								// Дублируем ответ в монитор порта
		}
	}
	return _resp;												// Возвращаем результат. Пусто, если проблема
}


// =========================================================================================================
// ============================================= Отправка SMS ==============================================
// =========================================================================================================
void SendSMS(String _Text, byte _Level) {	
	/*
		Text - текст сообщения
		PhoneNumber - номер телефона получателя
		Level - уровень важности сообщения. От этого зависит кому отправлять СМС
	*/
	
	CheckRegistrationGSM(false);			// Проверяем регистрацию в сети
	
	if(StateGSM.GSM_Registered){			// и если зарегистрирован
		String Phone;
		switch(_Level){
			case 0:							// info — обычные сообщения
				//Phone = PhoneNumber;
				Phone = (F("+79139045925"));
				break;
			case 1:							// Ответ на входящее СМС
				Phone = RingPhone;
				break;
			case 2:							// debug — Подробная информация для отладки			
				break;
			case 3:							// warning — Исключительные случаи, но не ошибки
				//Phone = NumberForWarningSMS[0];
				break;
			case 4:							// error — Ошибки исполнения, не требующие сиюминутного вмешательства
				Phone = (F("+79139045925"));
				break;
			case 5:							// critical — Критические состояния
				break;
			case 6:							// emergency — Система не работает
				break;
		}
		//gsm_dtr_off();								// Взбадриваем модуль
		Serial3.print(F("AT+CMGF=1\r"));
		delay(100);
		Serial3.println("AT+CMGS=\"" + Phone + "\"");
		delay(100);
		Serial3.print(_Text);
		delay(50);
		Serial3.print((char)26);
		delay(500);	
		gsm_dtr_on();								// Усыпляем модуль
	}
}


// =====================================================================================================
// ======================================= Отправка GET запроса ========================================
// =====================================================================================================
void SendGETrequest(String Text){
	bool ResendingGET = false;
	byte ResendCounterResendingGET = 2;			// Счетчик повторных отправок Get запросов (2 шт)
	wdt_reset();
	if(EEPROM.read(E_AllowGPRS) == ON){								// Если разрешена работа GPRS
		if(OUTPUT_LEVEL_UART_GSM){
			Serial.print(F("Sending a GET request: "));
		}
		sendATCommand(F("AT+HTTPPARA=\"CID\",1"), true, false);		// Установка CID параметра для http сессии
		SentGET:													// Метка для повторной отправки GET запроса
		String Answer = sendATCommand(Text, true, false);			// Шлем сам запрос и ждем ответ		
		if(Answer.lastIndexOf(F("OK")) != -1){						// Если запрос успешно отправлен, модуль вернул ответ "ОК"
			StateGSM.Error_Sent_GET = false;						// Снимаем флаг возможной ошибки
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("OK"));
			}
			// ========================= Проверяем ответ от сервера ==========================================
// 			String Response = waitResponse(ON);						// Ждем ответ от Web сервера
// 			if(OUTPUT_LEVEL_UART_GSM){
// 				Serial.println(Response);
// 			}
// 			if (Response.indexOf(F("+HTTPACTION")) > -1) {			// Ответ на GET запрос
// 				
// 				Answer_check_GET(Response, ON);						// Проверяем ответ и выводим лог в Serial
// 			}
		}
		else{														// и если не отправлен
			StateGSM.Error_Sent_GET = true;							// Поднимаем флаг ошибки
			ResendingGET = true;									// Флаг чтобы повторного отправить GET
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("ERROR"));
			}
		}
		if(ResendCounterResendingGET == 2 && ResendingGET){			// Переводим код на метку для повторной отправки
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.print(F("\tResending a GET request: "));
			}
			ResendCounterResendingGET --;
			goto SentGET;
		}
		sendATCommand(F("AT+HTTPACTION=0"), true, false);			// Закрытие http сессии
	}
}


// ========================================================================================================
// ================================ Проверка готовности SIM-карты (AT+CPIN?) ==============================
// ========================================================================================================
bool SIM_card_readiness_check(){
	bool _State = false;
	String Return = sendATCommand(F("AT+CPIN?"), true, false);		// Запрос статуса SIM карты
	
	if(Return.lastIndexOf(F("+CPIN")) != -1){						// Если вернулся правильный ответ, а не какой нибудь мусор
		if(Return.lastIndexOf(F("READY")) != -1){					// Если SIM карта готова
			_State = true;										
		}
	}	
	return _State;
	/*
	Test Command: AT+CPIN=?
	READY		MT is not pending for any password
	SIM	PIN		MT is waiting SIM PIN to be given
	SIM PUK		MT is waiting for SIM PUK to be given
	PH_SIM PIN	ME is waiting for phone to SIM card (antitheft)
	PH_SIM PUK	ME is waiting for SIM PUK (antitheft)
	SIM PIN2	PIN2, e.g. for editing the FDN book possible only if preceding Command was acknowledged with +CME ERROR: 17
	SIM PUK2	Possible only if preceding Command was acknowledged with error +CME ERROR: 18.*/
}


// ========================================================================================================
// =================================== Уровень сигнала GSM сети (AT+CSQ) ==================================
// ========================================================================================================
byte SignalLevel(bool _LogView){
	int _Level = 99;
	String _Return = sendATCommand(F("AT+CSQ"), true, false);		// Запрос уровня, обязательно дожидаемся ответа
	
	if(_Return.lastIndexOf(F("+CSQ")) != -1){						// Если получили правильный ответ
		_Return = _Return.substring(7,9);							// Убираем лишние символы
		_Level = _Return.toInt();									// Возвращает уровень сигнала
	}
	if(_LogView){
		if(OUTPUT_LEVEL_UART_GSM){
			Serial.print(F("Уровень сигнала: "));
		}
	}
	if(_Level == 0){
		StateGSM.GSM_Signal_Level = 115;
		if(_LogView){
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("115 dBm и меньше"));
			}
		}
		goto Stop;
	}
	else if(_Level == 1){
		StateGSM.GSM_Signal_Level = 111;
		if(_LogView){
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("111 dBm"));
			}
		}
		goto Stop;
	}
	else if(2 <= _Level <= 30){
		if(_LogView){
			StateGSM.GSM_Signal_Level = map(_Level, 2, 30, 109, 53);
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.print(map(_Level, 2, 30, 109, 53));
				Serial.println(F(" dBm"));
			}
		}
		goto Stop;
	}
	else if(_Level == 31){
		StateGSM.GSM_Signal_Level = 52;
		if(_LogView){
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("52 dBm и сильнее"));
			}
		}
		goto Stop;
	}
	else if(_Level == 99){
		StateGSM.GSM_Signal_Level = 99;
		if(_LogView){
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("нет сигнала"));
			}
		}
		goto Stop;
	}
	Stop:
	return _Level;
}


// Проверка уровня сигнала. Первая цифра — уровень сигнала <rssi>:
// 0 — 115 dBm or меньше					Превосходно
// 1 -111 dBm								Отлично
// 2…30 -110… -54 dBm						ОК
// 31 -52 dBm or больше						Плохо
// 99 — не известно или не обнаруживается	Нет



// ========================================================================================================
// ===================== Проверка готовности модуля. Отправка и проверка команды "AT" =====================
// ========================================================================================================
byte Check_Readiness_Module(boolean _LogView){					 
	
	if(ControllerSetup){
		WriteToLCD(String(F("- Readiness:      ")), LCD_LINE_3, LCD_START_SYMBOL_2, false);
	}
	
	if(_LogView){
		if(OUTPUT_LEVEL_UART_GSM){
			Serial.print(F("Состояние GSM модуля (\"AT\"): "));
		}
	}
	String _Response[] = {	(F("NOT INSERTED")),								// Готов
							(F("OK"))};											// Зарегистрирован в сети
	
	String _Error = sendATCommand(F("AT"), true, false);						// Отправляем комманду, ждем ответ, но в Serial его не выводим
	for(byte Pos = 0; Pos < sizeof(_Response) / sizeof(_Response[0]); Pos ++){	// Ищем в списке возможных ответов
		if(_Error.lastIndexOf(_Response[Pos]) != -1){							// И если нашли совпадение
			switch(Pos){
				case 0:															// Нет SIM карты
					if(ControllerSetup){
						WriteToLCD(String(F("Not SIM")), LCD_LINE_3, LCD_START_SYMBOL_14, true);
					}
					if(_LogView){
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.print(F("Нет SIM карты"));
						}
					}
					Pos = 2;
					StateGSM.State_GSM_Module = 0;
					break;
				case 1:															// Готов
					if(ControllerSetup){
						WriteToLCD(String(F("OK")), LCD_LINE_3, LCD_START_SYMBOL_19, true);
					}
					if(_LogView){
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.println(F("OK"));
						}
					}
					Pos = 2;
					StateGSM.State_GSM_Module = 1;
					break;
				default:														// Неизвестно
					if(ControllerSetup){
						WriteToLCD(String(F("Unknown")), LCD_LINE_3, LCD_START_SYMBOL_14, true);
					}
					if(_LogView){
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.println(F("Неизвестно"));
						}
					}
					Pos = 2;
					StateGSM.State_GSM_Module = 50;
					StateGSM.GSM_Registered = false;
			}
		}
	}
	return StateGSM.State_GSM_Module;
}


// ========================================================================================================
// =================== Проверка зарегистрировался или нет модуль в сети GSM (AT+CREG?) ====================
// ========================================================================================================
bool CheckRegistrationGSM(bool _LogView){				
	/*
		Ф-ция возвращает только true или false. 
		Остальные состояния можно посмотреть в переменной StateGSM.Code_Connect_GSM
	*/
	
	if(_LogView){
		if(OUTPUT_LEVEL_UART_GSM){
			Serial.print(F("Состояние GSM: "));
		}
	}
	
	String _Response[] = {	(F("+CREG: 0,0")),				// Не зарегистрирован в сети
							(F("+CREG: 0,1")),				// Зарегистрирован в сети
							(F("+CREG: 0,2")),				// Поиск сети
							(F("+CREG: 0,3")),				// Регистрация отклонена
							(F("+CREG: 0,4"))};				// Неизвестно
	
	String _Error = sendATCommand(F("AT+CREG?"), true, false);					// Отправляем команду, ждем ответ, но в Serial его не выводим
	
	for(byte Pos = 0; Pos < sizeof(_Error) / sizeof(_Error[0]); Pos ++){		// Ищем в списке возможных ответов
		if(_Error.lastIndexOf(_Response[Pos]) != -1){							// И если нашли совпадение
			if(OUTPUT_LEVEL_UART_GSM){
				switch(Pos){
					case 0:
						if(_LogView){
							if(OUTPUT_LEVEL_UART_GSM){
								Serial.print(F("Не зарегистрирован в сети: "));
								Serial.println(F("Нет поиска сети"));
							}
						}
						StateGSM.GSM_Registered = false;
						goto StopCheck;
					case 1:
						if(_LogView){
							if(OUTPUT_LEVEL_UART_GSM){
								Serial.println(F("Зарегистрирован в сети"));
							}
						}
						StateGSM.GSM_Registered = true;
						goto StopCheck;
					case 2:
						if(_LogView){
							if(OUTPUT_LEVEL_UART_GSM){
								Serial.println(F("Поиск сети"));
							}
						}
						StateGSM.GSM_Registered = false;
						goto StopCheck;
					case 3:
						if(_LogView){
							if(OUTPUT_LEVEL_UART_GSM){
								Serial.println(F("Регистрация отклонена"));
							}
						}
						StateGSM.GSM_Registered = false;
						goto StopCheck;
					case 4:
						if(_LogView){
							if(OUTPUT_LEVEL_UART_GSM){
								Serial.println(F("Неизвестно"));
							}
						}
						StateGSM.GSM_Registered = false;
						goto StopCheck;
				}
			}
			StateGSM.Code_Connect_GSM = Pos;
		}
	}
	
	if(_LogView){
		if(OUTPUT_LEVEL_UART_GSM){
			Serial.println();
		}
	}
	StateGSM.GSM_Registered = false;
	// -------------------------------
	StopCheck:
	// -------------------------------
	if(OUTPUT_LEVEL_UART_GSM){}
	gsm_dtr_on();											// Усыпляем модуль
	return StateGSM.GSM_Registered;							// Возвращаем только true или false. Остальные состояния можно поглядеть в StateGSM.Code_Connect_GSM
}


// =========================================================================================================
// ========================================== Очистка очереди СМС ==========================================
// =========================================================================================================
void clearsms() {
	Serial3.println(F("AT+CMGD=4"));
}
	

// =========================================================================================================
// =========================================================================================================
// =========================================================================================================
boolean CheckPhone(String _Line) {				// Выборка всех телефонов из списка
	for (byte i = 0; i < CountPhone; i++) {
		if (_Line.indexOf(AllowPhone[i]) > -1) {
			RingPhone = AllowPhone[i];
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println("Phone: " + RingPhone);
			}
			return true;
		}
	}
	return false;
}


// ========================================================================================================
// ============================= Проверка ответа от Web сервера на GET запрос =============================
// ========================================================================================================
void Answer_check_GET(String _Text, bool _LogView){
	
	StateGSM.Code_Error_Sent_GET = 255;				// default ошибка "100"
	
	if(OUTPUT_LEVEL_UART_GSM){
		if(_LogView){
			Serial.print(F("Response from WEB server: "));
		}
	}

	String _Response[] = {	(F("404")),				// Страница не найдена
							(F("200")),				// GET запрос успешно получен!!!
							(F("603")),				// Сервер не доступен
							(F("408"))};			// Время ожидания сервером передачи от клиента истекло

	for(byte Pos = 0; Pos < sizeof(_Response) / sizeof(_Response[0]); Pos ++){						// Ищем в списке возможных ответов
		if(_Text.lastIndexOf(_Response[Pos]) != -1){				// И если нашли совпадение
			switch(Pos){
				case 0:
					StateGSM.Code_Error_Sent_GET = 404;
					if(OUTPUT_LEVEL_UART_GSM){
						if(_LogView){
							Serial.println(F("Page not found"));
						}
					}
					goto StopCheck;
				case 1:
					StateGSM.Code_Error_Sent_GET = 200;
					if(OUTPUT_LEVEL_UART_GSM){
						if(_LogView){
							Serial.println(F("OK"));
						}
					}
					goto StopCheck;
				case 2:
					StateGSM.Code_Error_Sent_GET = 603;
					if(OUTPUT_LEVEL_UART_GSM){
						if(_LogView){
							Serial.println(F("Server not available"));
						}
					}
					goto StopCheck;
				case 3:
					StateGSM.Code_Error_Sent_GET = 408;
					if(OUTPUT_LEVEL_UART_GSM){
						if(_LogView){
							Serial.println(F("Request Timeout"));
						}
					}
					goto StopCheck;
			}
		}
	}
	if(OUTPUT_LEVEL_UART_GSM){
		if(_LogView){
			Serial.println(F("Неизвестно"));
		}
	}
	// -------------------------------
	StopCheck:
	// -------------------------------
	if(OUTPUT_LEVEL_UART_GSM){}
}


// ======================================================================================================
// ======================================================================================================
// ======================================================================================================
void _ViewErrorConnectGPRS(byte _ErrorConnectGPRS, byte _lang){
	switch(_ErrorConnectGPRS){
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
// ===================================== Проверка регистрации GPRS ======================================
// ======================================================================================================
bool CheckConnectionGPRS(){
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
	
	if(Error.lastIndexOf(F("+SAPBR")) != -1){				// Проверяем правильность ответа, что он именно на наш запрос
		
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
					Pos = 4;								// Увеличиваем счетчик чтобы прекратить выполнение цикла
			}
			if(Pos == 3){									// Если перебрали все ответы и не нашли совпадение, то считаем что модуль ничего не ответил
				StateGSM.GPRS_Connect = false;
				StateGSM.Code_Connect_GPRS = 4;				// Поднимаем ошибку "4"
			}
		}
	}
	else{ 
		if(OUTPUT_LEVEL_UART_GSM){
			Serial.println();								// Простой перевод строки если нет ответа. Сделано для красоты
		}
	}
	return StateGSM.GPRS_Connect;
}



// =========================================================================================================
// ================================== Обработка комманд полученных по СМС ==================================
// =========================================================================================================
void MasterSMS(String _Line) {
	wdt_reset();
	#define DelayTime 500
	_Line.toLowerCase();								// Преобразовываем строку в нижний регистр
	// ======================================== Перезагрузка контроллера =========================================
	if ((_Line.indexOf(F("reset")) > -1)) {
		EEPROM.update(E_RebutingFromGSM, 1);		// Поднимаем флаг что контроллер отправлен в перезагрузку
		resetFunc();								// reset
	}
	// ======================================== Перезагрузка GPRS ================================================
	if ((_Line.indexOf(F("gprs_rst")) > -1)) {
		EEPROM.write(E_AllowGPRS, 1);				// Разрешаем работу GPRS
		Serial3.println(F("AT+SAPBR=0,1"));			// Разрываем GPRS соединение
		delay(50);
		InitializingGPRS();
	}
	// ======================================= Включение GPRS ====================================================
	if ((_Line.indexOf(F("gprs_on")) > -1)) {
		EEPROM.write(E_AllowGPRS, 1);				// Разрешаем работу GPRS
		delay(50);
		InitializingGPRS();							// Повторно инициализируем GPRS
	}
	// ====================================== Выключение GPRS ====================================================
	if ((_Line.indexOf(F("gprs_off")) > -1)) {
		EEPROM.write(E_AllowGPRS, 0);				// Запрещаем работу GPRS
		Serial3.println(F("AT+SAPBR=0,1"));			// Разрываем GPRS соединение
	}
	// ===========================================================================================================
	if ((_Line.indexOf(F("status")) > -1)) {
		if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL){
			Serial.println(F("Status"));
		}
		SendSMS(String	(F("Temp_Box: "))	+ RealValueSensors[SENSOR_0][VALUE_1]	+	(F(", ")) +
						(F("Temp_UP: "))	+ RealValueSensors[SENSOR_8][VALUE_1]	+	(F(", ")) +
						(F("Temp_DOWN: "))	+ RealValueSensors[SENSOR_5][VALUE_1]	+	(F(", ")) +
						(F("Temp_Left: "))	+ RealValueSensors[SENSOR_7][VALUE_1]	+	(F(", ")) +
						(F("Temp_Right: ")) + RealValueSensors[SENSOR_2][VALUE_1]	+	(F(", ")) +
						(F("VCC: ")) + VCC, 4);
	}
	// ===========================================================================================================
	if ((_Line.indexOf(F("temp")) > -1)) {
		if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL){
			Serial.println(F("temp"));
		}
		SendSMS(String	(F("Temp_UP: "))	+ RealValueSensors[SENSOR_2][VALUE_1] + (F(", ")) +
						(F("Temp_DOWN: "))	+ RealValueSensors[SENSOR_3][VALUE_1] + (F(", ")) , 1);
	}
	RingPhone = "";								// Очищаем номер автора смс
	clearsms();									// Очищаем очередь входящих СМС
	gsm_dtr_on();								// Усыпляем модуль
}


// =========================================================================================================
// =========================================================================================================
// =========================================================================================================
void serial3ISR(){
	wdt_reset();
	String _val = "";
	if (Serial3.available()) {
		while (Serial3.available()) {
			_val += char(Serial3.read());
			delay(20);
		}
		if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL){
			_val.trim();
			Serial.println();
			Serial.print(F("GSM input: ")); Serial.println(_val);
			Serial.println();
		}
		// =========================================================================================================
		if (_val.indexOf(F("+CMTI")) > -1) {			// Пришло СМС сообщение
			if (CheckPhone(_val)) {					// Если СМС от разрешенного абонента
				Serial3.println(F("СМС_1"));
				MasterSMS(_val);						// Запускаем проверку его содержимого
			}
		}
		// =========================================================================================================
		if (_val.indexOf(F("+CLIP")) > -1) {			// Пришел звонок
			if (CheckPhone(_val)) {					// Если звонок от разрешенного абонента
				Serial3.println(F("ATH0"));
			}
			else {
				Serial3.println(F("ATH0"));
			}
		}
		// =========================================================================================================
		if (_val.indexOf(F("+HTTPACTION:")) > -1) {		// Пришел ответ от Web сервера на отправку GET
			Answer_check_GET(_val, ON);
		}
		// =========================================================================================================
		if (_val.indexOf(F("+CUSD:")) > -1) {		// Пришло уведомление о USSD-ответе
			if (_val.indexOf(F("\"")) > -1) {		// Если ответ содержит кавычки, значит есть сообщение (предохранитель от "пустых" USSD-ответов)
				delay(1000);
			}
			if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL){
				Serial3.println(F("AT+CUSD=0"));
			}
		}
	}
}
