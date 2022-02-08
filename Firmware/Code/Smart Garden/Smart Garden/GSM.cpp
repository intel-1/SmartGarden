#include <Arduino.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include "GSM.h"
#include "main.h"
#include "Sensors.h"
#include "DataToSerial.h"
#include "Warning.h"
#include "GPRS.h"



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
void Power_GSM(byte State){					
	switch(State){
		case OFF:							// Выключение питания модуля
			gsm_vcc_off();
			break;
		case ON:							// Включение питания
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("Inclusion VCC...OK"));
			}
			gsm_vcc_on();					// Включение DC-DC
			gsm_pwr_key();					// Включение самого модуля
			delay(3000);					// Ожидание запуска. По datasheet 3сек нужно для получения ответов по Serial		
			break;
	}
}


// ========================================================================================================
// ================================ Проверка готовности SIM-карты (AT+CPIN?) ==============================
// ========================================================================================================
bool SIM_card_readiness_check(){
	bool State = false;
	String Return = sendATCommand(F("AT+CPIN?"), true, false);		// Запрос статуса SIM карты
	
	if(Return.lastIndexOf(F("+CPIN")) != -1){						// Если вернулся правильный ответ, а не какой нибудь мусор
		if(Return.lastIndexOf(F("READY")) != -1){					// Если SIM карта готова
			State = true;										
		}
	}	
	return State;
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
byte SignalLevel(bool LogView){
	int Level = 255;
	String Return = sendATCommand(F("AT+CSQ"), true, false);		// Запрос уровня, обязательно дожидаемся ответа
	
	if(Return.lastIndexOf(F("+CSQ")) != -1){						// Если получили правильный ответ
		Return = Return.substring(7,9);								// Убираем лишние символы
		Level = Return.toInt();										// Возвращает уровень сигнала
	}
	
	if(LogView){
		if(OUTPUT_LEVEL_UART_GSM){
			Serial.print(F("Уровень сигнала: "));
			
			if(Level == 0){
				Serial.println(F("115 дБ и меньше"));
			}
			else if(Level == 1){
				Serial.println(F("112 дБ"));
			}
			else if(2 <= Level <= 30){
				Serial.println(F("110..-54 дБ"));
			}
			else if(Level == 31){
				Serial.println(F("52 дБ и сильнее"));
			}
			else if(Level == 99){
				Serial.println(F("нет сигнала"));
			}
			else if(Level == 255){
				Serial.println(F("Ошибка данных"));
			}
		}
	}
	
	return Level;
}


// ========================================================================================================
// ===================== Проверка готовности модуля. Отправка и проверка команды "AT" =====================
// ========================================================================================================
byte Check_Readiness_Module(boolean LogView){					 
	byte ErrorGSM;
	
	if(LogView){
		if(OUTPUT_LEVEL_UART_GSM){
			Serial.print(F("Состояние GSM модуля: "));
		}
	}
	
	String Response[] = {	(F("NOT INSERTED")),				// Готов
							(F("OK"))};							// Зарегистрирован в сети
	
	String Error = sendATCommand(F("AT"), true, false);						// Отправляем команду, ждем ответ, но в Serial его не выводим
	
	for(byte Pos = 0; Pos < sizeof(Response) / sizeof(Response[0]); Pos ++){	// Ищем в списке возможных ответов
		if(Error.lastIndexOf(Response[Pos]) != -1){							// И если нашли совпадение
			switch(Pos){
				case 0:														// Нет SIM карты
					if(LogView){
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.print(F("Нет SIM карты"));
						}
					}
					ErrorGSM = 0;
					Pos = 2;
					break;
				case 1:										// Готов
					if(LogView){
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.println(F("Готов"));
						}
					}
					ErrorGSM = 1;
					Pos = 2;
					break;
				default:									// Неизвестно
					if(LogView){
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.println(F("Неизвестно"));
						}
					}
					ErrorGSM = 50;
					Pos = 2;
					StateGSM.GSM_Registered = false;
			}
		}
	}
	return ErrorGSM;
}


