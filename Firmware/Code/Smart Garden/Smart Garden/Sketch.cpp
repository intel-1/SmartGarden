// Привет добрый друг
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <EEPROM.h>
#include <util/delay.h>
#include "lib\LiquidCrystal_I2C.h"
#include "lib\SD.h"
#include "lib\SPI.h"

#include "LCDdisplay.h"
#include "EEPROM_ADR.h"
#include "GSM.h"
#include "ParsingDataOnSerial.h"
#include "Sensors.h"
#include "Warning.h"
#include "Configuration.h"
#include "main.h"
#include "ExecModules.h"
#include "DigitalPorts.h"
#include "ConfigSensors.h"



unsigned long LoopSecond;
byte RegisterMCUSR; 
boolean OnEncoderKey;
String ReasonWDT = "";						// Причина перезагрузки контроллера или запуска
byte Code_Reason_WDT;						// Код причины перезагрузки контроллера или запуска
boolean SendSMSorStartController = false;	// Флаг отложеной отправки СМС о старте контроллера
											// Поднимается если GSM не успел зарегистрироваться в сети при старте контроллера


void ADC_init(){
	ADMUX = 0b01000000;			// Измеряем на ADC0, опорное напряжение: "AVCC with external capacitor at AREF pin"
	ADCSRA = 0b10001100;		// Включаем АЦП, разрешаем прерывания, делитель = 16
	ADCSRB = 0b00000000;		// Включаем ADC каналы, режим единичной выборки
	bitWrite(ADCSRA, 6, 1);		// Запускаем преобразование (1 в ADSC)
	sei();						// Разрешаем прерывания
}

void Timer5_init(){								// Таймер по переполнению раз в секунду
	TCCR5B = 0;									// Остановка таймера
	TCCR5A = (0<<WGM51)|(0<<WGM50);				// Режим CTC (сброс по совпадению)
	TCNT5 = 49911;								// Начальное значение таймера
	TIFR5 = (1<<TOV5);							// Сброс флагов в регистре TIFR
	TIMSK5 |= (1<<TOIE5);						// Разрешить прерывание по переполнению T3
	TCCR5B |= (1<<CS52)|(0<<CS51)|(1<<CS50);	// Предделитель CLK/1024
}

void Int_encoder_init(){									// Прерывание энкодера
	EIMSK = (1<<INT4)|(1<<INT5);							// Разрешаем прерывания. INT4 (кнопка), INT5 (крутилка)
	EICRB = (0<<ISC40)|(1<<ISC41)|(0<<ISC50)|(1<<ISC51);	// Прерывание при изменении фронта
}

void PCINT_init(){
	PCICR = (1<<PCIE0)|(1<<PCIE1);
	PCMSK1 = (1<<PCINT11)|(1<<PCINT12)|(1<<PCINT13)|(1<<PCINT14)|(1<<PCINT15);
	PCMSK0 = (1<<PCINT4)|(1<<PCINT5)|(1<<PCINT6)|(1<<PCINT7);
}

ISR (TIMER5_OVF_vect){
	TCNT5 = 49911;					// Обнуляем начальное состояние таймера
	T_second ++;					// Увеличиваем счетчик таймера (секунды)
	TimerTsecond ++;
	if(TimerTsecond == 60){
		TimerTsecond = 0;
		T_min ++;
	}
	if(T_min == 60){
		T_hour ++;
		T_min = 0;
	}
	if(T_hour == 24){
		T_day ++;
		T_hour = 0;
	}
	
	if (T_second == 65500){			// Если счетчик таймера достигает максимального значения (примерно 18 часов)
		CleanTimeIntervals();		// Обнуляем все счетчики
	}
}

ISR(INT4_vect){						// Кнопка энкодера
	OnEncoderKey = true;
	Serial.println(F("Enable encoder key"));
}

ISR(INT5_vect){						// Крутилка энкодера
	Serial.println(F("Key INT5"));
}

ISR(PCINT0_vect) {							// Прерывания концевиков
	if (!(PINB & (1 << INPUT_D_PIN_5))) {StatusSwitchPorts[5] = true;}		// INPUT_D_PIN_5 - PCINT4
	else StatusSwitchPorts[3] = false;
	if (!(PINB & (1 << INPUT_D_PIN_1))) {StatusSwitchPorts[1] = true;}		// INPUT_D_PIN_1 - PCINT5
	else StatusSwitchPorts[1] = false;
	if (!(PINB & (1 << INPUT_D_PIN_2))) {StatusSwitchPorts[2] = true;}		// INPUT_D_PIN_2 - PCINT6
	else StatusSwitchPorts[2] = false;
	if (!(PINB & (1 << INPUT_D_PIN_3))) {StatusSwitchPorts[3] = true;}		// INPUT_D_PIN_3 - PCINT7
	else StatusSwitchPorts[3] = false;
}

