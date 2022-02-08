
#include <Arduino.h>
#include <EEPROM.h>

#include "ParsingDataOnSerial.h"
#include "EEPROM_ADR.h"
#include "Sensors.h"
#include "main.h"
#include "DigitalPorts.h"
#include "ExecModules.h"
#include "RecievedUART\RecievedUART.h"


int InputFromSerial0[25];					// Массив с полученными по UART0 данными

boolean recievedFlag_date;
boolean transmitFlag_date;
boolean recievedFlag_config_controller;
boolean recievedFlag_config_module;
boolean recievedFlag_debug;
boolean recievedFlag_channel;
boolean recievedFlag_sensors;
boolean recievedFlag_WindowsAPP;
boolean recievedFlag_commandModule;
boolean recievedFlag_ModeConfigurationController;
boolean getStarted;
char incomingByte;
char oldByte;
byte index;
String string_convert = "";
byte typeData = 0;
boolean cmdERR;
boolean RecievedDataFromUART;				// Флаг что получены данные из UART
boolean ConnectUART;						// Флаг что осуществленно подключение по UART. Управляющее приложение раз в 10 секунд поднимает этот флаг.

byte NumberUARTPort;						// Номер Serial порта чтобы отличать откуда пришла комманда

typedef void (*do_reboot_t)(void);
const do_reboot_t do_reboot = (do_reboot_t)((FLASHEND-1023)>>1);


void jmp_bootloader() {
	cli();
	MCUSR = 0;
	__asm__ __volatile__ ("jmp 0x3fc00" ::);	// Start bootloader
} 

void CleanInputFromSerial0(){
	for (byte i=0; i<sizeof(InputFromSerial0)/sizeof(int); i++){			// Затираем массив после работы
		InputFromSerial0[i] = 0;
	}
}

boolean TestCMD() {
	switch(incomingByte) {
		case '\r' :
			if (oldByte!='\n') {
				cmdERR = true;
			}
			break;
		
		case '\n' :
			if (oldByte!='\r') {
				cmdERR = true;
			}
			break;
		
		case ' ':
		if 	(oldByte=='0') jmp_bootloader();	// поймали '0 ' переход на бутлоадер
		
		if (string_convert=="") {				// команда только в начале строки
			switch(oldByte){
				case 't':						// Настройка даты и времени
					typeData = 1;
					return true;
				case 'c':						// Настройка контроллера
					typeData = 3;
					return true;
				case 'm':						// Настройка модулев управления
					typeData = 4;
					return true;
				case 'd':						// Изменение режима вывода данных в Serial
					typeData = 6;
					return true;
				case 's':						// Настройка датчиков
					typeData = 7;
					return true;
				case 'g':						// Настройка групп управления
					typeData = 8;
					return true;
				case 'w':						// Служебный байт для Windows приложений
					typeData = 9;
					return true;
				case 'k':						// Команды управления модулями
					typeData = 10;
					return true;
				case 'p':						// Включение\Выключение режима конфигурирования
					typeData = 11;
					return true;
			}
		}
	}
	return false;
}


boolean SetFlags() {
	switch (typeData){
		case 1:
			recievedFlag_date = true;
			break;
		case 2:											// Свободная ячейка
			break;
		case 3:
			recievedFlag_config_controller = true;
			break;
		case 4:
			recievedFlag_config_module = true;
			break;
		case 6:
			recievedFlag_debug = true;
			break;
		case 7:
			recievedFlag_sensors = true;
			break;
		case 8:
			recievedFlag_channel = true;
			break;
		case 9:
			recievedFlag_WindowsAPP = true;
			break;
		case 10:
			recievedFlag_commandModule = true;
			break;
		case 11:
			recievedFlag_ModeConfigurationController = true;
			break;
		default:
			return false;
	}
	return true;
}

/*
void serial_WiFi_ISR() {				// Комманды с Wi-Fi модуля
	while (Serial2.available() > 0) {
		oldByte = incomingByte;
		//NumberUARTPort = 2;
		incomingByte = Serial2.read();	// читаем первый символ
		
		if(!getStarted){				// Проверяем первый символ
			index = 0;
			getStarted = TestCMD();
			if (getStarted) {
				string_convert = "";
			}
		}
		else {										// если парсинг разрешён
			if ((incomingByte != ' ') && (incomingByte != '\n')&& (incomingByte != '\r')) {  // если это не пробел и не конец
				string_convert += incomingByte;					// складываем в строку
			}
			else {									// если это пробел или "/n" конец пакета
				InputFromSerial0[index] = string_convert.toInt();
				string_convert = "";				// очищаем строку
				index++;							// переходим к парсингу следующего элемента массива
				if ((incomingByte == '\n')||(incomingByte == '\r')) {		// если данные приняты: '/n' - кнопка "Enter"
					getStarted = false;				// сброс парсинга
					if (!SetFlags()) {
						cmdERR = true;
					}
				}
			}
		}
	}
}*/


