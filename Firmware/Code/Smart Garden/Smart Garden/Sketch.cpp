
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
	ADMUX = 0b01000000;		// Измеряем на ADC0, опорное напряжение: "AVCC with external capacitor at AREF pin"
	ADCSRA = 0b10001100;		// Включаем АЦП, разрешаем прерывания, делитель = 16
	ADCSRB = 0b00000000;		// Включаем ADC каналы, режим единичной выборки
	bitWrite(ADCSRA, 6, 1);	// Запускаем преобразование (1 в ADSC)
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
	PCMSK0 = (1<<PCINT5)|(1<<PCINT6)|(1<<PCINT7);
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
	//Serial.print(F("T_second: ")); Serial.println(T_second);
}

ISR(INT4_vect){						// Кнопка энкодера
	OnEncoderKey = true;
}

ISR(INT5_vect){						// Крутилка энкодера
	Serial.println(F("Key INT5"));
}


// Прерывания концевиков
ISR(PCINT0_vect) {
	if (!(PINB & (1 << INPUT_D_PIN_1))) {StatusSwitchPorts[1] = true;	/*Serial.println("INPUT_D_PIN_1");*/}		// INPUT_D_PIN_1 - PCINT5
	else StatusSwitchPorts[1] = false;
	if (!(PINB & (1 << INPUT_D_PIN_2))) {StatusSwitchPorts[2] = true;	/*Serial.println("INPUT_D_PIN_2");*/}		// INPUT_D_PIN_2 - PCINT6
	else StatusSwitchPorts[2] = false;
	if (!(PINB & (1 << INPUT_D_PIN_3))) {StatusSwitchPorts[3] = true;	/*Serial.println("INPUT_D_PIN_3");*/}		// INPUT_D_PIN_3 - PCINT7
	else StatusSwitchPorts[3] = false;
}

