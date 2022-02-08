#ifndef GSM_H
#define GSM_H

#include <stdint.h>
#include <Arduino.h>
#include "Configuration.h"



extern struct StructGPRSConnection{
	boolean GSM_Registered;
	String IP_GPRS;
	byte GPRS_Connect;
	byte Code_Connect_GPRS;					// Номер ошибки подключения к GPRS
	bool ErrorSentGET;						// Ошибка отправки GET запроса
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
	
	
const char AllowPhone_0[] PROGMEM = "79139045925";
const char AllowPhone_1[] PROGMEM = "79137517075";
const char AllowPhone_2[] PROGMEM = "79138963482";
const char AllowPhone_3[] PROGMEM = "79133774145";

static const char* AllowPhone[] = {	AllowPhone_0, // Номера которым разрешено управлять контролером
									AllowPhone_1, 
									AllowPhone_2, 
									AllowPhone_3}; 
									
static const char* NumberForWarningSMS[] = {"79139045925"};
static const char* NumberForCriticSMS[] = {"79139045925"};
static const char* AlarmPhone[] = {"79139045925"};				// Номера для отправки смс
	
static int8_t CountPhone = sizeof(AllowPhone) / sizeof(AllowPhone[0]);
static int8_t CountAlarmPhone = sizeof(AlarmPhone) / sizeof(AlarmPhone[0]);
extern uint8_t BalanceStringLen;		//Число символов от начала строки которые нужно переслать в смс сообщении при получении USSD ответа о балансе
extern String RingPhone;
extern boolean _Power_GSM;			// Флаг что GSM модуль включен
//extern boolean GSMmoduleRegistered;		// Флаг что модуль зарегистрирован в сети
//extern boolean GPRSinitializing;		// Флаг что GPRS проинициализирован
//extern byte ErrorGSM;					// Ошибка регистрации GSM

//extern byte QuantityInitializingGPRS;	// Количество потыток не удачных ошибок инициализации GPRS
extern boolean ReadinessGSMmodule;		// Проверка готовности GSM модуля


extern String Link_LogWebServer;
extern String Link_LogDataWebServer;

struct MessageQueueGSM{					// Структура для добавления СМС сообщения в очередь отправки
	byte Arg_1_a;
	byte Arg_1_b;
	byte Arg_2_a;
	byte Arg_2_b;
	byte Arg_3_a;
	byte Arg_3_b;
	byte Arg_4_a;
	byte Arg_4_b;
	byte PhoneNumber;
};
static struct MessageQueueGSM MessageGSM;

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
												
static void(* resetFunc) (void) = 0;	// Reset MC function

String ReadGSM();						//функция чтения данных от GSM модуля

void WriteToQueueGSM(struct MessageQueueGSM Par);

void ReadFromQueueGSM(byte Paramm);

void InitializingGSM();					// Инициализация GSM модуля

void SendSMS(String Text, byte Level);	// Отправка СМС

void balance();

void serial3ISR(); 

String sendATCommand(String cmd, bool waiting, bool logView);	// Параметры: Текст сообщения, ждать или нет ответа от модуля, выводить ли данные в консоль

void Power_GSM(byte State);							// Включение\выключение GSM модуля

String waitResponse(bool logView);					// Ожидание ответа от GSM модуля 
byte CheckRegistrationGSM(bool LogView);			// Проверка регистрации в сети			
byte Check_Readiness_Module(bool LogView);			// Проверка готовности GSM модуля
byte SignalLevel(bool LogView);						// Уровень сигнала GSM сети
bool SIM_card_readiness_check();					// Проверка готовности SIM-карты


#endif
