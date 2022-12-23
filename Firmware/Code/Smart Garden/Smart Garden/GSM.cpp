#include <Arduino.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include "GSM.h"
#include "main.h"
#include "Sensors.h"
#include "Warning.h"
#include "LCDdisplay.h"


String Link_LogWebServer = "";
String Link_LogDataWebServer = "";

String GPRS_APN_NAME = "";
String GPRS_APN_USER = "";
String GPRS_APN_PASSWORD = "";

String GSM_CODE_BALANCE = "";

String GSM_GET_Tab_1 = "-- ";
String GSM_GET_Tab_2 = "---- ";

String RingPhone = "";

String AllowPhone[4];													

byte TurnOfMessagesGSM[ExtentOfTurn][QuantityParametersInMessage];		// Очередь СМС сообщений (максимально 10 исходящих сообщений)


struct StructGPRSConnection StateGSM;


void Write_To_Queue_GSM(struct MessageQueueGSM Par){
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
	/*
		_State - что выполнять:	
								RESET	- перезагрузку модуля
								OFF		- выключение питания
								ON		- включение питания
	*/
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
				Serial.println(F("Power on VCC...OK"));
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
void Initializing_GSM(bool _out_to_lcd){
	//wdt_reset();
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
	if(_out_to_lcd){
		WriteToLCD(String(F("- AT: ")), LCD_LINE_4, LCD_START_SYMBOL_2, LCD_NO_SCREEN_REFRESH_DELAY);
	}
	for (byte i = 0; i < sizeof(_GSMATs) / sizeof(_GSMATs[0]); i++) {
 		if(_out_to_lcd){
 			WriteToLCD(String(F("*")), LCD_LINE_4, LCD_START_SYMBOL_8 + i, LCD_NO_SCREEN_REFRESH_DELAY);
 		}
		send_AT_Command(_GSMATs[i], GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);					// Отправляем АТ команду, ждем ответ и выводим ответ в Serial
	}
	// ----------------------------------------------------------------------------------------
	byte Exit = 1;
	while(Exit <= 6){																			// Максимальное время ожидания регистрации 10 сек
		if(Check_Registration_GSM(ON, LCD_ALLOW_OTPUT_ON_SCREEN) == GSM_REGISTERED){			// Если GSM модуль зарегистрирован в сети
			Exit = 6;
		}
		Exit++;		
		_delay_ms(1000);
	}
	// ----------------------------------------------------------------------------------------
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.println(F("We resolve sleep mode...OK"));
	}
	Serial3.println(F("AT+CSCLK=1"));								// Разрешаем спящий режим
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.println(F("================= Initializing complete ================="));
	}
	gsm_dtr_on();													// Усыпляем модуль
}


// ======================================================================================================
// ========================================= Инициализация GPRS =========================================
// ======================================================================================================
void Initializing_GPRS(){
	String _GPRS_ATs[] = {																// массив АТ команд инициализации GPRS
							(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"")),					// Установка настроек подключения
							("AT+SAPBR=3,1,\"APN\",\"") + GPRS_APN_NAME + ("\""),		// Имя APN точки доступа
							("AT+SAPBR=3,1,\"USER\",\"") + GPRS_APN_USER + ("\""),		// Имя пользователя
							("AT+SAPBR=3,1,\"PWD\",\"") + GPRS_APN_PASSWORD + ("\""),	// Пароль							
							(F("AT+SAPBR=1,1")),										// Установка GPRS соединения
							(F("AT+HTTPINIT")),											// Инициализация http сервиса
	};
	String _Word;
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.println(F("============= Start GPRS ============="));
	}
	if(ControllerSetup){
		WriteToLCD(String(F("- AT: ")), LCD_LINE_3, LCD_START_SYMBOL_2, LCD_NO_SCREEN_REFRESH_DELAY);
		WriteToLCD(String(F("                    ")), LCD_LINE_4, LCD_START_SYMBOL_1, LCD_NO_SCREEN_REFRESH_DELAY);
	}
	for (byte i = 0; i < sizeof(_GPRS_ATs) / sizeof(_GPRS_ATs[0]); i++) {
		wdt_reset();
		if(ControllerSetup){
			WriteToLCD(String(F("*")), LCD_LINE_3, LCD_START_SYMBOL_8 + i, LCD_NO_SCREEN_REFRESH_DELAY);
		}
		_Word = send_AT_Command(_GPRS_ATs[i], GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);// Отправляем АТ команду, ждем, получаем и выводим ответ в Serial
		byte TimerCommand = 1;
		while(TimerCommand <= 4){														// Максимальное кол-во отправок команды
			if(_Word.lastIndexOf(F("OK")) != -1){										// Если ответ "OK"
				TimerCommand = 5;														// Останавливаем выполнение цикла while
				goto end_while;
			}
			else{
				_Word = send_AT_Command(_GPRS_ATs[i], GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);			// Повторно отправляем команду
				TimerCommand ++;
			}
		}
		end_while: ;
	}
	if(ControllerSetup){
		WriteToLCD(String(F("OK")), LCD_LINE_3, LCD_START_SYMBOL_19, LCD_SCREEN_REFRESH_DELAY);
		
	}
	Check_Connection_GPRS(ON);															// Проверяем подключение
	if(ControllerSetup){
		WriteToLCD(String(F("- IP: ")), LCD_LINE_4, LCD_START_SYMBOL_2, LCD_NO_SCREEN_REFRESH_DELAY);
		WriteToLCD(StateGSM.IP_GPRS, LCD_LINE_4, LCD_START_SYMBOL_7, LCD_SCREEN_REFRESH_DELAY);
	}
}