void serialISR() {
	while (Serial.available() > 0) {
		RecievedDataFromUART = true;	// Пришли данные из UART
		oldByte = incomingByte;
		NumberUARTPort = 0;
		incomingByte = Serial.read();	// Читаем первый символ
		
		if(!getStarted){				// Проверяем первый символ
			index = 0;
			getStarted = TestCMD();
			if (getStarted) {
				string_convert = "";
			}
		}
		else {																					// если парсинг разрешён
			if ((incomingByte != ' ') && (incomingByte != '\n') && (incomingByte != '\r')) {	// если это не пробел и не конец
				string_convert += incomingByte;													// складываем в строку
			}
			else {																				// если это пробел или "/n" конец пакета
				InputFromSerial0[index] = string_convert.toInt();
				string_convert = "";															// очищаем строку
				index++;																		// переходим к парсингу следующего элемента массива
				if ((incomingByte == '\n')||(incomingByte == '\r')) {							// если данные приняты: '/n' - кнопка "Enter"
					getStarted = false;															// сброс парсинга
					if (!SetFlags()) {
						cmdERR = true;
					}
				}
			}
		}
	}
	if(RecievedDataFromUART){				// Если пришли данные из UART
		ProcessingDataFromSerial();			// Разбираем данные полученные из Serial
	}
}


	
void CleaningDuplicatedPorts(int Value){							// Ф-ция для проверки не назначен ли порт кому либо. 
	for(byte Module = 1; Module <= QuantityExecModule; Module ++){	// Проходим по всем модулям		
		if(EEPROM.read(E_LowSwitchPortModule + Module) == Value){	// HIGH концевик у модулей
			EEPROM.write(E_LowSwitchPortModule + Module, 0);
			break;
		}
		if(EEPROM.read(E_HighSwitchPortModule + Module) == Value){	// LOW концевик у модулей
			EEPROM.write(E_HighSwitchPortModule + Module, 0);
			break;
		}
	}
	for(byte Sensor = 1; Sensor <= QuantitySensors; Sensor++){			// Проходим по всем датчикам
		if(EEPROM.read(E_SensorControllPort + Sensor) == Value){		// Порт управления у датчиков
			EEPROM.write(E_SensorControllPort + Sensor, 0);
			break;
		}
		if(EEPROM.read(E_Type_A_Sensor + Sensor) == 8){					// Если датчик настроен как аналоговый
			if(EEPROM.read(E_Address_Sensor + (10*Sensor)) == Value){
				EEPROM.write(E_Address_Sensor + (10*Sensor), 0);
				break;
			}
		}
	}
}



void ProcessingDataFromSerial(){
	if (cmdERR) {
		cmdERR = false;
		Serial.println(F("ERR"));
	}
	// ================================= Служебный байт Windows APP ========================================= 
	if(recievedFlag_WindowsAPP){
		Serial.print(F("w "));
		Serial.println(Short_VersionFirmware);
		recievedFlag_WindowsAPP = false;
	}
	// ==================================== Изменение режима debug ==========================================
	if (recievedFlag_debug) {
		RecievedDebug();
	}
	// ================================= Настройка даты и времени ===========================================
	if (recievedFlag_date) {
		RecievedDate();
	}
	// ================================= Конфигурация контроллера ===========================================
	if (recievedFlag_config_controller) {														
		RecievedConfigController();
	}
	// ================================ Настройки исполнительных модулей ====================================
	if (recievedFlag_config_module) {
		RecievedConfigExecModule();
	}
	// ==================================== конфигурация датчиков ===========================================
	if (recievedFlag_sensors) {
		RecievedSensors();
	}
	// ================================ конфигурация групп управления =======================================
	if (recievedFlag_channel) {
		RerecievedChannel();
	}	
	// ============================ Команды управления исполнительными модулями =============================
	if (recievedFlag_commandModule) {
		RerecievedControllCommand();
	}	
	// =========================== Перевод контроллера в режим конфигурирования =============================
	if (recievedFlag_ModeConfigurationController) {
		if(0 <= InputFromSerial0[0] && InputFromSerial0[0] <= 2){						// Разрешенные значения 0, 1 и 2
			switch(InputFromSerial0[0]){		
				case 0:								// Если получаем параметры
					if(0 <= InputFromSerial0[1] && InputFromSerial0[1] <= 1){			// Разрешенные значения 0 и 1
						EEPROM.update(E_ConfigModeController, InputFromSerial0[1]);
						Serial.println();
						if(InputFromSerial0[1] == 1){
							Serial.println(F("Включен режим конфигурирования"));
							jmp_bootloader();
							//Reboot();	
						}
						else{ 
							Serial.println(F("Режим конфигурирования выключен"));
							//Reboot();
							jmp_bootloader();
						}
					}
					break;
				case 1:								// Если возвращаем в виде байт
					Serial.print(F("p ")); Serial.print(F("1 ")); Serial.println(EEPROM.read(E_ConfigModeController));
					break;
				case 2:								// Если возвращаяем с описаниями
					Serial.println();
					if(EEPROM.read(E_ConfigModeController) == 1){
						Serial.println(F("Контроллер в режиме конфигурирования"));
					}
					else Serial.println(F("Режим конфигурирования выключен"));
					break;
			}
		}
		recievedFlag_ModeConfigurationController = false;
		CleanInputFromSerial0();
	}
	RecievedDataFromUART = false;
}