ISR(PCINT1_vect) {
	if (!(PINJ & (1 << INPUT_D_PIN_4))) {StatusSwitchPorts[4] = true;	/*Serial.println("INPUT_D_PIN_4");*/}		// INPUT_D_PIN_4 - PCINT11
	else StatusSwitchPorts[4] = false;
	//if (!(PINJ & (1 << INPUT_D_PIN_5))) {StatusSwitchPorts[5] = true; /*Serial.println("INPUT_D_PIN_5");*/}		// INPUT_D_PIN_5 - PCINT9
	//else StatusSwitchPorts[5] = false;
	if (!(PINJ & (1 << INPUT_D_PIN_6))) {StatusSwitchPorts[6] = true;	/*Serial.println("INPUT_D_PIN_6");*/}		// INPUT_D_PIN_6 - PCINT13
	else StatusSwitchPorts[6] = false;
	if (!(PINJ & (1 << INPUT_D_PIN_7))) {StatusSwitchPorts[7] = true;	/*Serial.println("INPUT_D_PIN_7");*/}		// INPUT_D_PIN_7 - PCINT12
	else StatusSwitchPorts[7] = false;
	if (!(PINJ & (1 << INPUT_D_PIN_8))) {StatusSwitchPorts[8] = true;	/*Serial.println("INPUT_D_PIN_8");*/}		// INPUT_D_PIN_8 - PCINT15
	else StatusSwitchPorts[8] = false;
	if (!(PINJ & (1 << INPUT_D_PIN_9))) {StatusSwitchPorts[9] = true;	/*Serial.println("INPUT_D_PIN_9");*/}		// INPUT_D_PIN_9 - PCINT14
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
	//Serial.print("NumberMeasurementsADC = "); Serial.println(NumberMeasurementsADC);
	BuffADC_VCC = ADCL;
	BuffADC_VCC += ADCH << 8;
	SummADC += BuffADC_VCC;
	//Serial.print("SummADC = "); Serial.println(SummADC);
	if(NumberMeasurementsADC == 250){	// Измеряем уровень ADC 60 раз для увеличения точности
		ADC = SummADC / 60;
		AllowSaveADC = true;			// Разрешаем сохранять измеренное показание
		NumberMeasurementsADC = 0;		// Обнуляем счетчик если измерили 60 раз
		SummADC = 0;
	}
	//Serial.print("ADC = "); Serial.println(ADC);
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
	delay(10000);							// Задержка чтобы контроллер успел измерить ADC в том числе и напряжение питания и успеть подключиться к контроллеру по UART
							
	ControllerSetup = true;					// Обозначаем стадию загрузки контроллера
	
	WDT_state(OFF);							// Выводим причину перезапуска контроллера
	
	CfCalcDC = (float(EEPROM_int_read(E_CfCalcDC)) / 100) / 1023 * EEPROM.read(E_InputVCC);		// Поправочный коэфициент для вычисления VCC
	VCC = RealValueADC[15] * CfCalcDC;															// Вычмсляем апряжение питания
		
	Link_LogWebServer		= "http://net.uniscan.biz/LogsStand.php/?Type=1";
	Link_LogDataWebServer	= "http://net.uniscan.biz/ValueStand.php/?Type=1";
	
	// =====================================================================================
	Serial.println(F("Initialization of the internal periphery:"));
		Serial.print(F("\tLCD..."));
			lcd.init();										// Инициализация LCD
			lcd.clear();									// Очистка экрана
			InitializingLCDicons();							// Инициализация значков для LCD экрана
			ViewSignalLevel(99);							// ВЫводим значек уровня сигнала GSM (пока отсутствие)
		
		Serial.print(F("\tWire..."));
			Wire.begin();
			Serial.println(F("done"));
	
		Serial.print(F("\tTime..."));
			time.begin();									// Инициализация RTC
			time.period(60);								// Период опроса 60 сек
			Serial.println(F("done"));
	Serial.println(F("done"));
	
	
	Serial.println();
	Serial.println(F("===================================================="));
	Serial.println(F("================= Controller setup ================="));
	Serial.println(F("===================================================="));
	Serial.println();
	WriteToLCD(String(F("=Setup Controller=")), LCD_LINE_1, LCD_START_SYMBOL_1, false);
	
	
/*	boolean ConfigModeController = false;
	while(EEPROM.read(E_ConfigModeController) == 1){	// Если контроллер в режиме конфигурирования
		if(!ConfigModeController){
			Serial.println(F("Контроллер запущен в режиме конфигурирования"));
			WriteToLCD(String(F("Controller in   ")), 1);
			WriteToLCD(String(F("config. mode    ")), 0);
			status_led_red_on();
			status_led_blue_on();
			status_led_green_on();
			ConfigModeController = true;
		}
	}*/
	status_led_red_off();								// Гасим все статусные светодиоды
	status_led_blue_off();
	status_led_green_off();
	

	// ================================ Измерение напряжения питания ================================
	WriteToLCD(String(F("Input VCC")), LCD_LINE_2, LCD_START_SYMBOL_1, false);
	Serial.print(F("Input VCC "));
	Serial.print(VCC);
	if(VCC >= float(EEPROM.read(E_MinInputVCC)) / 10){								// Напряжение питания ниже минимального
		WriteToLCD(String(F("...OK")), LCD_LINE_2, LCD_START_SYMBOL_10, true);
		Serial.println(F("...OK"));
		Low_Input_VCC = false;
	}
	else{
		WriteToLCD(String(F("...LOW")), LCD_LINE_2, LCD_START_SYMBOL_10, true);
		Serial.println(F("...LOW"));
		Low_Input_VCC = true;														// Поднимаем флаг низкого напряжения питания
		StatusLED(LED_ERROR_VCC);													
	}


	// ============================ Включение, инициализация GSM модуля и GPRS ============================
 	if(EEPROM.read(E_WorkSIM800) == ON && !Low_Input_VCC){		// Если модуль настроен на постоянную работу и напряжение питания в норме
		WriteToLCD(String(F("== Init GSM module =")), LCD_LINE_2, LCD_START_SYMBOL_1, false);
		Power_GSM(ON);											// Подаем питание на модуль
		SignalLevel(ON);										// Проверяем уровень сигнала сети
		ViewSignalLevel(StateGSM.GSM_Signal_Level);				// Выводим уровень сигнала GSM сети
		if(Check_Readiness_Module(ON) == 1){					// Проверяем готовность модуля (выводим лог в Serial) и если готов (вернул единицу)
			InitializingGSM();									// то инициализируем GSM			
			byte a = 0;											// Счетчик времени ожидания соединения
			while(a <= 20){										// Максимальное время ожидания регистрации 20 сек
				if(CheckRegistrationGSM(ON)){					// Если GSM модуль зарегистрирован в сети (выводим лог в Serial)
					
					delay(50);
					if(EEPROM.read(E_AllowGPRS) == ON){			// Если разрешена работа GPRS
						WriteToLCD(String(F("=== Connect GPRS ===")), LCD_LINE_2, LCD_START_SYMBOL_1, false);
						Clean_LCD(LCD_LINE_3);
						Clean_LCD(LCD_LINE_4);
						InitializingGPRS();						// Инициализируем GPRS
					}
					break;
				}
				a++;
				_delay_ms(1000);
			}
		}
	}
	
	
	// ====================== Отправка причины перезапуска контроллера по GPRS ======================
	switch(Code_Reason_WDT){
		case 4:
			SendGETrequest(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Controller setup: Low voltage")) + (F("\"")));
			break;
		case 5:
			SendGETrequest(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Controller setup: On power")) + (F("\"")));
			break;
		case 8:
			SendGETrequest(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Controller setup: Operation watchdog")) + (F("\"")));
			break;
	}

	
	// =============================== Инициализация датчиков ===============================
	SendGETrequest(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("List of available sensors")) + (F("\"")));
	Serial.println(F("List of available sensors: "));
	i2c_scaner(ON);
	DS18B20_scaner(ON);
	Serial.println(F("Initializing Sensors: "));
	for (byte NumberSensor = 1; NumberSensor <= QuantitySensors; NumberSensor ++){
		if(EEPROM.read(E_StatusSensor + NumberSensor) == 1){				//  Если датчик включен 
			ConfigSensor(NumberSensor);
		}
	}
	for(byte Sensor = 1; Sensor <= QuantitySensors; Sensor++){
		QuantityCalcSensors.QuantityCalc[Sensor] = 1;						// Обнуляем количество измерений
		for(byte i = 0; i <= 19; i++){										// Заполняем массив с названиями
			NameSensor[Sensor-1][i] = EEPROM.read(E_NameSensor + (Sensor - 1) * 20 + i);			
		}
	}
	
	ConfigurationInputTempSensor();											// Настраиваем встроенный датчик температуры LM75	
	
	
	// ================== Инициализация всех исполнительных модулей (возврящение их в нули) ====================
	SendGETrequest(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Initialization Exec Module")) + (F("\"")));
	Serial.println(F("Initialization Exec Module"));
	Clean_LCD(LCD_LINE_3);
	Clean_LCD(LCD_LINE_4);
	WriteToLCD(String(F("Init Exec Module: "/*18 символов*/)), LCD_LINE_2, LCD_START_SYMBOL_1, false);
	STEPPER_VCC_off();
	//DDRC |= _BV(PK5);														// STEPPER_STEP, OUTPUT
	//DDRC |= _BV(PK4);														// STEPPER_DIR, OUTPUT
	pinMode(STEPPER_STEP, OUTPUT);
	pinMode(STEPPER_DIR, OUTPUT);
	WriteToLCD(String(F("OK")), LCD_LINE_2, LCD_START_SYMBOL_19, false);
	
	if(EEPROM.read(E_ReturnModulesToZeros) == OFF){							// Если после перезагрузки не хотим возвращять модули в нули
		SendGETrequest(String (F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Restor variables exec modules")) + (F("\"")));
		WriteToLCD(String(F("Restor variables:   ")), LCD_LINE_2, LCD_START_SYMBOL_1, false);
		ArchiveRestoreSaveWordData(2);										// Восстановление рабочих переменных
		EEPROM.write(E_ReturnModulesToZeros, ON);
		WriteToLCD(String(F("OK")), LCD_LINE_2, LCD_START_SYMBOL_18, true);
	}
	else{
		SendGETrequest(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Initializing exec modules")) + (F("\"")));
		WriteToLCD(String(F("Init Exec Module    ")), LCD_LINE_2, LCD_START_SYMBOL_1, false);
		for(byte Module = 1; Module <= QuantityExecModule; Module++){	
			if(EEPROM.read(E_StatusModule + Module) == 1){					// И если модуль включен)	
				Serial.print(F("\t...Module ")); Serial.print(Module); 
				WriteToLCD(String(F("Module ")) + String(Module) + String(F(": ")), LCD_LINE_3, LCD_START_SYMBOL_3, false);
				
				if(InitializingExecModule(Module)){
					Serial.println(F("...done"));
					WriteToLCD(String(F("DONE")), LCD_LINE_3, LCD_START_SYMBOL_9, true);
				}
				else{
					Serial.println(F("...error"));
					WriteToLCD(String(F("ERROR")), LCD_LINE_3, LCD_START_SYMBOL_9, true);
				}
			}
		}	
	}
    
	CalculateSensors();										// Начальное измерение показаний датчиков


	// ===========================================================================================
	// ============================ Сигнализация о запуске контролера ============================
	// ===========================================================================================
	Serial.println();
	Serial.println(F("====================================================================="));
	Serial.print(F("============== Controller started,  ")); Serial.print(VersionFirmware); Serial.println(F("  =============="));
	Serial.println(F("====================================================================="));
	
	SendGETrequest(String (F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Controller started")) + (F("\"")));
	
	lcd.clear();
	//WriteToLCD(String(F(" ")), LCD_LINE_1, LCD_START_SYMBOL_1);
	WriteToLCD(String(F("Controller Run")), LCD_LINE_2, LCD_START_SYMBOL_4, false);
	WriteToLCD(Short_VersionFirmware, LCD_LINE_3, LCD_START_SYMBOL_6, true);
	//WriteToLCD(String(F(" ")), LCD_LINE_4, LCD_START_SYMBOL_1);
	
    StatusLED(LED_START_CONTROLLER);						// Световая сигнализация
    
	delay(2000);
	
	lcd.noBacklight();
    lcd.noDisplay();
    WindowMenu(1,0);										// Стартовый экран на LCD
    WindowMenu(1,1);										// Значение переменных
	
	
	// ============================ Отправка СМС о старте контроллера ============================
	if(EEPROM.read(E_SentSMSorStartController) == ON){				// Если настроена отправка СМС о старте контроллера
		if(EEPROM.read(E_RebutingFromGSM) == 1){					// Если перезагрузку потребовали по СМС
			if(StateGSM.GSM_Registered){							// Если GSM модуль зарегистрирован в сети
				SendSMS(String (F("Controller is rebooting")), 0);
				EEPROM.write(E_RebutingFromGSM, 0);
			}
			else SendSMSorStartController = true;			// Поднимаем флаг чтобы отправить СМС как будет регистрация
		}
		else{												// иначе считаем что был "холодный старт"
			if(StateGSM.GSM_Registered){					// Если модуль зарегистрирован в сети
				SendSMS(TextOfStartController, 0);			// Отправка СМС
			}
			else SendSMSorStartController = true;			// Поднимаем флаг чтобы отправить СМС как будет регистрация
		}	
	}
	
	
	// ===========================================================================================
	LOGING_TO_SERIAL = EEPROM.read(E_LOGING_TO_SERIAL);		// Установка уровня логирования в Serial1
	ControllerSetup = false;								// Выключение режима инициализации контроллера
	
	Timer5_init();											// Настройка пятого таймера
	Int_encoder_init();										// Настройка энкодера
	sei();													// Глобально разрешаем прерывания
	wdt_enable(WDTO_8S);									// Включаем watchdog
	set_sleep_mode(SLEEP_MODE_IDLE);						// Настраиваем режим сна
	sleep_mode();											// Усыпляем контроллер
		
	