// ======================================================================================================
// ========================================= Подключение к GPRS =========================================
// ======================================================================================================
void Connecting_GPRS(bool _out_to_lcd){
	if(EEPROM.read(E_AllowGPRS) == ON){							// Если разрешена работа GPRS
		if(ControllerSetup){									// Если контроллер в режиме запуска, то сразу подключаемся к GPRS.
			// Проверку подключения к GSM провели в цикле его инициализации
			if(StateGSM.Code_Connect_GSM == 1){					// Если зарегистрирован в сети
				if(_out_to_lcd){
					WriteToLCD(String(F("=== Connect GPRS ===")), LCD_LINE_2, LCD_START_SYMBOL_1, LCD_NO_SCREEN_REFRESH_DELAY);
					Clean_LCD(LCD_LINE_3, LCD_START_SYMBOL_1);
					Clean_LCD(LCD_LINE_4, LCD_START_SYMBOL_1);
				}
				Initializing_GPRS();								// Инициализируем GPRS
			}
		}
		else{														// Если контроллера в режиме обычной работы, то нужно перед иницализацией приверить подключение GSM
			byte a = 1;
			while(a <= 5){																	// Максимальное время ожидания регистрации GSM (5 сек)
				if(Check_Registration_GSM(ON, LCD_NO_OTPUT_ON_SCREEN) == GSM_REGISTERED){	// Если GSM модуль зарегистрирован в сети
					//_delay_ms(50);
					Initializing_GPRS();							// Инициализируем GPRS
					break;
				}
				a++;
				_delay_ms(1000);
			}
		}
		Check_Connection_GPRS(GSM_NO_OUTPUT_TO_SERIAL);				// Проверяем подключиление к GPRS
	}
}


// =========================================================================================================
// ======================= Функция ожидания ответа и возврата полученного результата =======================
// =========================================================================================================
String wait_Response(bool _logView) {
	String _resp = "";															// Переменная для хранения результата
	long _timeout = millis() + EEPROM.read(E_MaximumTimeResponseGSM) * 1000;	// Переменная для отслеживания таймаута (10 секунд)
	while (!Serial3.available() && millis() < _timeout){						// Ждем ответа 10 секунд и обнуляем собаку чтобы не перезагрузить контроллер,
		wdt_reset();															// сбрасываем собаку чтобы она не сбросила контроллер во время ожидания
	}	
	if (Serial3.available()) {													// Если есть, что считывать
		_resp = Serial3.readString();											// считываем и запоминаем
	}
	else {																		// Если пришел таймаут, то
		if(OUTPUT_LEVEL_UART_GSM && _logView){
			Serial.println(F("Timeout..."));									// ... оповещаем об этом и
		}
	}
	return _resp;																// ... возвращаем результат. Пусто, если проблемма
}