ISR(PCINT1_vect) {
	if (!(PINJ & (1 << INPUT_D_PIN_4))) {StatusSwitchPorts[4] = true;}		// INPUT_D_PIN_4 - PCINT11
	else StatusSwitchPorts[4] = false;
	if (!(PINJ & (1 << INPUT_D_PIN_6))) {StatusSwitchPorts[6] = true;}		// INPUT_D_PIN_6 - PCINT13
	else StatusSwitchPorts[6] = false;
	if (!(PINJ & (1 << INPUT_D_PIN_7))) {StatusSwitchPorts[7] = true;}		// INPUT_D_PIN_7 - PCINT12
	else StatusSwitchPorts[7] = false;
	if (!(PINJ & (1 << INPUT_D_PIN_8))) {StatusSwitchPorts[8] = true;}		// INPUT_D_PIN_8 - PCINT15
	else StatusSwitchPorts[8] = false;
	if (!(PINJ & (1 << INPUT_D_PIN_9))) {StatusSwitchPorts[9] = true;}		// INPUT_D_PIN_9 - PCINT14
	else StatusSwitchPorts[9] = false;
}

ISR(ADC_vect){
	float ADC;
	static byte WorkChanelADC_7_15;
	static int SummADC = 0;								// Суммирование измерений ADC
	static byte NumberMeasurementsADC = 0;				// Счетчик количества измерений ADC
	int BuffADC_VCC = 0;
	byte AllowSaveADC = false;							// Разрешение сохранять ADC в итоговый массив значений
	NumberMeasurementsADC ++;
	BuffADC_VCC = ADCL;
	BuffADC_VCC += ADCH << 8;
	SummADC += BuffADC_VCC;
	if(NumberMeasurementsADC == 250){	// Измеряем уровень ADC 60 раз для увеличения точности
		ADC = SummADC / 60;
		AllowSaveADC = true;			// Разрешаем сохранять измеренное показание
		NumberMeasurementsADC = 0;		// Обнуляем счетчик если измерили 60 раз
		SummADC = 0;
	}
	if(AllowSaveADC){
		switch(ADMUX){
			case 0b01000000:							// ADC0
				RealValueADC[0] = ADC;					// Сохраняем измеренное показание
				ADMUX = 0b01000001;						// Выставляем следующий канал для измерения (ADC1)
				ADCSRB = 0b00000000;					// --//--//--//--
				break;
			case 0b01000001:							// ADC1
				RealValueADC[1] = ADC;					// Сохраняем измеренное показание
				ADMUX = 0b01000011;						// Выставляем следующий канал для измерения (ADC3)
				ADCSRB = 0b00000000;					// --//--//--//--
				break;
			case 0b01000011:							// ADC3
				RealValueADC[3] = ADC;					// Сохраняем измеренное показание
				ADMUX = 0b01000100;						// Выставляем следующий канал для измерения (ADC4)
				ADCSRB = 0b00000000;					// --//--//--//--
				break;									// Сохраняем измеренное показание
			case 0b01000100:							// ADC4
				RealValueADC[4] = ADC;					// Сохраняем измеренное показание
				ADMUX = 0b01000101;						// Выставляем следующий канал для измерения (ADC5)
				ADCSRB = 0b00000000;					// --//--//--//--
				break;
			case 0b01000101:							// ADC5			(ACS712)
				RealValueADC[5] = ADC;					// Сохраняем измеренное показание
				ADMUX = 0b01000111;						// Выставляем следующий канал для измерения (ADC7)
				ADCSRB = 0b00000000;					// --//--//--//--
				WorkChanelADC_7_15 = 7;
				break;
			case 0b01000111:							// ADC7 или ADC15
				switch(WorkChanelADC_7_15){
					case 7:								// ADC7
						RealValueADC[7] = ADC;			// Сохраняем измеренное показание
						ADMUX = 0b01000111;				// Выставляем следующий канал для измерения (ADC15)
						ADCSRB = 0b00001000;
						WorkChanelADC_7_15 = 15;
						break;
					case 15:									// ADC15 (напряжение питания)
						RealValueADC[15] = ADC;					// Сохраняем измеренное показание
						ADMUX = 0b01000000;						// Выставляем следующий канал для измерения (ADC0)
						ADCSRB = 0b00000000;
						break;
				}
				break;
		}
	}
	bitWrite(ADCSRA, 6, 1);	// Запускаем преобразование (1 в ADSC)
}


void userPreInit() {		// Это вызывается самым первым перед инициализацией ядра ардуино
	RegisterMCUSR = MCUSR;	// Сохраняем причину перезагрузки
	MCUSR=0;
	wdt_disable();			// Запрещаем WDT
}