/*	
	MessageGSM.Arg_1_a = 1;		// Тест
	MessageGSM.Arg_1_b = 2;
	MessageGSM.Arg_2_a = 2;		// Номер датчика
	MessageGSM.Arg_2_b = 14;
	MessageGSM.Arg_3_a = 1;		// Текст
	MessageGSM.Arg_3_b = 3;
	MessageGSM.PhoneNumber = 0;
	WriteToQueueGSM(MessageGSM);
	
	MessageGSM.Arg_1_a = 1;		// Тест
	MessageGSM.Arg_1_b = 0;
	MessageGSM.Arg_2_a = 2;		// Номер датчика
	MessageGSM.Arg_2_b = 15;
	MessageGSM.Arg_3_a = 1;		// Текст
	MessageGSM.Arg_3_b = 4;
	MessageGSM.Arg_4_a = 1;		// Текст
	MessageGSM.Arg_4_b = 5;
	MessageGSM.PhoneNumber = 0;
	WriteToQueueGSM(MessageGSM);
	
	ReadFromQueueGSM(0);			// Тестово выводим все содержимое очереди сообщений*/

}




void loop() {		
	
	// =============================================================================================================================
	// ===================== Отправка отложенной СМС о старте контроллера как только GSM зарегистрируетя в сети ====================
	// =============================================================================================================================
	if(SendSMSorStartController && StateGSM.GSM_Registered){
		if(EEPROM.read(E_RebutingFromGSM) == 1){
			SendSMS(String (F("Controller is rebooting")), 0);
			EEPROM.update(E_RebutingFromGSM, 0);
		}
		else SendSMS(TextOfStartController, 0);			// Отправка СМС
		SendSMSorStartController = false;
	}
	
	if(T_second > (LoopReadInternalTemp + 1)){			// Измерение показаний встроенного датчика температуры
		LoopReadInternalTemp = T_second;
		Ti = lm75(ADDRESS_INPUT_TEMP_SENSOR);						
	}
	
	// =============================================================================================================================
	// ========================================== Проверка почему не отправлен GET запрос ==========================================
	// =============================================================================================================================
	static bool _WaitingForGPRSConnection;							// Ожидание соединения по GPRS. Поднимается во время проверки соединения
	static bool _WaitingForGPRSclosure; 
	
	if(StateGSM.Error_Sent_GET){									// Если висит ошибка что GET запрос не отправлен		
		if(CheckConnectionGPRS()){									// Проверяем регистрацию GPRS и если подключен
			if(StateGSM.IP_GPRS.lastIndexOf(F("0.0.0.0")) != 1){	// Если получен IP адрес (дополнительная проверка регистрации в сети)
				/* Вариантов два:
									херовый уровень сигнала сети
									нет денег на счете???? */
				SignalLevel(ON);								// Проверяем уровень сигнала сети
			}
		} //
		else{													// если не подключен GPRS
			if(CheckRegistrationGSM(ON)){						// Проверяем регистрацию GSM и если зарегистрирован	
				switch(StateGSM.Code_Connect_GPRS){				// Разбираемся с ошибками GPRS
					case 0:										// Соединение устанавливается
						_WaitingForGPRSConnection = true;
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.println();
							Serial.println(F("Соединение устанавливается")); 
						}
						break;
					case 1:										// Соединение установлено
						SignalLevel(ON);						// Проверяем уровень сигнала сети 
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.println();
							Serial.println(F("Соединение установлено"));
						}
						break;
					case 2:										// Соединение закрывается
						_WaitingForGPRSclosure = true;
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.println();
							Serial.println(F("Соединение закрывается"));
						}
						break;
					case 3:										// Нет соединения
						if(OUTPUT_LEVEL_UART_GSM){ 
							Serial.println();
							Serial.println(F("Нет соединения"));
						}
						// Заного устанавливаем GPRS соединение
						String GPRS_ATs[] = {										// массив АТ команд инициализации GPRS
												(F("AT+SAPBR=1,1")),				// Установка GPRS соединения
												(F("AT+HTTPINIT")),					// Инициализация http сервиса
						};
						String Word;
						for (byte i = 0; i < sizeof(GPRS_ATs) / sizeof(GPRS_ATs[0]); i++) {
							wdt_reset();
							Word = sendATCommand(GPRS_ATs[i], YES, YES);			// Отправляем АТ команду, ждем, получаем и выводим ответ в Serial
							byte TimerCommand = 1;
							while(TimerCommand <= 4){								// Максимальное кол-во отправок команды
								if(Word.lastIndexOf(F("OK")) != -1){					// Если ответ "OK"
									TimerCommand = 5;								// Останавливаем выполнение цикла while
									goto end_while;
								}
								else{
									Word = sendATCommand(GPRS_ATs[i], YES, NO);		// Повторно отправляем команду
									TimerCommand ++;
								}
							}
							end_while: ; 
						}
						CheckConnectionGPRS();										// Проверяем подключение
						break;
				}
			}
		}
	}
	
	
	// =============================================================================================================================
	// =========================================== Проверка работоспособности GSM модуля ===========================================
	// =============================================================================================================================
	if(EEPROM.read(E_WorkSIM800) == ON){															// Если модуль настроен на постоянную работу
		if(T_second > (LoopCheckRegistrationGSM + EEPROM.read(E_IntervalCheckRegistrationGSM))){	// Интервал проверки регистрации GSM
			LoopCheckRegistrationGSM = T_second;
			if(CheckRegistrationGSM(ON)){				// Проверяем регистрацию GSM и если зарегистрирован	
				if(OUTPUT_LEVEL_UART_GSM){
					SignalLevel(ON);					// Дополнительно выводим уровень сигнала сети
				}
			}				
			else{										// Если нет
				if(Check_Readiness_Module(ON) == 1){	// то проверяем готовность модуля (отправляем комманду "AT") и если готов (ответил "ОК" и в переменной лежит "1")
					if(0 < SignalLevel(ON)< 50){		// Проверяем уровень сигнала сети
						// Что делать дальше????
					}
				}
				else{									// иначе модуль не готов и пробуем его просто перезапустить и заного проинициализировать
					Serial.println(F("Модуль не готов"));
					Power_GSM(RESET);								// Перезапускаем питание
					InitializingGSM();								// Инициализируем
					byte a = 0;
					while(a <= 5){									// Максимальное время ожидания регистрации 5 сек
						if(CheckRegistrationGSM(true)){				// Если GSM модуль зарегистрирован в сети
							_delay_ms(50);
							if(EEPROM.read(E_AllowGPRS) == ON){		// Если разрешена работа GPRS
								InitializingGPRS();					// Инициализируем GPRS
							}
							break;
						}
						a++;
						_delay_ms(1000);
					}
				}
			}
		}
	}
	
	
	// ======================== Управление напряжением питания ==================================
	static byte QuantityCalculateVCC;				// Счетчик количества измерений VCC
	static float SummQuantityCalculateVCC;			// Для суммирования измеренных значений VCC 
	
	if(QuantityCalculateVCC <= 20){					// 10 раз измеряем VCC (для увеличения точности)
		SummQuantityCalculateVCC += RealValueADC[15] * CfCalcDC;	// Получаем напряжение питания
		QuantityCalculateVCC ++;					// Увеличиваем счетчик количества измерений
	}
	else{ 
		VCC = SummQuantityCalculateVCC / 20;		// Получаем среднее значение VCC (примерно за 10 секунд)
		QuantityCalculateVCC = 0;
		SummQuantityCalculateVCC = 0;
	}
	if(EEPROM.read(E_ControllVCC) == 1){			// Если включено контролирование VCC
		if(QuantityCalculateVCC == 0){				// Счетчик дошел до максимального значения и обнулился
			ManagementVCC();						// Проверяем напряжение питания
		}
	}
	// ==========================================================================================

	if(OnEncoderKey){
		EncoderKeyInt();
		OnEncoderKey = false;
	}

	TimeIntervals();					// Временные интервалы для всех ф-ций!!! Самая важная процедура!!!!!!!!!!!!
	
	serial3ISR();						// Парсинг GSM сообщений
	
	if(LightLCDEnable){					// Если включена подсветка экрана
		UpdateMenu();					// Обновление данных на экране при простое
		ViewSignalLevel(StateGSM.GSM_Signal_Level);
	}
		
	set_sleep_mode(SLEEP_MODE_IDLE);		// Устанавливаем интересующий нас режим
	sleep_mode();							// Переводим МК в спящий режим
}