// ========================================================================================================
// ==================================== Отправка AT команды GSM модулю ====================================
// ========================================================================================================
String send_AT_Command(String _Command, bool _waiting, bool _logView) {
	/*
		_Command	- команда
		_waiting	- требуется или нет дождаться ответа от модуля
		_LogView	- выводить или нет сообщение в UART. Системная переменная, никак не настраивается в отличии от OUTPUT_LEVEL_UART_GSM
	*/
	wdt_reset();
	String _resp = "";											// Переменная для хранения результата
	if(OUTPUT_LEVEL_UART_GSM){
		if(_logView){
			Serial.println(_Command);							// Дублируем команду в UART
		}
	}
	Serial3.println(_Command);									// Отправляем команду модулю

	if (_waiting) {												// Если необходимо дождаться ответа...
		_resp = wait_Response(_logView);						// ... ждем, когда будет передан ответ
		//Если Echo Mode выключен (ATE0), то эти 3 строки можно закомментировать
		if (_resp.startsWith(_Command)) {						// Убираем из ответа дублирующуюся команду
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
void Send_SMS(String _Text, byte _Level) {	
	/*
		Text - текст сообщения
		PhoneNumber - номер телефона получателя
		Level - уровень важности сообщения. От этого зависит кому отправлять СМС
	*/	
	if(StateGSM.GSM_Registered){				// и если зарегистрирован
		String Phone;
		switch(_Level){
			case GSM_INFO_SMS:					// info — обычные сообщения
				//Phone = PhoneNumber;
				Phone = (F("+79139045925"));
				break;
			case GSM_ANSWER_SMS:				// Ответ на входящее СМС
				Phone = RingPhone;
				break;
			case GSM_DEBUG_SMS:					// debug — Подробная информация для отладки			
				break;
			case GSM_WARNING_SMS:				// warning — Исключительные случаи, но не ошибки
				//Phone = NumberForWarningSMS[0];
				Phone = (F("+79139045925"));
				break;
			case GSM_ERROR_SMS:					// error — Ошибки исполнения, не требующие сиюминутного вмешательства
				Phone = (F("+79139045925"));
				break;
			case GSM_CRITICAL_SMS:				// critical — Критические состояния
				break;
			case GSM_EMERGENCY_SMS:				// emergency — Система не работает
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
void Send_GET_request(String _Text, bool _waiting, bool _LogView, byte _Request_Type){
	/*
		_Text			- Текст запроса
		_waiting		- Требуется или нет дождаться ответа от модуля
		_LogView		- Выводить или нет сообщение в UART. Системная переменная, никак не настраивается в отличии от OUTPUT_LEVEL_UART_GSM
		_Request_Type	- Тип запроса
	*/
	bool ResendingGET = false;
	byte ResendCounterResendingGET = 2;															// Счетчик повторных отправок Get запросов (2 шт)
	wdt_reset();
	if(EEPROM.read(E_AllowGPRS) == ON /*&& StateGSM.GPRS_Connect*/){								// Если разрешена работа GPRS и соединение установлено
		if(OUTPUT_LEVEL_UART_GSM && _LogView){
			switch(_Request_Type){
				case GET_VALUE_REQUEST:
					Serial.print(F("Sending a GET value sensor's request: "));
					break;
				case GET_LOG_REQUEST:
					Serial.print(F("Sending a GET log request: "));
					break;
			}
		}
		send_AT_Command(F("AT+HTTPPARA=\"CID\",1"), GSM_NO_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);// Установка CID параметра для http сессии
		SentGET:																					// Метка для повторной отправки GET запроса
		String Answer = send_AT_Command(_Text, _waiting, GSM_NO_OUTPUT_TO_SERIAL);					// Шлем сам запрос и ждем ответ		
		if(Answer.lastIndexOf(F("OK")) != -1){														// Если запрос успешно отправлен, модуль вернул ответ "ОК"
			StateGSM.Error_Sent_GET = false;														// Снимаем флаг возможной ошибки
			if(OUTPUT_LEVEL_UART_GSM && _LogView){
				Serial.println(F("OK"));
			}
		}
		else{														// и если не отправлен
			StateGSM.Error_Sent_GET = true;							// Поднимаем флаг ошибки
			ResendingGET = true;									// Флаг чтобы повторного отправить GET
			if(OUTPUT_LEVEL_UART_GSM && _LogView){
				Serial.println(F("ERROR"));
			}
		}
		if(ResendCounterResendingGET == 2 && ResendingGET){			// Переводим код на метку для повторной отправки
			if(OUTPUT_LEVEL_UART_GSM && _LogView){
				Serial.print(F("\tResending a GET request: "));
			}
			ResendCounterResendingGET --;
			goto SentGET;
		}
		send_AT_Command(F("AT+HTTPACTION=0"), GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);			// Закрытие http сессии
	}
}


// ========================================================================================================
// ================================ Проверка готовности SIM-карты (AT+CPIN?) ==============================
// ========================================================================================================
byte SIM_card_readiness_check(byte _Logview){
	bool _State = false;
	String _ANSWER[] = {							// массив ответов на команду
							(F("READY")),			// MT is not pending for any password
							(F("SIM	PIN")),			// MT is waiting SIM PIN to be given
							(F("SIM PUK")),			// MT is waiting for SIM PUK to be given
							(F("PH_SIM PIN")),		// ME is waiting for phone to SIM card (antitheft)
							(F("PH_SIM PUK")),		// ME is waiting for SIM PUK (antitheft)
							(F("SIM PIN2")),		// PIN2, e.g. for editing the FDN book possible only if preceding Command was acknowledged with +CME ERROR: 17
							(F("SIM PUK2")),		// Possible only if preceding Command was acknowledged with error +CME ERROR: 18
	};
	String Return = send_AT_Command(F("AT+CPIN?"), GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);		// Запрос статуса SIM карты
	if(_Logview){
		Clean_LCD(LCD_LINE_3, LCD_START_SYMBOL_1);
		WriteToLCD(String(F("- SIM card:")), LCD_LINE_3, LCD_START_SYMBOL_2, LCD_NO_SCREEN_REFRESH_DELAY);
	}
	if(Return.lastIndexOf(F("+CPIN")) != -1){										// Если вернулся правильный ответ, а не какой нибудь мусор
		for (byte Pos = 0; Pos < sizeof(_ANSWER) / sizeof(_ANSWER[0]); Pos++) {		// Проходим по всему массиву ответов
			wdt_reset();
			if(Return.lastIndexOf(_ANSWER[Pos]) != -1){								// Если нашли совпадение			
				switch(Pos){
					case GSM_SIM_READY:												// Если SIM карта готова (ответ "READY")					
						WriteToLCD(String(F("READY")), LCD_LINE_3, LCD_START_SYMBOL_16, LCD_SCREEN_REFRESH_DELAY);
						goto StopCheck;
					case GSM_SIM_PIN:
						WriteToLCD(String(F("PIN")), LCD_LINE_3, LCD_START_SYMBOL_18, LCD_SCREEN_REFRESH_DELAY);
						goto StopCheck;
					case GSM_SIM_PUK:
						WriteToLCD(String(F("PUK")), LCD_LINE_3, LCD_START_SYMBOL_18, LCD_SCREEN_REFRESH_DELAY);
						goto StopCheck;
					case GSM_PH_SIM_PIN:
					
						goto StopCheck;
					case GSM_PH_SIM_PUK:
					
						goto StopCheck;
					case GSM_SIM_PIN2:
					
						goto StopCheck;
					case GSM_SIM_PUK2:
					
						goto StopCheck;
				}
				_State = Pos;
			}
		}
	}	
	StopCheck:
	return _State;
}


// ========================================================================================================
// =================================== Уровень сигнала GSM сети (AT+CSQ) ==================================
// ========================================================================================================
void Signal_Level(bool _LogView, bool _Perform_Measurement){
	/*
		_LogView				- выводить или нет сообщение в UART. Системная переменная, никак не настраивается в отличии от OUTPUT_LEVEL_UART_GSM
		_Perform_Measurement	- нужно измерить уровень сигнала или только вывести его уровень в UART
		
	// Проверка уровня сигнала. Первая цифра — уровень сигнала <rssi>:
	// 0			- 115 dBm or меньше					- Превосходно
	// 1			- 111 dBm							- Отлично
	// 2…30 -110…	- 54 dBm							- ОК
	// 31			- 52 dBm or больше					- Плохо
	// 99			- не известно или не обнаруживается	- Нет
	*/

	if(OUTPUT_LEVEL_UART_GSM && _LogView || ControllerSetup){
		Serial.print(F("Уровень сигнала GSM: "));
	}
		
	int _Level = 99;
	
	if(_Perform_Measurement){																		// Если запросили измерение уровня
		String _Return = send_AT_Command(F("AT+CSQ"), GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);	// Запрос уровня, обязательно дожидаемся ответа
		if(_Return.lastIndexOf(F("+CSQ")) != -1){													// Если получили правильный ответ
			_Return = _Return.substring(7,9);														// Убираем лишние символы
			_Level = _Return.toInt();																// Возвращает уровень сигнала
		}
	}
	else {
		_Level = StateGSM.GSM_Signal_Level;															// default значение. Сделано только для вывода значения с описанием без измерения
	}
	if(_Level == 0){
		StateGSM.GSM_Signal_Level = 115;
		if(OUTPUT_LEVEL_UART_GSM && _LogView){
			Serial.println(F("-115 dBm и меньше"));
		}
		goto Stop;
	}
	else if(_Level == 1){
		StateGSM.GSM_Signal_Level = 111;
		if(OUTPUT_LEVEL_UART_GSM && _LogView){
			Serial.println(F("-111 dBm"));
		}
		goto Stop;
	}
	else if(2 <= _Level && _Level <= 30){
		StateGSM.GSM_Signal_Level = map(_Level, 2, 30, 110, 54);
		if(OUTPUT_LEVEL_UART_GSM && _LogView){
			Serial.print(F("-"));
			Serial.print(StateGSM.GSM_Signal_Level);
			Serial.println(F(" dBm"));
		}
		goto Stop;
	}
	else if(_Level == 31){
		StateGSM.GSM_Signal_Level = 52;
		if(OUTPUT_LEVEL_UART_GSM && _LogView){
			Serial.println(F("-52 dBm и сильнее"));
		}
		goto Stop;
	}
	else if(_Level == 99){
		StateGSM.GSM_Signal_Level = 99;
		if(OUTPUT_LEVEL_UART_GSM && _LogView){
			Serial.println(F("нет сигнала"));
		}
		goto Stop;
	}
	Serial.println(F("Неизвестно"));
	Stop:
	
	if(LightLCDEnable || ControllerSetup){				// Если включена подсветка
		ViewSignalLevel(StateGSM.GSM_Signal_Level);		// Обновляем значек на LCD экране
	}
}


// ========================================================================================================
// ===================== Проверка готовности модуля. Отправка и проверка команды "AT" =====================
// ========================================================================================================
byte Check_Readiness_GSM_Module(boolean _LogView, bool _out_to_lcd){					 
	if(ControllerSetup){
		WriteToLCD(String(F("- Readiness:      ")), LCD_LINE_3, LCD_START_SYMBOL_2, LCD_NO_SCREEN_REFRESH_DELAY);
	}
	
	if(OUTPUT_LEVEL_UART_GSM && _LogView){
		Serial.print(F("Состояние GSM модуля (\"AT\"): "));
	}
	String _Response[] = {	(F("NOT INSERTED")),								// Нет SIM карты
							(F("OK"))};											// Зарегистрирован в сети
	
	String _Error = send_AT_Command(F("AT"), GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);	// Отправляем комманду, ждем ответ, но в Serial его не выводим
	
	for(byte Pos = 0; Pos < sizeof(_Response) / sizeof(_Response[0]); Pos ++){				// Ищем в списке возможных ответов
		if(_Error.lastIndexOf(_Response[Pos]) != -1){										// И если нашли совпадение
			StateGSM.State_GSM_Module = Pos;
			switch(Pos){
				case GSM_MODULE_NOT_SIM_CARD:														// Нет SIM карты
					if(_out_to_lcd){
						lcd.setCursor(LCD_START_SYMBOL_20, LCD_LINE_1);
						lcd.print(char(LCD_ICON_NOT_SIM));
						WriteToLCD(String(F("Not SIM")), LCD_LINE_3, LCD_START_SYMBOL_14, LCD_SCREEN_REFRESH_DELAY);
					}
					if(OUTPUT_LEVEL_UART_GSM && _LogView){
						Serial.print(F("Нет SIM карты"));
					}
					goto Exit;
				case GSM_MODULE_READY:														// Готов
					if(_out_to_lcd){
						WriteToLCD(String(F("OK")), LCD_LINE_3, LCD_START_SYMBOL_19, LCD_SCREEN_REFRESH_DELAY);
					}
					if(OUTPUT_LEVEL_UART_GSM && _LogView){
						Serial.println(F("OK"));
					}
					goto Exit;
			}
		}
		if(Pos == sizeof(_Response) / sizeof(_Response[0]) - 1){							// Прошлись по всем возможным ответам и не нашли совпадения
			if(_out_to_lcd){
				WriteToLCD(String(F("Unknown")), LCD_LINE_3, LCD_START_SYMBOL_14, LCD_SCREEN_REFRESH_DELAY);
			}
			if(OUTPUT_LEVEL_UART_GSM && _LogView){
				Serial.println(F("Неизвестно"));
			}
			StateGSM.State_GSM_Module = GSM_MODULE_UNKNOWN;
		}
	}
	Exit:
	return StateGSM.State_GSM_Module;
}


// ========================================================================================================
// =================== Проверка зарегистрировался или нет модуль в сети GSM (AT+CREG?) ====================
// ========================================================================================================
byte Check_Registration_GSM(bool _LogView, bool _out_to_lcd){				
	/*
		Ф-ция возвращает только true или false. 
		Остальные состояния можно посмотреть в переменной StateGSM.Code_Connect_GSM
	*/
	
	if(OUTPUT_LEVEL_UART_GSM && _LogView){
		Serial.print(F("Состояние GSM: "));
	}
	
	String _Response[] = {	(F("+CREG: 0,0")),				// Не зарегистрирован в сети
							(F("+CREG: 0,1")),				// Зарегистрирован в сети
							(F("+CREG: 0,2")),				// Поиск сети
							(F("+CREG: 0,3")),				// Регистрация отклонена
							(F("+CREG: 0,4"))};				// Неизвестно
	
	String _Error = send_AT_Command(F("AT+CREG?"), GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);	// Отправляем команду, ждем ответ, но в Serial его не выводим
	//_Error = _Response[3];
	StateGSM.GSM_Registered = false;											// default состояние
	if(_out_to_lcd){
		Clean_LCD(LCD_LINE_4, LCD_START_SYMBOL_4);
		WriteToLCD(String(F("- ")), LCD_LINE_4, LCD_START_SYMBOL_2, LCD_NO_SCREEN_REFRESH_DELAY);
	}
	for(byte Pos = 0; Pos < sizeof(_Error) / sizeof(_Error[0]); Pos ++){		// Ищем в списке возможных ответов
		if(_Error.lastIndexOf(_Response[Pos]) != -1){							// И если нашли совпадение
			StateGSM.Code_Connect_GSM = Pos;
			switch(Pos){
				case GSM_NO_FIND_NETWORK:
					if(OUTPUT_LEVEL_UART_GSM && _LogView){
						Serial.print(F("Не зарегистрирован в сети: "));
						Serial.println(F("Нет поиска сети"));
					}
					if(_out_to_lcd){
						WriteToLCD(String(F("No find network ")), LCD_LINE_4, LCD_START_SYMBOL_4, LCD_SCREEN_REFRESH_DELAY);
					}
					goto StopCheck;
				case GSM_REGISTERED:
					if(OUTPUT_LEVEL_UART_GSM && _LogView){
						Serial.println(F("Зарегистрирован в сети"));
					}
					if(_out_to_lcd){
						WriteToLCD(String(F("Registration OK ")), LCD_LINE_4, LCD_START_SYMBOL_4, LCD_SCREEN_REFRESH_DELAY);
					}
					StateGSM.GSM_Registered = true;
					goto StopCheck;
				case GSM_FINDING_NETWORK:
					if(OUTPUT_LEVEL_UART_GSM && _LogView){
						Serial.println(F("Поиск сети"));
					}
					if(_out_to_lcd){
						WriteToLCD(String(F("Finding network ")), LCD_LINE_4, LCD_START_SYMBOL_4, LCD_SCREEN_REFRESH_DELAY);
					}
					goto StopCheck;
				case GSM_REGISTRED_REJECTED:
					if(OUTPUT_LEVEL_UART_GSM && _LogView){
						Serial.println(F("Регистрация отклонена"));
					}
					if(_out_to_lcd){
						WriteToLCD(String(F("Registr rejected")), LCD_LINE_4, LCD_START_SYMBOL_4, LCD_SCREEN_REFRESH_DELAY);
					}
					goto StopCheck;
				case GSM_REGISTERED_UNKNOWN:
					if(OUTPUT_LEVEL_UART_GSM && _LogView){
						Serial.println(F("Неизвестно"));
					}
					if(_out_to_lcd){
						WriteToLCD(String(F("Unknown         ")), LCD_LINE_4, LCD_START_SYMBOL_4, LCD_SCREEN_REFRESH_DELAY);
					}
					goto StopCheck;
			}
		}
	}	
	
	if(OUTPUT_LEVEL_UART_GSM && _LogView){
		Serial.println();
	}
	StateGSM.GSM_Registered = false;
	// -------------------------------
	StopCheck:
	// -------------------------------
	if(OUTPUT_LEVEL_UART_GSM){}
	gsm_dtr_on();											// Усыпляем модуль
	return StateGSM.Code_Connect_GSM;						// Возвращаем только true или false. Остальные состояния можно поглядеть в StateGSM.Code_Connect_GSM
}


// =========================================================================================================
// ========================================== Очистка всех СМС ==========================================
// =========================================================================================================
void Clear_SMS(byte Type){
	switch(Type){
		case DEL_READ:						// Удалить все прочитанные SMS
			send_AT_Command(F("AT+CMGDA=\"DEL READ\""), GSM_NO_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);
			break;
		case DEL_UNREAD:					// Удалить все непрочитанные SMS
			send_AT_Command(F("AT+CMGDA=\"DEL UNREAD\""), GSM_NO_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);
			break;
		case DEL_SENT:						// Удалить все отправленные SMS
			send_AT_Command(F("AT+CMGDA=\"DEL SENT\""), GSM_NO_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);
			break;
		case DEL_UNSENT:					// Удалить все неотправленные SMS
			send_AT_Command(F("AT+CMGDA=\"DEL UNSENT\""), GSM_NO_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);
			break;
		case DEL_INBOX:						// Удалить все полученные SMS
			send_AT_Command(F("AT+CMGDA=\"DEL INBOX\""), GSM_NO_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);
			break;
		case DEL_ALL:						// Удалить все SMS
			send_AT_Command(F("AT+CMGDA=\"DEL ALL\""), GSM_NO_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);
			break;
	}
	//sendATCommand(F("AT+CMGD=4"), GSM_NO_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);
								
	//sendATCommand(F("AT+CMGL=\"ALL\",1"), GSM_NO_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);
	
}
	

// =========================================================================================================
// =========================================================================================================
// =========================================================================================================
boolean Check_Phone(String _Line) {					// Выборка всех телефонов из списка
	for (byte i = 0; i <= ARRAY_SIZE(AllowPhone)/*3*//*CountPhone*/; i++) {
		//Serial.print("CountPhone: "); Serial.println(AllowPhone[i]);
		if(_Line.lastIndexOf(AllowPhone[i]) != -1){				// И если нашли совпадение
			RingPhone = AllowPhone[i];
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println("Phone: " + RingPhone);
			}
			return true;
		}
	}
	return false;
}


// =========================================================================================================
// ======================================== Проверка баланса SIM карты =====================================
// =========================================================================================================
float Card_Balance_Check(String Text){	
	Text.remove(1,17);
 	Text.remove(Text.length()-8);
 	Text.trim();
	
	return Text.toFloat();							// Возвращаем полученное число
}


// ========================================================================================================
// ============================= Проверка ответа от Web сервера на GET запрос =============================
// ========================================================================================================
void Answer_check_GET(String _Text, bool _LogView){
	
	StateGSM.Code_Error_Sent_GET = 255;				// default ошибка "100"
	
	if(OUTPUT_LEVEL_UART_GSM && _LogView){
		Serial.print(F("Response from WEB server: "));
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
					if(OUTPUT_LEVEL_UART_GSM && _LogView){
						Serial.println(F("Page not found"));
					}
					goto StopCheck;
				case 1:
					StateGSM.Code_Error_Sent_GET = 200;
					if(OUTPUT_LEVEL_UART_GSM && _LogView){
						Serial.println(F("OK"));
					}
					goto StopCheck;
				case 2:
					StateGSM.Code_Error_Sent_GET = 603;
					if(OUTPUT_LEVEL_UART_GSM && _LogView){
						Serial.println(F("Server not available"));
					}
					goto StopCheck;
				case 3:
					StateGSM.Code_Error_Sent_GET = 408;
					if(OUTPUT_LEVEL_UART_GSM && _LogView){
						Serial.println(F("Request Timeout"));
					}
					goto StopCheck;
			}
		}
	}
	if(OUTPUT_LEVEL_UART_GSM && _LogView){
		Serial.println(F("Неизвестно"));
	}
	// -------------------------------
	StopCheck:
	// -------------------------------
	if(OUTPUT_LEVEL_UART_GSM){}
}


// ======================================================================================================
// ===================================== Проверка регистрации GPRS ======================================
// ======================================================================================================
bool Check_Connection_GPRS(bool _LogView){
	StateGSM.GPRS_Connect = false;				// Сбрасываем флаг подключения
	StateGSM.IP_GPRS = "0";
	
	if(OUTPUT_LEVEL_UART_GSM && _LogView){
		Serial.print(F("State GPRS Connection: "));
	}
	String Response[] = {	(F("1,0")),			// Соединение устанавливается
							(F("1,1")),			// Соединение установлено
							(F("1,2")),			// Соединение закрывается
							(F("1,3"))};		// Нет соединения
	
	String Error = StateGSM.IP_GPRS = send_AT_Command(F("AT+SAPBR=2,1"), GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);	// Узнаем параметры соединения
	
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
					case GPRS_CONNECTION_IS_ESTABLISHED:
						if(OUTPUT_LEVEL_UART_GSM && _LogView){
							Serial.println(F("Installed"));
						}
						break;
					case GPRS_CONNECTION_ESTABLISHED:								// Если соединение установлено (ответ "+SAPBR: 1,1")
						if(OUTPUT_LEVEL_UART_GSM && _LogView){
							Serial.print(F("Established ("));
							Serial.print(StateGSM.IP_GPRS);
							Serial.println(F(")"));
						}
						StateGSM.GPRS_Connect = true;								// Поднимаем флаг. Сделано для удобства
						break;
					case GPRS_CONNECTION_CLOSED:
						if(OUTPUT_LEVEL_UART_GSM && _LogView){
							Serial.println(F("Closed"));
						}
						break;
					case GPRS_NO_CONNECTED:
						if(OUTPUT_LEVEL_UART_GSM && _LogView){
							Serial.println(F("No connection"));
						}
						break;
				}
				if(OUTPUT_LEVEL_UART_GSM && _LogView){
					Serial.println(F("=============================================="));
				}
				//Pos = /*4*/sizeof(Response) / sizeof(Response[0]) + 1;	// Увеличиваем счетчик чтобы прекратить выполнение цикла
			}
			if(Pos == ARRAY_SIZE(Response)/*sizeof(Response) / sizeof(Response[0])*//*3*/){		// Если перебрали все ответы и не нашли совпадение, то считаем что модуль ничего не ответил
				//StateGSM.GPRS_Connect = false;
				StateGSM.Code_Connect_GPRS = GPRS_UNKNOWN;				// Поднимаем ошибку "GPRS_UNKNOWN"
			}
		}
	}
	else{ 
		if(OUTPUT_LEVEL_UART_GSM && _LogView){
			Serial.println();											// Простой перевод строки если нет ответа. Сделано для красоты
		}
	}
	return StateGSM.GPRS_Connect;
}