// ========================================================================================================
// =================== Проверка зарегистрировался или нет модуль в сети GSM (AT+CREG?) ====================
// ========================================================================================================
byte CheckRegistrationGSM(bool LogView){				
	byte ErrorGSM;
	
	if(LogView){
		if(OUTPUT_LEVEL_UART_GSM){
			Serial.print(F("Состояние GSM: "));
		}
	}
	
	String Response[] = {	(F("+CREG: 0,0")),			// Не зарегистрирован в сети
							(F("+CREG: 0,1")),			// Зарегистрирован в сети
							(F("+CREG: 0,2")),			// Поиск сети
							(F("+CREG: 0,3")),			// Регистрация отклонена
							(F("+CREG: 0,4"))};			// Неизвестно
	
	String Error = sendATCommand(F("AT+CREG?"), true, false);				// Отправляем команду, ждем ответ, но в Serial его не выводим
	
	for(byte Pos = 0; Pos < sizeof(Error) / sizeof(Error[0]); Pos ++){		// Ищем в списке возможных ответов
		if(Error.lastIndexOf(Response[Pos]) != -1){							// И если нашли совпадение
			if(OUTPUT_LEVEL_UART_GSM){
				switch(Pos){
					case 0:
						if(LogView){
							Serial.print(F("Не зарегистрирован в сети: "));
							Serial.println(F("Нет поиска сети"));
						}
						StateGSM.GSM_Registered = false;
						break;
					case 1:
						if(LogView){
							Serial.println(F("Зарегистрирован в сети"));
						}
						StateGSM.GSM_Registered = true;
						break;
					case 2:
						if(LogView){
							Serial.println(F("Поиск сети"));
						}
						StateGSM.GSM_Registered = false;
						break;
					case 3:
						if(LogView){
							Serial.println(F("Регистрация отклонена"));
						}
						StateGSM.GSM_Registered = false;
						break;
					case 4:
						if(LogView){
							Serial.println(F("Неизвестно"));
						}
						StateGSM.GSM_Registered = false;
						break;
					default:
						if(LogView){
							Serial.println();
						}
						StateGSM.GSM_Registered = false;
				}
			}
			ErrorGSM = Pos;
			return Pos;
		}
	}
	gsm_dtr_on();								// Усыпляем модуль
}


// =========================================================================================================
// ======================= Функция ожидания ответа и возврата полученного результата =======================
// =========================================================================================================
String waitResponse(bool logView) {							
	String _resp = "";															// Переменная для хранения результата
	long _timeout = millis() + EEPROM.read(E_MaximumTimeResponseGSM) * 1000;	// Переменная для отслеживания таймаута (10 секунд)
	while (!Serial3.available() && millis() < _timeout) {wdt_reset();};			// Ждем ответа 10 секунд и обнуляем собаку чтобы не перезагрузить контроллер, 
																				// если пришел ответ или наступил таймаут, то...
	if (Serial3.available()) {													// Если есть, что считывать
		_resp = Serial3.readString();											// считываем и запоминаем
	}
	else {																		// Если пришел таймаут, то
		if(logView){
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
String sendATCommand(String cmd, bool waiting, bool logView) {
	wdt_reset();
	String _resp = "";											// Переменная для хранения результата
	if(OUTPUT_LEVEL_UART_GSM){
		if(logView){
			Serial.println(cmd);								// Дублируем команду в монитор порта
		}
	}
	Serial3.println(cmd);										// Отправляем команду модулю
	if (waiting) {												// Если необходимо дождаться ответа...
		_resp = waitResponse(logView);							// ... ждем, когда будет передан ответ
		//Если Echo Mode выключен (ATE0), то эти 3 строки можно закомментировать	
		if (_resp.startsWith(cmd)) {							// Убираем из ответа дублирующуюся команду
			_resp = _resp.substring(_resp.indexOf("\r", cmd.length()) + 2);
		}
	}
	if(OUTPUT_LEVEL_UART_GSM){
		if(logView){
			Serial.println(_resp);								// Дублируем ответ в монитор порта
		}
	}
	return _resp;												// Возвращаем результат. Пусто, если проблема
}


// =========================================================================================================
// =========================================================================================================
// =========================================================================================================
void InitializingGSM(){
	wdt_reset();	
	String GSMATs[] = {								// Массив АТ команд инициализации GSM
						//(F("ATE0V0+CMEE=1;&W")),	// Для рабочего режима (выключены Эхо, уменьшен уровель логирования)
						(F("ATE0")),				// Выключаем эхо
						(F("ATE1V1+CMEE=2;&W")),	// Средний уровень логирования (для тестового режима работы)					
						(F("AT+CLIP=1")),			// Включаем АОН
						(F("AT+CMGF=1")),			// Формат СМС = ASCII текст
						(F("AT+CSCS=\"GSM\"")),		// Режим кодировки текста = GSM (только англ.)
					    //(F("AT+CNMI=2,2")),
	};
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.println(F("================= Start initializing GSM module  ================="));
	}

	for (byte i = 0; i < sizeof(GSMATs) / sizeof(GSMATs[0]); i++) {
		wdt_reset();
		sendATCommand(GSMATs[i], true, true);		// Отправляем АТ команду, ждем ответ и выводим ответ в Serial
	}
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.println(F("We resolve sleep mode...OK"));	
	}
	Serial3.println(F("AT+CSCLK=1"));				// Разрешаем спящий режим
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.println(F("================= Initializing complete ================="));
	}
	gsm_dtr_on();									// Усыпляем модуль
}


