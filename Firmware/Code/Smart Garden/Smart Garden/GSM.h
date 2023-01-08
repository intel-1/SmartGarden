#ifndef GSM_H
#define GSM_H

#include <stdint.h>
#include <Arduino.h>
#include "Configuration.h"



#define GSM_OUTPUT_TO_SERIAL true
#define GSM_NO_OUTPUT_TO_SERIAL false
#define GSM_WAITING_ANSWER true			// Ждать ответ от модуля
#define GSM_NO_WAITING_ANSWER false		// Не ждать ответа от модуля
#define GSM_PERFORM_MEASUREMENT true	// Выполнить измерение (сделано для измерения уровня сигнала GSM)
#define GSM_DO_NOT_MEASURE false		// Не выполнять измерения

// Уровни исходящих СМС
#define GSM_SMS_INFO 0					// info			- Обычные сообщения
#define GSM_SMS_ANSWER 1				// answer		- Ответ на входящее СМС
#define GSM_SMS_DEBUG 2					// debug		- Подробная информация для отладки
#define GSM_SMS_WARNING 3				// warning		- Исключительные случаи, но не ошибки
#define GSM_SMS_ERROR 4					// error		- Ошибки исполнения, не требующие сиюминутного вмешательства
#define GSM_SMS_CRITICAL 5				// critical		- Критические состояния
#define GSM_SMS_EMERGENCY 6				// emergency	- Система не работает

// Параметры очистки очереди СМС
#define DEL_READ 1						// Удалить все прочитанные SMS
#define DEL_UNREAD 2					// Удалить все непрочитанные SMS
#define DEL_SENT 3						// Удалить все отправленные SMS
#define DEL_UNSENT 4					// Удалить все неотправленные SMS
#define DEL_INBOX 5						// Удалить все полученные SMS
#define DEL_ALL 6						// Удалить все SMS

// Проверка зарегистрировался или нет модуль в сети GSM (AT+CREG?) (Check_Registration_GSM)
#define GSM_NO_FIND_NETWORK 0			// Нет поиска сети
#define GSM_REGISTERED 1				// Зарегистрирован
#define GSM_FINDING_NETWORK 2			// Поиск сети
#define GSM_REGISTRED_REJECTED 3		// Регистрация отменена
#define GSM_REGISTERED_UNKNOWN 4		// Неизвестно

// Проверка готовности модуля. Отправка и проверка команды "AT"(Check_Readiness_GSM_Module)
#define GSM_MODULE_NOT_SIM_CARD 0	// Нет SIM карты
#define GSM_MODULE_READY 1			// Готов
#define GSM_MODULE_UNKNOWN 50		// Неизвестно

// Проверка готовности SIM-карты (AT+CPIN?)(SIM_card_readiness_check())
#define GSM_SIM_READY 0			// MT is not pending for any password
#define GSM_SIM_PIN 1			// MT is waiting SIM PIN to be given
#define GSM_SIM_PUK 2			// MT is waiting for SIM PUK to be given
#define GSM_PH_SIM_PIN 3		// ME is waiting for phone to SIM card (antitheft)
#define GSM_PH_SIM_PUK 4		// ME is waiting for SIM PUK (antitheft)
#define GSM_SIM_PIN2 5			// PIN2, e.g. for editing the FDN book possible only if preceding Command was acknowledged with +CME ERROR: 17
#define GSM_SIM_PUK2 6			// Possible only if preceding Command was acknowledged with error +CME ERROR: 18

// Тип GET запроса
#define GET_LOG_REQUEST 1		// Отправка логирования
#define GET_VALUE_REQUEST 2		// Отправка значений датчиков

// Проверка регистрации GPRS
#define	GPRS_CONNECTION_IS_ESTABLISHED 0	// Соединение устанавливается
#define	GPRS_CONNECTION_ESTABLISHED	1		// Соединение установлено
#define	GPRS_CONNECTION_CLOSED 2			// Соединение закрывается
#define GPRS_NO_CONNECTED 3					// Нет соединения
#define GPRS_UNKNOWN 4						// Неизвестно




extern struct StructGPRSConnection{
	boolean GSM_Registered;
	float Balance;
	String IP_GPRS;
	byte GPRS_Connect;
	byte GSM_Signal_Level;				// Уровень сигнала GSM сети
	byte Code_Connect_GPRS;				/* Номер ошибки подключения к GPRS:		
																				0 - Соединение устанавливается
																				1 - Соединение установлено
																				2 - Соединение закрывается
																				3 - Нет соединения */
	byte Code_Connect_GSM;				/* Номер ошибки подключения к GSM:		
																				0 - Не зарегистрирован в сети
																				1 - Зарегистрирован в сети
																				3 - Поиск сети
																				4 - Регистрация отклонена
																				5 - Неизвестно */
	byte State_GSM_Module;				/* Состояние GSM модуля:				
																				0	-	Нет sim карты
																				1	-	Готов
																				50	-	Неизвестно */
	int Code_Error_Sent_GET;			/* Номер ответа отправки GET запроса:
																				404 - Страница не найдена
																				200 - GET запрос успешно получен!!!
																				603 - Сервер не доступен */
	bool Error_Sent_GET;				/* Состояние отправки GET запроса:		
																				true - запрос не отправлен */
} StateGSM;