// =========================================================================================================
// ================================== Обработка комманд полученных по СМС ==================================
// =========================================================================================================
void Master_SMS(String _Line) {
	wdt_reset();
	//#define DelayTime 500
	_Line.toLowerCase();								// Преобразовываем строку в нижний регистр
	// ======================================== Перезагрузка контроллера =========================================
	if ((_Line.indexOf(F("reboot_mc")) > -1)) {
		EEPROM.update(E_RebutingFromGSM, 1);		// Поднимаем флаг что контроллер отправлен в перезагрузку
		resetFunc();								// reset
	}
	// ===========================================================================================================
	if ((_Line.indexOf(F("status")) > -1)) {
		if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL){
			Serial.println(F("Status"));
		}
		Send_SMS(String	(F("Temp_Box: "))	+ RealValueSensors[SENSOR_0][VALUE_1]	+	(F(", ")) +
						(F("Temp_UP: "))	+ RealValueSensors[SENSOR_8][VALUE_1]	+	(F(", ")) +
						(F("Temp_DOWN: "))	+ RealValueSensors[SENSOR_5][VALUE_1]	+	(F(", ")) +
						(F("Temp_Left: "))	+ RealValueSensors[SENSOR_7][VALUE_1]	+	(F(", ")) +
						(F("Temp_Right: ")) + RealValueSensors[SENSOR_2][VALUE_1]	+	(F(", ")) +
						(F("Ti: "))			+ Ti 									+	(F(", ")) + 
						(F("VCC: "))		+ VCC, 
																GSM_ANSWER_SMS);
	}
	// ===========================================================================================================
	if ((_Line.indexOf(F("temp")) > -1)) {
		if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL){
			Serial.println(F("temp"));
		}
		Send_SMS(String	(F("Temp_UP: "))	+ RealValueSensors[SENSOR_2][VALUE_1] + (F(", ")) +
						(F("Temp_DOWN: "))	+ RealValueSensors[SENSOR_3][VALUE_1] + (F(", ")) , 
																GSM_ANSWER_SMS);
	}
	RingPhone = "";								// Очищаем номер автора смс
	gsm_dtr_on();								// Усыпляем модуль
}