// =========================================================================================================
// =========================================================================================================
// =========================================================================================================
void SendSMS(String Text, byte Level) {	
	/*
		Text - текст сообщения
		PhoneNumber - номер телефона получателя
		Level - уровень важности сообщения. От этого зависит кому отправлять СМС
	*/
	if(CheckRegistrationGSM(false)){		// Проверяем регистрацию в сети и если зареган
		String Phone;
		switch(Level){
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
		Serial3.print(Text);
		delay(50);
		Serial3.print((char)26);
		delay(500);	
		gsm_dtr_on();								// Усыпляем модуль
	}
}


// =========================================================================================================
// =========================================================================================================
// =========================================================================================================
void clearsms() {
	Serial3.println(F("AT+CMGD=4"));
}
	

// =========================================================================================================
// =========================================================================================================
// =========================================================================================================
boolean CheckPhone(String Line) {				// Выборка всех телефонов из списка
	for (byte i = 0; i < CountPhone; i++) {
		if (Line.indexOf(AllowPhone[i]) > -1) {
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
// =========================================================================================================
// =========================================================================================================
void MasterSMS(String Line) {
	wdt_reset();
	#define DelayTime 500
	Line.toLowerCase();								// Преобразовываем строку в нижний регистр
	// ======================================== Перезагрузка контроллера =========================================
	if ((Line.indexOf(F("reset")) > -1)) {				
		EEPROM.update(E_RebutingFromGSM, 1);		// Поднимаем флаг что контроллер отправлен в перезагрузку
		resetFunc();								// reset
	}
	// ======================================== Перезагрузка GPRS ================================================
	if ((Line.indexOf(F("gprs_rst")) > -1)) {	
		EEPROM.write(E_AllowGPRS, 1);				// Разрешаем работу GPRS		
		Serial3.println(F("AT+SAPBR=0,1"));			// Разрываем GPRS соединение
		delay(50);
		InitializingGPRS();
	}
	// ======================================= Включение GPRS ====================================================
	if ((Line.indexOf(F("gprs_on")) > -1)) {			
		EEPROM.write(E_AllowGPRS, 1);				// Разрешаем работу GPRS
		delay(50);
		InitializingGPRS();							// Повторно инициализируем GPRS
	}
	// ====================================== Выключение GPRS ====================================================
	if ((Line.indexOf(F("gprs_off")) > -1)) {			
		EEPROM.write(E_AllowGPRS, 0);				// Запрещаем работу GPRS
		Serial3.println(F("AT+SAPBR=0,1"));			// Разрываем GPRS соединение
	}
	// ===========================================================================================================
	if ((Line.indexOf(F("status")) > -1)) {
		if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL){
			Serial.println(F("Status"));
		}
		SendSMS(String	(F("Temp_Box: ")) + RealValueSensors[SENSOR_0][VALUE_1] + (F(", ")) +
						(F("Temp_UP: ")) + RealValueSensors[SENSOR_8][VALUE_1] + (F(", ")) +
						(F("Temp_DOWN: ")) + RealValueSensors[SENSOR_5][VALUE_1] + (", ") +
						(F("Temp_Left: ")) + RealValueSensors[SENSOR_7][VALUE_1] + (", ") +
						(F("Temp_Right: ")) + RealValueSensors[SENSOR_2][VALUE_1] + (", ") +
						(F("VCC: ")) + VCC, 4);
	}
	// ===========================================================================================================
	if ((Line.indexOf(F("temp")) > -1)) {
		if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL){
			Serial.println(F("temp"));
		}
		SendSMS(String	(F("Temp_UP: ")) + RealValueSensors[SENSOR_2][VALUE_1] + (F(", ")) +
						(F("Temp_DOWN: ")) + RealValueSensors[SENSOR_3][VALUE_1] + (", ") , 1);
	}
	RingPhone = "";								// Очищаем номер автора смс
	clearsms();									// Очищаем очередь входящих СМС
	gsm_dtr_on();								// Усыпляем модуль
}


// =========================================================================================================
// =========================================================================================================
// =========================================================================================================
void serial3ISR(){
	//#define DelayTime 50
	wdt_reset();
	String val = "";
	if (Serial3.available()) {
		while (Serial3.available()) {
			val += char(Serial3.read());
			delay(20);
		}
		if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL){
			Serial.print(F("GSM input: ")); Serial.println(val);
		}
		//delay(DelayTime);
		// =========================================================================================================
		if (val.indexOf(F("+CMTI")) > -1) {			// Пришло СМС сообщение
			//delay(50);
			if (CheckPhone(val)) {					// Если СМС от разрешенного абонента
				Serial3.println(F("СМС_1"));
				MasterSMS(val);						// Запускаем проверку его содержимого
			}
		}
		// =========================================================================================================
		if (val.indexOf(F("+CLIP")) > -1) {			// Пришел звонок
			if (CheckPhone(val)) {					// Если звонок от разрешенного абонента
				Serial3.println(F("ATH0"));
			}
			else {
				Serial3.println(F("ATH0"));
			}
		}
		// =========================================================================================================
		if (val.indexOf(F("+CUSD:")) > -1) {		// Пришло уведомление о USSD-ответе
			//delay(DelayTime);
			if (val.indexOf(F("\"")) > -1) {		// Если ответ содержит кавычки, значит есть сообщение (предохранитель от "пустых" USSD-ответов)
				delay(1000);
			}
			if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL){
				Serial3.println(F("AT+CUSD=0"));
			}
			//delay(DelayTime);
		}
	}
}


































/*
// =========================================================================================================
// =========================================================================================================
// =========================================================================================================
void ReadFromQueueGSM(byte Paramm){
	char buff[30];
	String Text;
	for(byte Place = 0; Place < ExtentOfTurn; Place ++){	// Проходим по всем ячейчам очереди сообщений
		if(TurnOfMessagesGSM[Place][0] == 1){				// Если находим занятую
			for(byte i = 1; i <= QuantityParametersInMessage - 3; i += 2){
				switch(TurnOfMessagesGSM[Place][i]){
					case 0:									// Если след. ячейка пустая
						break;
					case 1:									// Текст
						strcpy_P(buff, (char*)pgm_read_word(&(SMS_Text[TurnOfMessagesGSM[Place][i + 1]])));
						Text += String(buff);
						break;
					case 2:									// Номер датчика
						Text += TurnOfMessagesGSM[Place][i + 1];
						break;
				}
			}
			if(Paramm == 0){				// Вывод очереди сообщений в консоль
				Serial.println(Text);
				Text = "";
			}
			else{							// Выполняем отправку сообщений
				for(byte i = 0; i <= QuantityParametersInMessage; i ++){
					TurnOfMessagesGSM[Place][i] = 0;
				}
			}
		}
	}
}*/