const char SMS_Text_0[] PROGMEM = "Read value sensor: ";
const char SMS_Text_1[] PROGMEM = "Error read sensor ";
const char SMS_Text_2[] PROGMEM = "Sensor ";
const char SMS_Text_3[] PROGMEM = " is off";
const char SMS_Text_4[] PROGMEM = " is error";
const char SMS_Text_5[] PROGMEM = " Xa-xa-xa";

static const char* const SMS_Text[] PROGMEM = {	SMS_Text_0, 
												SMS_Text_1, 
												SMS_Text_2, 
												SMS_Text_3, 
												SMS_Text_4, 
												SMS_Text_5};
	
									
static const char* NumberForWarningSMS[]	= {"79139045925"};
static const char* NumberForCriticSMS[]		= {"79139045925"};
static const char* AlarmPhone[]				= {"79139045925"};	// Номера для отправки смс

extern String AllowPhone[];										// Список разрешенных мобильных телефонов (с них разрешено управление контроллером)
	
static int8_t CountAlarmPhone = sizeof(AlarmPhone) / sizeof(AlarmPhone[0]);
extern String RingPhone;
extern boolean _Power_GSM;										// Флаг что GSM модуль включен

extern boolean ReadinessGSMmodule;								// Проверка готовности GSM модуля

extern String Link_LogWebServer;
extern String Link_LogDataWebServer;

extern String GPRS_APN_NAME;
extern String GPRS_APN_USER;
extern String GPRS_APN_PASSWORD;

extern String GSM_CODE_BALANCE;

// Символы табулящии для логов GSM
extern String GSM_GET_Tab_1;
extern String GSM_GET_Tab_2;
//================================

static struct MessageQueueGSM{					// Структура для добавления СМС сообщения в очередь отправки
								byte Arg_1_a;
								byte Arg_1_b;
								byte Arg_2_a;
								byte Arg_2_b;
								byte Arg_3_a;
								byte Arg_3_b;
								byte Arg_4_a;
								byte Arg_4_b;
								byte PhoneNumber;
} MessageGSM;

#define ExtentOfTurn 10					// Размер очереди сообщений
#define QuantityParametersInMessage 10	// Максимальное кол-во параметров в сообщении

extern byte TurnOfMessagesGSM[ExtentOfTurn][QuantityParametersInMessage];	/* Очередь СМС сообщений (максимально 10 исходящих сообщений):
										|-------|---|-------------------|---|-------------------|---|-------------------|---|-------------------|-------------------|
										| Статус| Б	| Часть сообщения 1	| Б	| Часть сообщения 2	| Б	| Часть сообщения 3	| Б	| Часть сообщения 4	|  Номер получателя	|
										|-------|---|-------------------|---|-------------------|---|-------------------|---|-------------------|-------------------|
										|	1	|	|					|	|					|	|					|	|					|					|
										|  ...	|...| .................	|...| .................	|...| .................	|...| .................	| .................	|
										|   10	|	|					|	|					|	|					|	|					|					|
										|-------|---|-------------------|---|-------------------|---|-------------------|---|-------------------|-------------------|  */
												
static void(* resetFunc) (void) = 0;			// Reset MC function

String Read_GSM();								// Функция чтения данных от GSM модуля
void Write_To_Queue_GSM(struct MessageQueueGSM Par);
void Read_From_Queue_GSM(byte _Paramm);
void Initializing_GSM(bool _out_to_lcd);		// Инициализация GSM модуля
void Send_SMS(String _Text, byte _Level);		// Отправка СМС
float Card_Balance_Check(String Text);
void serial3_ISR(); 
String send_AT_Command(String _Command, bool _waiting, bool _logView);	// Параметры: Текст сообщения, ждать или нет ответа от модуля, выводить ли данные в консоль
void Power_GSM(byte _State);					/* Включение\выключение GSM модуля:		ON	- Включение
																						OFF - Выключение
																						RESET - Перезагрузка */
void Initializing_GPRS();																// Настройка подключения к GPRS
void Connecting_GPRS(bool _out_to_lcd);													// Подключение к GPRS
bool Check_Connection_GPRS(bool _LogView);												// Проверка регистрации GPRS
void Send_GET_request(String Text, bool _waiting, bool _LogView, byte _Request_Type);	// Отправка GET пакетов в БД по GPRS

void Answer_check_GET(String _Text, bool _LogView);										// Проверка ответа от Web сервера на GET запрос

String wait_Response(bool _logView);													// Ожидание ответа от GSM модуля 
byte Check_Registration_GSM(bool _LogView, bool _out_to_lcd);	/* Проверка регистрации в сети:		Ф-ция возвращает только true или false (зарегистрирован\не зарегистрирован). 
																						Остальные состояния можно посмотреть в переменной StateGSM.Code_Connect_GSM:
																								0 - Не зарегистрирован в сети
																								1 - Зарегистрирован в сети
																								2 - Поиск сети
																								3 - Регистрация отклонена
																								4 - Неизвестно */
byte Check_Readiness_GSM_Module(bool _LogView, bool _out_to_lcd);	// Проверка готовности GSM модуля
void Signal_Level(bool _LogView, bool _Perform_Measurement);		// Уровень сигнала GSM сети (Первый параметр нужно или нет выводит данные в UART, второй измерять или нет)
byte SIM_card_readiness_check(byte _Logview);						// Проверка готовности SIM-карты


#endif