void WDT_state(bool ViewLog){												// Вывод причины перезагрузки контроллера
	switch(RegisterMCUSR){
		case 4:
			if(ViewLog){
				Serial.println(F("Low voltage"));
			}
			ReasonWDT = F("Low voltage");
			Code_Reason_WDT = 4;
			break;
		case 5:
			if(ViewLog){
				Serial.println(F("On power"));
			}
			ReasonWDT = F("On power");
			Code_Reason_WDT = 5;
			break;
		case 8:
			if(ViewLog){
				Serial.println(F("Operation watchdog"));
			}
			ReasonWDT = F("Operation watchdog");
			Code_Reason_WDT = 8;
			break;
	}
}

void Dimension_VCC(){	
	VCC = ina219_int.getBusVoltage_V() + (ina219_int.getShuntVoltage_mV() / 1000);
}


void Start_Init_GSM(){
	Serial.println(F("========== Start initialization GSM =========="));
	WriteToLCD(String(F("== Init GSM module =")), LCD_LINE_2, LCD_START_SYMBOL_1, LCD_NO_SCREEN_REFRESH_DELAY);
	Power_GSM(ON);																		// Включение питания на модуля
	_delay_ms(3000);																	// Задержка чтобы модуль успел нормально запуститься
	Signal_Level(GSM_OUTPUT_TO_SERIAL, GSM_PERFORM_MEASUREMENT);						// Проверка уровеня сигнала сети и вывод его на экран
	if(Check_Readiness_GSM_Module(ON, LCD_ALLOW_OTPUT_ON_SCREEN) == GSM_MODULE_READY){	// Проверка готовности модуля (вывод лог в Serial) и если готов (вернул единицу)
		if(SIM_card_readiness_check(ON) == GSM_SIM_READY){								// Проверка SIM карты и вывод на LCD экран
			Initializing_GSM(LCD_ALLOW_OTPUT_ON_SCREEN);								// то инициализация GSM
			Connecting_GPRS(LCD_ALLOW_OTPUT_ON_SCREEN);									// Подключение к GPRS и проверка подключения
		}
	}

	Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("============================")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
	Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Running controller...")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
}