// =========================================================================================================
// =========================================================================================================
// =========================================================================================================
void serial3_ISR(){
	wdt_reset();
	bool View_Input_Line = true;
	String _val = "";
	if (Serial3.available()) {
		while (Serial3.available()) {
			_val = Serial3.readString();
		}
		// =========================================================================================================
		if (_val.indexOf(F("+CMTI")) > -1) {			// Пришло СМС сообщение
 			_val.remove(0,12);
			
			String _Line = F("AT+CMGR=");
			_Line += _val;
			_Line += F(",0");
			
//			Serial.print("_Line: "); Serial.println(_Line);

			//sendATCommand(F("AT+CMGR=") + _val + F(",1") , GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL);
			
			//Serial.print(sendATCommand(_Line, GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL));
			//sendATCommand(_Line, GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);

			if (Check_Phone(_val)) {						// Если СМС от разрешенного абонента
				Serial3.println(F("СМС_1"));
				Master_SMS(_val);							// Запускаем проверку его содержимого
			}
			Clear_SMS(DEL_ALL);								// Очищаем очередь входящих СМС
		}
		// =========================================================================================================
		if (_val.indexOf(F("+CLIP")) > -1) {				// Пришел звонок
			Serial3.println(F("ATH0"));
// 			if (CheckPhone(_val)) {							// Если звонок от разрешенного абонента
// 				Serial3.println(F("ATH0"));
// 			}
// 			else {
// 				Serial3.println(F("ATH0"));
// 			}
		}
		// =========================================================================================================
		if (_val.indexOf(F("+HTTPACTION:")) > -1) {			// Пришел ответ от Web сервера на отправку GET
			View_Input_Line = false;
			Answer_check_GET(_val, GSM_NO_OUTPUT_TO_SERIAL);
		}
		// =========================================================================================================
		if (_val.indexOf(F("+CUSD:")) > -1) {				// Пришло уведомление о USSD-ответе
			View_Input_Line = false;
			if (_val.indexOf(F("\"")) > -1) {				// Если ответ содержит кавычки, значит есть сообщение (предохранитель от "пустых" USSD-ответов)
				StateGSM.Balance = Card_Balance_Check(_val);
			}
		}
		// =========================================================================================================
		if(View_Input_Line){
			if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL){
				_val.trim();
				Serial.println();
				Serial.print(F("GSM input: ")); Serial.println(_val);
				Serial.println();
			}
		}
	}
}