void setup() {
	// ============================ Настройка внутренней начинки ============================
	ControllAllPortsOutput();				// Настройка всех портов в INPUT	
	
	bt_vcc_off();
	delay(500);
	bt_vcc_on();							// Включение Bluetooth модуля
	
	Serial.begin(115200);					// UART MAIN
	Serial3.begin(115200);					// UART GSM
	
	analogReference(INTERNAL2V56);			// Источник опорного напряжения
	PCINT_init();							// Настройка PCINT (цифровые входы)
	ADC_init();								// Настройка ADC каналов
	delay(10000);							// Задержка чтобы контроллер успел измерить ADC в том числе и напряжение питания и успеть подключиться по UART
							
	ControllerSetup = true;					// Обозначаем стадию загрузки контроллера
	
	WDT_state(OFF);							// Выводим причину перезапуска контроллера
	
	
	// ====================================================================================
	Link_LogWebServer		= F("http://net.uniscan.biz/LogsController.php/?Type=1");
	Link_LogDataWebServer	= F("http://net.uniscan.biz/ValueSensors.php/?Type=1");
	
	GPRS_APN_NAME			= F("internet.beeline.ru");
	GPRS_APN_USER			= F("beeline");
	GPRS_APN_PASSWORD		= F("beeline");
	
	GSM_CODE_BALANCE		= F("#102#");	
	
	AllowPhone[0] = F("79139045925");
	AllowPhone[1] = F("79137517075");
	AllowPhone[2] = F("79138963482");
	AllowPhone[3] = F("79133774145");

	
	// =====================================================================================
	Serial.println(F("Initialization of the internal periphery:"));
		Serial.print(F("\tLCD..."));
			lcd.init();										// Инициализация LCD
			lcd.clear();									// Очистка экрана
			InitializingLCDicons();							// Инициализация значков для LCD экрана
			ViewSignalLevel(115);							// Выводим значек уровня сигнала GSM (пока отсутствие)
			Serial.println(F("done"));
			
		Serial.print(F("\tWire..."));
			Wire.begin();
			Serial.println(F("done"));
	
		Serial.print(F("\tTime..."));
			time.begin();									// Инициализация RTC
			time.period(60);								// Период опроса 60 сек
			Serial.println(F("done"));
			
				
	
	Serial.println();
	Serial.println(F("===================================================="));
	Serial.println(F("================= Controller setup ================="));
	Serial.println(F("===================================================="));
	Serial.println();
	WriteToLCD(String(F("=Setup Controller=")), LCD_LINE_1, LCD_START_SYMBOL_1, LCD_NO_SCREEN_REFRESH_DELAY);
	
/*	boolean ConfigModeController = false;
	while(EEPROM.read(E_ConfigModeController) == 1){	// Если контроллер в режиме конфигурирования
		if(!ConfigModeController){
			Serial.println(F("Контроллер запущен в режиме конфигурирования"));
			WriteToLCD(String(F("Controller in   ")), LCD_SCREEN_REFRESH_DELAY);
			WriteToLCD(String(F("config. mode    ")), LCD_NO_SCREEN_REFRESH_DELAY);
			status_led_red_on();
			status_led_blue_on();
			status_led_green_on();
			ConfigModeController = true;
		}
	}*/
	status_led_red_off();								// Гасим все статусные светодиоды
	status_led_blue_off();
	status_led_green_off();
	

	// ===================================================================================================================================
	// =================================================== Измерение напряжения питания ==================================================
	// ===================================================================================================================================	
	WriteToLCD(String(F("Input VCC")), LCD_LINE_2, LCD_START_SYMBOL_1, LCD_NO_SCREEN_REFRESH_DELAY);
	Serial.print(F("Input VCC: "));
	Dimension_VCC();											// Измерение напряжения
	Serial.print(VCC);
	if(VCC >= float(EEPROM.read(E_MinInputVCC)) / 10){			// Напряжение питания выше минимального
		WriteToLCD(String(F("...OK")), LCD_LINE_2, LCD_START_SYMBOL_10, LCD_SCREEN_REFRESH_DELAY);
		Serial.println(F("...OK"));
		if(EEPROM.read(E_ControllVCC) == true){					// Если включено контролирование VCC
			Low_Input_VCC = false;
		}
	}
	else{
		WriteToLCD(String(F("...LOW")), LCD_LINE_2, LCD_START_SYMBOL_10, LCD_NO_SCREEN_REFRESH_DELAY);
		WriteToLCD(String(F("...No start GSM   ")), LCD_LINE_3, LCD_START_SYMBOL_2, LCD_NO_SCREEN_REFRESH_DELAY);
		WriteToLCD(String(F("...No Exec modules ")), LCD_LINE_4, LCD_START_SYMBOL_2, LCD_SCREEN_REFRESH_DELAY);
		Serial.println(F("...LOW"));
		if(EEPROM.read(E_ControllVCC) == true){					// Если включено контролирование VCC
			Low_Input_VCC = true;								// Поднимаем флаг низкого напряжения питания
		}
		StatusLED(LED_ERROR_VCC);													
	}


	// ===================================================================================================================================
	// ======================================= Включение и настройка внутреннего термостата в LM75 =======================================
	// ===================================================================================================================================
	if(EEPROM.read(E_ControllVCC) == true){									// Если включено контролирование VCC
		if(!Low_Input_VCC){													// Если напряжение питания в норме
			Configuration_LM75(ADDRESS_INPUT_LM75, EEPROM.read(E_INT_LM75_THYST), EEPROM.read(E_INT_LM75_TOS));		// Настраиваем встроенный датчик температуры LM75
		}
	}
	

	// ===================================================================================================================================
	// =========================================== Включение, инициализация GSM модуля и GPRS ============================================
	// ===================================================================================================================================
 	if(EEPROM.read(E_WorkSIM800) == ON){							// Если модуль настроен на постоянную работу
		if(!Low_Input_VCC){											// Если напряжение питания в норме
			Start_Init_GSM();
		}
		else{
			if(1 <= EEPROM.read(E_GSM_STATE_MIN_VCC) <= 2){			// Если настроена работа GSM при понижении питания контроллера (работает или сон)
				Start_Init_GSM();
			}
		}
	}
	
	
	// ===================================================================================================================================
	// ========================================= Отправка причины перезапуска контроллера по GPRS ========================================
	// ===================================================================================================================================
	switch(Code_Reason_WDT){
		case 4:
			Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + GSM_GET_Tab_1 + (F("Cause of reboot: Low voltage")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
			break;
		case 5:
			Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + GSM_GET_Tab_1 + (F("Cause of reboot: On power")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
			break;
		case 8:
			Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + GSM_GET_Tab_1 + (F("Cause of reboot: Operation watchdog")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
			break;
	}


	// ===================================================================================================================================
	// ===================================================== Инициализация датчиков ======================================================
	// ===================================================================================================================================
	Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + GSM_GET_Tab_1 + (F("List of available sensors:")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
	Serial.println(F("List of available sensors: "));
	
	Clean_LCD(LCD_LINE_3, LCD_START_SYMBOL_1);
	Clean_LCD(LCD_LINE_4, LCD_START_SYMBOL_1);
	WriteToLCD(String(F("Initializing Sensors")), LCD_LINE_2, LCD_START_SYMBOL_1, LCD_NO_SCREEN_REFRESH_DELAY);
	
	i2c_scaner(ON);
	DS18B20_scaner(ON);
	Serial.println(F("Initializing Sensors: "));
	for (byte NumberSensor = 1; NumberSensor <= QUANTITY_SENSORS; NumberSensor ++){
		if(EEPROM.read(E_StatusSensor + NumberSensor) == 1){									//  Если датчик включен 
			ConfigSensor(NumberSensor);
		}
	}
	for(byte Sensor = 1; Sensor <= QUANTITY_SENSORS; Sensor++){
		QuantityCalcSensors.QuantityCalc[Sensor] = 1;											// Обнуляем количество измерений
		for(byte i = 0; i <= 19; i++){															// Заполняем массив с названиями
			NameSensor[Sensor-1][i] = EEPROM.read(E_NameSensor + (Sensor - 1) * 20 + i);			
		}
	}	

	
	// ===================================================================================================================================
	// ========== Инициализация всех исполнительных модулей (возврящение их в нули или восстановление значений до перезагрузки) ==========
	// ===================================================================================================================================
	Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + GSM_GET_Tab_1 + (F("Initialization of executive modules:")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
	Serial.println(F("Initialization Exec Module"));
	Clean_LCD(LCD_LINE_2, LCD_START_SYMBOL_1);
	Clean_LCD(LCD_LINE_3, LCD_START_SYMBOL_1);
	Clean_LCD(LCD_LINE_4, LCD_START_SYMBOL_1);
	WriteToLCD(String(F("Init Exec Module: ")), LCD_LINE_2, LCD_START_SYMBOL_1, LCD_NO_SCREEN_REFRESH_DELAY);
	STEPPER_VCC_on();
	DigitalPort(PORT_EXT_PIN_1, DIGITAL_PORT_ON, DIGITAL_PORT_SWITCH_PORT, NO_LOG_TO_UART);		// Выключаем драйвер 1
	DigitalPort(PORT_EXT_PIN_2, DIGITAL_PORT_ON, DIGITAL_PORT_SWITCH_PORT, NO_LOG_TO_UART);		// Выключаем драйвер 2
	DigitalPort(PORT_EXT_PIN_3, DIGITAL_PORT_ON, DIGITAL_PORT_SWITCH_PORT, NO_LOG_TO_UART);		// Выключаем драйвер 3
	DigitalPort(PORT_EXT_PIN_5, DIGITAL_PORT_ON, DIGITAL_PORT_SWITCH_PORT, NO_LOG_TO_UART);		// Выключаем драйвер 4
	//DDRC |= _BV(PK5);																// STEPPER_STEP, OUTPUT
	//DDRC |= _BV(PK4);																// STEPPER_DIR, OUTPUT
	pinMode(STEPPER_STEP, OUTPUT);
	pinMode(STEPPER_DIR, OUTPUT);
	
	
	if(EEPROM.read(E_ReturnModulesToZeros) == OFF){												// Если после перезагрузки не хотим возвращать модули в нули
		Send_GET_request(String (F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + GSM_GET_Tab_2 + (F("Restoration of executive modules")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
		WriteToLCD(String(F("Restor variables:   ")), LCD_LINE_3, LCD_START_SYMBOL_1, LCD_SCREEN_REFRESH_DELAY);
		ArchiveRestoreSaveWordData(2);															// Восстановление рабочих переменных
		EEPROM.write(E_ReturnModulesToZeros, ON);
		WriteToLCD(String(F("OK")), LCD_LINE_2, LCD_START_SYMBOL_19, LCD_SCREEN_REFRESH_DELAY);
	}
	else{
		Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + GSM_GET_Tab_2 + (F("Return of executive modules in the initial position")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
		Clean_LCD(LCD_LINE_3, LCD_START_SYMBOL_1);
		WriteToLCD(String(F("Init Exec Module:")), LCD_LINE_3, LCD_START_SYMBOL_1, LCD_NO_SCREEN_REFRESH_DELAY);
		for(byte Module = 1; Module <= QUANTITY_EXEC_MODULES; Module++){	
			if(EEPROM.read(E_StatusModule + Module) == 1){										// И если модуль включен)	
				Clean_LCD(LCD_LINE_3, LCD_START_SYMBOL_1);
				Clean_LCD(LCD_LINE_4, LCD_START_SYMBOL_1);
				Serial.print(F("\t...Module ")); Serial.print(Module); 
				WriteToLCD(String(F("--- Module ")) + String(Module) + String(F(": ")), LCD_LINE_3, LCD_START_SYMBOL_3, LCD_NO_SCREEN_REFRESH_DELAY);
				
				if(!InitializingExecModule(Module)){											// Если не висит ошибка инициализации модуля
					Serial.println(F("...done"));
					WriteToLCD(String(F("DONE")), LCD_LINE_3, LCD_START_SYMBOL_17, LCD_SCREEN_REFRESH_DELAY);
					Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + GSM_GET_Tab_2 + (F("Module ")) + String(Module) + (F(": done")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
				}
				else{
					Serial.println(F("...error"));
					WriteToLCD(String(F("ERROR")), LCD_LINE_3, LCD_START_SYMBOL_16, LCD_SCREEN_REFRESH_DELAY);
					Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + GSM_GET_Tab_2 + (F("Module ")) + String(Module) + (F(": error")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
				}
			}
		}	
	}
  

	// ===================================================================================================================================
	// ================================================ Сигнализация о запуске контролера ================================================
	// ===================================================================================================================================
	Serial.println();
	Serial.println(F("====================================================================="));
	Serial.print(F("============== Controller started,  ")); Serial.print(VersionFirmware); Serial.println(F("  =============="));
	Serial.println(F("====================================================================="));
	
	lcd.clear();
	WriteToLCD(String(F("Controller Run")), LCD_LINE_2, LCD_START_SYMBOL_4, LCD_NO_SCREEN_REFRESH_DELAY);
	WriteToLCD(Short_VersionFirmware, LCD_LINE_3, LCD_START_SYMBOL_6, LCD_SCREEN_REFRESH_DELAY);	
	
	Send_GET_request(String (F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("--- Controller started, ver. ")) + String(VersionFirmware) + (F(" ---")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
	
	StatusLED(LED_START_CONTROLLER);						// Световая сигнализация
	
	CalculateSensors();										// Начальное измерение показаний датчиков
	
	WindowMenu(LCD_SCREEN_MAIN,LCD_CONCLUSION_SCREEN);		// Стартовый экран на LCD
	WindowMenu(LCD_SCREEN_MAIN,LCD_UPDATE_SCREEN);			// Значение переменных
	
	// ============================ Отправка СМС о старте контроллера ============================
	if(EEPROM.read(E_SentSMSorStartController) == ON){							// Если настроена отправка СМС о старте контроллера
		if(EEPROM.read(E_RebutingFromGSM) == 1){								// Если перезагрузку потребовали по СМС
			if(StateGSM.GSM_Registered){										// Если GSM модуль зарегистрирован в сети
				Send_SMS(String (F("Controller is rebooting")), GSM_INFO_SMS);
				EEPROM.write(E_RebutingFromGSM, 0);
			}
			else SendSMSorStartController = true;					// Поднимаем флаг чтобы отправить СМС как будет регистрация
		}
		else{														// иначе считаем что был "холодный старт"
			if(StateGSM.GSM_Registered){							// Если модуль зарегистрирован в сети
				Send_SMS(TextOfStartController, GSM_INFO_SMS);		// Отправка СМС
			}
			else SendSMSorStartController = true;					// Поднимаем флаг чтобы отправить СМС как будет регистрация
		}
	}
	
	LOGING_TO_SERIAL = EEPROM.read(E_LOGING_TO_SERIAL);		// Установка уровня логирования в Serial1
	ControllerSetup = false;								// Выключение режима инициализации контроллера
	
	// ================================== Выключение LCD экрана ==================================
	lcd.noBacklight();
	lcd.noDisplay();
	
	// =================================== Настройка прерываний ==================================
	Timer5_init();											// Настройка пятого таймера
	Int_encoder_init();										// Настройка энкодера
	sei();													// Глобально разрешаем прерывания
	
	wdt_enable(WDTO_8S);									// Включаем watchdog
	set_sleep_mode(SLEEP_MODE_IDLE);						// Настраиваем режим сна
	sleep_mode();											// Усыпляем контроллер
}

// ===================================================================================================================================
// ===================================================================================================================================
// ===================================================================================================================================


void loop() {		
	// =============================================================================================================================
	// =========================================== Проверка работоспособности GSM модуля ===========================================
	// =============================================================================================================================
	static bool Cyclic_Network_Signal_Strength_Check;
	// ------------------------------------------------
	if(Cyclic_Network_Signal_Strength_Check && !Low_Input_VCC){						// Если висит задача циклической проверки уровня сигнала сети
		Signal_Level(GSM_NO_OUTPUT_TO_SERIAL, GSM_PERFORM_MEASUREMENT);	
		if(0 < StateGSM.GSM_Signal_Level < 50){										// Проверяем уровень сигнала, выводим его в UART и если уровень в рабочем диапазоне
			Cyclic_Network_Signal_Strength_Check = false;							// Отключаем задачу циклической проверки уровня сигнала сети
			Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Signal level GSM is OK")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
		}
		else{																		// Иначе пробуем перезапуститьпитание у модуля				
			Power_GSM(RESET);														// Перезапускаем питание модуля
		}
	}
	// ------------------------------------------------
	if(EEPROM.read(E_WorkSIM800) == ON && !Cyclic_Network_Signal_Strength_Check && !Low_Input_VCC){		// Если модуль настроен на постоянную работу и нет ошибки слабого сигнала сети
		Signal_Level(GSM_NO_OUTPUT_TO_SERIAL, GSM_PERFORM_MEASUREMENT);									// Только измеряем уровень сигнала сети
		if(T_second > (LoopCheckRegistrationGSM + EEPROM.read(E_IntervalCheckRegistrationGSM))){		// Интервал проверки регистрации GSM
			LoopCheckRegistrationGSM = T_second;
			switch(Check_Registration_GSM(GSM_OUTPUT_TO_SERIAL, LCD_NO_OTPUT_ON_SCREEN)){				// Проверка регистрации GSM
				case GSM_NO_FIND_NETWORK:																// Нет поиска сети
					// Обычно происходит когда модуль помер или у него отключено питание
					Power_GSM(ON);																		// Подаем питание на модуль
					break;
				case GSM_REGISTERED:																	// Зарегистрирован в сети
					if(OUTPUT_LEVEL_UART_GSM){
						Signal_Level(GSM_OUTPUT_TO_SERIAL, GSM_DO_NOT_MEASURE);							// Дополнительно выводим в UART уровень сигнала сети (с описаниями). Измерение не проводим!!!!!
					}
					break;
				case GSM_FINDING_NETWORK:																// Поиск сети
					// Что то еще нужно сделать?
					if(OUTPUT_LEVEL_UART_GSM){
						Signal_Level(GSM_OUTPUT_TO_SERIAL, GSM_DO_NOT_MEASURE);							// Выводим уровень сигнала сети (с описаниями), измерение не проводим
					}
					break;
				case GSM_REGISTRED_REJECTED:															// Регистрация отклонена
					// Что то еще нужно сделать?
					break;
				case GSM_REGISTERED_UNKNOWN:															// Неизвестно
					switch(Check_Readiness_GSM_Module(GSM_NO_OUTPUT_TO_SERIAL, LCD_NO_OTPUT_ON_SCREEN)){	// Проверяем готовность модуля (отправляем команду "AT")
						case GSM_MODULE_NOT_SIM_CARD:														// Нет SiM карты
							break;
						case GSM_MODULE_READY:																// Если готов (ответил "ОК")
							if(0 < StateGSM.GSM_Signal_Level < 50){											// Если уровень сигнала сети в рабочем диапазоне
								Serial.print(F("Balance SIM card: ")); Serial.println(StateGSM.Balance);
								Initializing_GSM(LCD_NO_OTPUT_ON_SCREEN);									// Инициализируем
								Connecting_GPRS(LCD_NO_OTPUT_ON_SCREEN);									// Запускаем подключение к GPRS
								Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Reinit GSM and restart GPRS")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
							}
							else{
								Cyclic_Network_Signal_Strength_Check = true;								// Заставляем в основном цикле loop() проверять уровень сигнала чтобы не
																											// запускать просто так всю проверку регистрацию GSM
							}
							break;
						case GSM_MODULE_UNKNOWN:															// Неизвестно						}
							if(0 < StateGSM.GSM_Signal_Level < 50){											// Если уровень сигнала сети в рабочем диапазоне
								Power_GSM(RESET);															// Перезапускаем питание модуля
								Initializing_GSM(LCD_NO_OTPUT_ON_SCREEN);									// Инициализируем
								Connecting_GPRS(LCD_NO_OTPUT_ON_SCREEN);									// Запускаем подключение к GPRS
								Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Restart GSM module")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
							}
							else{
								Cyclic_Network_Signal_Strength_Check = true;		// Заставляем в основном цикле loop() проверять уровень сигнала чтобы не
																					// запускать просто так всю проверку регистрацию GSM
							}
							break;				
					}
					break;
			}
		}
	}
	
	
	// =============================================================================================================================
	// ================================================== Проверка GPRS соединения =================================================
	// =============================================================================================================================
	static bool _WaitingForGPRSConnection;												// Ожидание соединения по GPRS. Поднимается во время проверки соединения
	static bool _WaitingForGPRSclosure;
	
	if(EEPROM.read(E_AllowGPRS) == ON && StateGSM.Code_Connect_GSM == GSM_REGISTERED){	// Если разрешена работа GPRS и модем зарегистрирован в сети		
		if(T_second > (LoopCheckIntervalGPRS + 5)){										// Проверяем регистрации GPRS через каждые 5 секунд
			LoopCheckIntervalGPRS = T_second;
			bool _ViewLog = false;
			if(!Check_Connection_GPRS(GSM_OUTPUT_TO_SERIAL)) {							// Проверяем регистрацию GPRS и если не подключен
				switch(StateGSM.Code_Connect_GPRS){										// Разбираемся с ошибками GPRS
					case GPRS_CONNECTION_IS_ESTABLISHED:								// Соединение устанавливается
						_WaitingForGPRSConnection = true;
						Serial.println(F("_WaitingForGPRSConnection"));
						break;
					case GPRS_CONNECTION_CLOSED:										// Соединение закрывается
						_WaitingForGPRSclosure = true;
						Serial.println(F("_WaitingForGPRSclosure"));
						break;
					case GPRS_NO_CONNECTED:												// Нет соединения
																						// Заново устанавливаем GPRS соединение
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.print(F("Restart connecting GPRS: "));
						}
						String GPRS_ATs[] = {											// массив "АТ" комманд инициализации GPRS
												(F("AT+SAPBR=1,1")),					// Установка GPRS соединения
												(F("AT+HTTPINIT")),						// Инициализация http сервиса				
						};
						String Word;
						for (byte i = 0; i < sizeof(GPRS_ATs) / sizeof(GPRS_ATs[0]); i++) {
							wdt_reset();
							Word = send_AT_Command(GPRS_ATs[i], GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);				// Отправляем АТ команду и ждем ответ
							byte TimerCommand = 1;
							while(TimerCommand <= 4){																		// Максимальное кол-во отправок команды
								if(Word.lastIndexOf(F("OK")) != -1){														// Если ответ "OK"
									goto end_while;
								}
								else{
									Word = send_AT_Command(GPRS_ATs[i], GSM_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);		// Повторно отправляем команду
									TimerCommand ++;
								}
							}
							end_while: ; 
						}
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.println(F("OK"));
						}
						Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Restart connect GPRS")) + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
						break;
				}
			}
		}
	};
	
	
	
	// =============================================================================================================================
	// =========================== Временные интервалы для всех ф-ций!!! Самая важная процедура!!!!!!!!!!!! ========================
	// =============================================================================================================================
	TimeIntervals();				
	
	
	// =============================================================================================================================
	// ===================== Отправка отложенной СМС о старте контроллера как только GSM зарегистрируетя в сети ====================
	// =============================================================================================================================
	if(SendSMSorStartController && StateGSM.GSM_Registered){
		if(EEPROM.read(E_RebutingFromGSM) == 1){
			Send_SMS(String (F("Controller is rebooting")), GSM_INFO_SMS);
			EEPROM.update(E_RebutingFromGSM, 0);
		}
		else Send_SMS(TextOfStartController, GSM_INFO_SMS);			// Отправка СМС
		SendSMSorStartController = false;
	}
	
	
	// =============================================================================================================================
	// ==================================== Измерение показаний встроенного датчика температуры ====================================
	// =============================================================================================================================
	if(T_second > (LoopReadInternalTemp + 5)){						// Раз в 5 секунд
		LoopReadInternalTemp = T_second;
		Ti =  ReadValueLM75(0, ADDRESS_INPUT_LM75);						
	}
	
	
	// =============================================================================================================================
	// ========================================== Проверка почему не отправлен GET запрос ==========================================
	// =============================================================================================================================
	if(StateGSM.Error_Sent_GET){								// Если висит ошибка что GET запрос не отправлен		

	}
	
	
	// =============================================================================================================================
	// ========================================= Управление напряжением питания ====================================================
	// =============================================================================================================================
	Dimension_VCC();							// Измерение напряжения питания

	if(EEPROM.read(E_ControllVCC) == 1){		// Если включено контролирование VCC
		ManagementVCC();						// Проверяем напряжение питания
	}
	
	
	// =============================================================================================================================
	// ============================================== Обработка энкодера ===========================================================
	// =============================================================================================================================
	if(OnEncoderKey){
		Turning_Backlight_LCD();						
		OnEncoderKey = false;
	}
	
	
	// =============================================================================================================================
	// ========================================= Обновление данных на LCD экране ===================================================
	// =============================================================================================================================	
	if(LightLCDEnable){									// Если включена подсветка экрана
		UpdateMenu();									// Обновление данных на экране при простое
	}
	
	
	// =============================================================================================================================
	// ============================================== Парсинг входящих GSM сообщений ===============================================
	// =============================================================================================================================
	serial3_ISR();										
	
	
	// =============================================================================================================================
	// =============================================================================================================================
	// =============================================================================================================================
	set_sleep_mode(SLEEP_MODE_IDLE);					// Устанавливаем интересующий нас режим
	sleep_mode();										// Переводим МК в спящий режим
}