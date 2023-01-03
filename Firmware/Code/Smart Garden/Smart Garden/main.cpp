#include <avr/wdt.h>
#include <Arduino.h>

#include "LCDdisplay.h"
//#include "SDcard.h"
#include "Sensors.h"
#include "Warning.h"
#include "main.h"
#include "GSM.h"
#include "ExecModules.h"
#include "DigitalPorts.h"
#include "ConfigSensors.h"
#include "RecievedUART\RecievedConfigController.h"


iarduino_RTC time(RTC_DS3231);


unsigned int LoopLCDLightTime;				// Интервал выключения подсветки LCD
unsigned int LoopSensorTime;				// Интервал измерения температур воздуха
unsigned int LoopWriteValueSensorOnSD;		// Логирование текущих данных на SD карту
unsigned int LoopRunChannel;				// Запуск ф-ции управления каналами
unsigned int LoopCalculationDC;
unsigned int LoopOffBluetoothTime;			// Выключение Bluetooth модуля
unsigned int LoopCheckRegistrationGSM;		// Интервал проверки регистрации GSM
unsigned int LoopMaximumTimeResponseGSM;
unsigned int LoopResetTimeGSM;				// Время перезагрузки GSM модуля
unsigned int LoopOutputGPRS;				// Отправка GPRS пакетов на сайт
unsigned int LoopReadInternalTemp;			// Интервал измерения встроеный LM75
unsigned int LoopCheckIntervalGPRS;			// Интервал проверки регистрации GPRS
unsigned int LoopCheckBalanseSIM;			// Проверка баланса SIM карты


boolean RegistrationGSM = false;			// Флаг что отправлена команда для проверки регистрации GSM
int QuantityPacketGPRS;						// Счетчик количества отправок по GPRS


unsigned int T_second;						// Счетчик секунд таймера 
byte TimerTsecond;
byte T_min;									// Счетчик минут таймера 
byte T_hour;								// Счетчик часов таймера 
unsigned int T_day;							// Счетчик дней таймера 

int UpStepValue[QUANTITY_EXEC_MODULES];		// Массив со значениями перемещений исп. модулей для пропорционального управления



struct StructSensor{
	byte NumberSensor;
	float Value;
	float OldValue;
	byte NumberSGB;
	boolean Error;
} WorkValueSensor;

struct StructModule{
	byte Module;							// Номер исполнительного модуля
	byte TypeModule;						// Тип исполнительного модуля
	boolean ModuleFound = false;			// Флаг найден ли рабочий модуль
} WorkExecModule;

struct StructExecModule{
	int StepValue; 
	byte NumberChannel;
	int StepValueStepper; 
	int StepValesPWM; 
	int StepValesPDM; 
	int DigitalLevel; 
	byte Mode;
	boolean State;
} ModuleWorkData;

struct StructWorkChannel{
	byte Status;
	byte Number;
	byte TypeControll;
	int MinValue;
	int MaxValue;
	byte TimerStart_hours;
	byte TimerStart_minute;
	byte TimerStop_hours;
	byte TimerStop_minute;
	byte Timer_delta;
} WorkChannel;


void CleanTimeIntervals(){					// Сброс счетчиков интервалов
	T_second = 0;
	LoopOffBluetoothTime = 0;
	LoopSensorTime = 0;
	LoopWriteValueSensorOnSD = 0;
	LoopLCDLightTime = 0;
	LoopRunChannel = 0;
	LoopCheckRegistrationGSM = 0;
	LoopMaximumTimeResponseGSM = 0;
	LoopOutputGPRS = 0;
	LoopReadInternalTemp = 0;
	LoopCheckIntervalGPRS = 0;
	LoopCheckBalanseSIM = 0;
	for(byte Sensor = 0; Sensor <= QUANTITY_SENSORS; Sensor ++){
		LoopTimeRunCalculateSensor[Sensor] = 0;
	}
}


boolean ArchiveRestoreSaveWordData(byte Type){							// Ф-ция сохранения рабочих значений датчиков и исполнительных модулей после перезагрузки (Type=1 Save, Type=2 Restore)	
	switch(Type){
		case 1:
			Serial.print(F("Сохранение значений рабочих переменных для перезагрузки..."));
		case 2:
			Serial.print(F("\tВосстановление значений рабочих переменных..."));
	}
	
	for(byte Sensor = 0; Sensor < QUANTITY_SENSORS; Sensor ++){			// Проходим по байтам конфигурации всех датчиков
		for(byte SGB = 0; SGB < 3; SGB ++){								// Ищем датчики прикрепленные к группе по байтам конфигурации E_SBG_A_*, E_SBG_B_*, E_SBG_C_*
			switch(Type){
				case 1:
					EEPROM_float_write(E_ArchiveOldValueSensors, OldValueSensors[Sensor][SGB]);
				case2:
					OldValueSensors[Sensor][SGB] = EEPROM_float_read(E_ArchiveOldValueSensors);
			}
		}
	}
	
	for(byte Module = 0; Module < QUANTITY_EXEC_MODULES; Module ++){	
		switch(Type){
			case 1:
				EEPROM.write(E_UpStepValue + Module*2, UpStepValue[Module]);				// Под каждое значение в EEPROM выделяется по 2 байта
			case 2:
				UpStepValue[Module] = EEPROM.read(E_UpStepValue + Module*2);
		}
	}	
	Serial.println(F("OK"));
	return true;
}


void Turning_Backlight_LCD(){
	lcd.backlight();
	lcd.display();
	LightLCDEnable = true;
}


void ViewInit(){
	Serial.print(F("TCCR5B"));									
	TCCR5A = (0<<WGM51)|(0<<WGM50);				
	TCNT5 = 49911;								
	TIFR5 = (1<<TOV5);							
	TIMSK5 |= (1<<TOIE5);						
	TCCR5B |= (1<<CS52)|(0<<CS51)|(1<<CS50);	
}

boolean GSMloaded = false;


void TimeIntervals(){		
	wdt_reset();																		// Сбрасываем watchdog
	static boolean Switch;
	if(EEPROM_int_read(E_LoopLCDLightTime) != 0){										// Если 0, то не выключаем подсветку
		if(LightLCDEnable){																// Если включена подсветка LCD экрана
			if(!Switch){						
				LoopLCDLightTime = T_second;
				Switch = true;
			}
			if(T_second > (LoopLCDLightTime + EEPROM_int_read(E_LoopLCDLightTime))){	// Выключение подсветки экрана
				LoopLCDLightTime = T_second;
				lcd.noBacklight();
				lcd.noDisplay();
				LightLCDEnable = false;
				Switch = false;
			}
		}
	}
	// ==================================== Измерение показаний датчиков ==============================================
	if(EEPROM_int_read(E_LoopSensorTime) != 0){
		if(T_second > (LoopSensorTime + EEPROM_int_read(E_LoopSensorTime))){
			LoopSensorTime = T_second;
			if(OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F(""));
				Serial.println(F("==============================================="));
				Serial.print(F("========= "));
				Serial.print(time.gettime("H:i:s")); Serial.println(F(" === Опрос датчиков ========="));
				Serial.println(F("==============================================="));
			}
			CalculateSensors();												// Измеряем значения датчиков
			if(OUTPUT_LEVEL_UART_SENSOR){
				Serial.println();
				Serial.println(F("========================================================="));
				Serial.println(F("========== Измеренные показания всех датчиков ==========="));
				Serial.println(F("========================================================="));
				wdt_reset();
				ViewValueAllSensors();										// Выводим измеренные показания всех датчиков
			}
		}
	}
	// ============================ Если настроен режим включаем Bluetooth модуля по требованию ============================
	if(EEPROM.read(E_ConfigPowerBluetooth) == 2){
		if(T_second > (LoopOffBluetoothTime + EEPROM_int_read(E_BluetoothTimeOff))){
			LoopOffBluetoothTime = T_second;
			bt_vcc_off();
		}
	}
	// ================== Выводим в консоль напряжение питания и температуру встроенного датчика (LM75A) ===================
	if(T_second > (LoopCalculationDC + 6)){
		LoopCalculationDC = T_second;
		if(OUTPUT_LEVEL_UART_SENSOR){
			Serial.print(F("Напряжение питания: ")); Serial.println(VCC);
			Serial.print(F("Температура контроллера: ")); Serial.println(Ti);
		}
	} 
	// ===================================== Проверка баланса SIM карты (раз в 3 минуты) ===================================
	if(StateGSM.Code_Connect_GSM == GSM_REGISTERED){
		if(T_second > (LoopCheckBalanseSIM + 60)){
			LoopCheckBalanseSIM = T_second;
			send_AT_Command(F("AT+CUSD=1,\"#102#\""), GSM_NO_WAITING_ANSWER, GSM_NO_OUTPUT_TO_SERIAL);
		}	
	}
	// =================================== Если GSM модуль настроен на постоянную работу ===================================
	if(EEPROM.read(E_WORK_SIM800) == ON){
		if(T_second > (LoopOutputGPRS + 60)){
			LoopOutputGPRS = T_second;
			String VCC_Ext = String(VCC, 1);									// Убираем лишние символы после запятой
			Send_GET_request(String	(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogDataWebServer	+ 
									(F("&VCC="))	+ VCC_Ext		        				+
									(F("&Ti="))		+ Ti									+ 
									(F("&S11="))	+ RealValueSensors[SENSOR_1] [VALUE_1]  + 
									(F("&S12="))	+ RealValueSensors[SENSOR_1] [VALUE_2]  + 
									(F("&S13="))	+ RealValueSensors[SENSOR_1] [VALUE_3]  + 
									
									(F("&S21="))	+ RealValueSensors[SENSOR_2] [VALUE_1]  +
									(F("&S22="))	+ RealValueSensors[SENSOR_2] [VALUE_2]  +
									(F("&S23="))	+ RealValueSensors[SENSOR_2] [VALUE_3]  + 
									
									(F("&S31="))	+ RealValueSensors[SENSOR_3] [VALUE_1]  + 
									(F("&S32="))	+ RealValueSensors[SENSOR_3] [VALUE_2]  + 
									(F("&S33="))	+ RealValueSensors[SENSOR_3] [VALUE_3]  + 
									
									(F("&S41="))	+ RealValueSensors[SENSOR_4] [VALUE_1]  + 
									(F("&S42="))	+ RealValueSensors[SENSOR_4] [VALUE_2]  + 
									(F("&S43="))	+ RealValueSensors[SENSOR_4] [VALUE_3]  + 
									
 									(F("&S51="))	+ RealValueSensors[SENSOR_5] [VALUE_1]  + 
 									(F("&S52="))	+ RealValueSensors[SENSOR_5] [VALUE_2]  + 
 									(F("&S53="))	+ RealValueSensors[SENSOR_5] [VALUE_3]  + 
									
									(F("&S61="))	+ RealValueSensors[SENSOR_6] [VALUE_1]  +
									(F("&S62="))	+ RealValueSensors[SENSOR_6] [VALUE_2]  +
									(F("&S63="))	+ RealValueSensors[SENSOR_6] [VALUE_3]  +
																		
									(F("&S71="))	+ RealValueSensors[SENSOR_7] [VALUE_1]  +
									(F("&S72="))	+ RealValueSensors[SENSOR_7] [VALUE_2]  +
									(F("&S73="))	+ RealValueSensors[SENSOR_7] [VALUE_3]  +
									
									(F("&S81="))	+ RealValueSensors[SENSOR_8] [VALUE_1]  +
									(F("&S82="))	+ RealValueSensors[SENSOR_8] [VALUE_2]  +
									(F("&S83="))	+ RealValueSensors[SENSOR_8] [VALUE_3]  +
																	 
									(F("&S91="))	+ RealValueSensors[SENSOR_9] [VALUE_1]  + 
									(F("&S92="))	+ RealValueSensors[SENSOR_9] [VALUE_2]  + 
									(F("&S93="))	+ RealValueSensors[SENSOR_9] [VALUE_3]  + 
									
									(F("&S101="))	+ RealValueSensors[SENSOR_10][VALUE_1]  +
									(F("&S102="))	+ RealValueSensors[SENSOR_10][VALUE_2]  +
									(F("&S102="))	+ RealValueSensors[SENSOR_10][VALUE_3]  +
									
									(F("&S111="))	+ RealValueSensors[SENSOR_11][VALUE_1]  + 
									(F("&S112="))	+ RealValueSensors[SENSOR_11][VALUE_2]  +
									(F("&S113="))	+ RealValueSensors[SENSOR_11][VALUE_3]  +
									
// 									(F("&S121="))	+ RealValueSensors[SENSOR_12][VALUE_1]  +
// 									(F("&S122="))	+ RealValueSensors[SENSOR_12][VALUE_2]  +
// 									(F("&S123="))	+ RealValueSensors[SENSOR_12][VALUE_3]  +
// 									
// 									(F("&S131="))	+ RealValueSensors[SENSOR_13][VALUE_1]  +
// 									(F("&S132="))	+ RealValueSensors[SENSOR_13][VALUE_2]  +
// 									(F("&S133="))	+ RealValueSensors[SENSOR_13][VALUE_3]  +
// 									
// 									(F("&S141="))	+ RealValueSensors[SENSOR_14][VALUE_1]  + 
// 									(F("&S142="))	+ RealValueSensors[SENSOR_14][VALUE_2]  + 
// 									(F("&S143="))	+ RealValueSensors[SENSOR_14][VALUE_3]  + 
// 									
// 									(F("&S151="))	+ RealValueSensors[SENSOR_15][VALUE_1]  +
// 									(F("&S152="))	+ RealValueSensors[SENSOR_15][VALUE_2]  +
// 									(F("&S153="))	+ RealValueSensors[SENSOR_15][VALUE_3]  +
// 									
// 									(F("&S161="))	+ RealValueSensors[SENSOR_16][VALUE_1]  +
// 									(F("&S162="))	+ RealValueSensors[SENSOR_16][VALUE_2]  +
// 									(F("&S163="))	+ RealValueSensors[SENSOR_16][VALUE_3]  +
									
									(F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_VALUE_REQUEST);
		}
	}
}


#define Number_Measurements_VCC 5
boolean LowChargeBattery_SMS = 0;
byte Counter_Number_Measurements = 1;
float GridMeasuredIndications[Number_Measurements_VCC + 1];

void ManagementVCC(){	
	if(Counter_Number_Measurements <= Number_Measurements_VCC){											
		GridMeasuredIndications [Counter_Number_Measurements] = VCC;		// Заполняем массив значением
		Counter_Number_Measurements ++;
	}
	else{																	// и когда заполнили
		Counter_Number_Measurements = 0;									// Обнуляем счетчик
		float MinVCCEEPROM = float(EEPROM.read(E_MinInputVCC)) / 10;
		byte CounterMinValues = 0;
		byte CounterMaxValues = 0;
		
		for (byte i = 1; i <= Number_Measurements_VCC; i ++){				// Проходимся по всем ячейкам массива
			if(GridMeasuredIndications[i] < MinVCCEEPROM){					// Если значение меньше эталонного из EEPROM
				CounterMinValues ++;
			}
			if(GridMeasuredIndications[i] > MinVCCEEPROM){					// Если значение больше эталонного из EEPROM
				CounterMaxValues ++;
			}
		}
		// ======================================== Обработка значений ========================================
		if(CounterMinValues == Number_Measurements_VCC){			// Если все значения в массиве оказались ниже минимального				
			if(OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("Low input VCC"));
			}
			if (LowChargeBattery_SMS == false){						// Если СМС не отправлялось
				switch(EEPROM.read(E_ReactToMinVCC)){
					case REACTION_ERORR_VCC_SEND_SMS:
						Send_SMS(String (F("Controller. Low input VCC: ")) + VCC, GSM_WARNING_SMS);
						break;
					case REACTION_ERORR_VCC_SEND_GET:
						Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Input VCC OK: ")) + VCC + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
						break;
					case REACTION_ERORR_VCC_SEND_SMS_AND_GET:
						Send_SMS(String (F("Controller. Low input VCC: ")) + VCC, GSM_WARNING_SMS);
						Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Low input VCC: ")) + VCC + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
						break;
				}
				LowChargeBattery_SMS = true;						// СМС отправлено
				Low_Input_VCC = true;								// Поднимаем флаг низкого напряжения питания
				// =============================================================================================
						// Выключаем все сервисы настроенные на отключение из-за низкого VCC
				// =============================================================================================
			}
		}
		if(CounterMaxValues == Number_Measurements_VCC){			// Если все значения в массиве были больше E_MinInputVCC (напряжение питания стабилизировалось и  выше минимального)
			if(OUTPUT_LEVEL_UART_SENSOR){
				Serial.println(F("Input VCC OK"));
			}
			if(Low_Input_VCC){										// Если висит флаг низкого напряжения питания
				Low_Input_VCC = false;
				if(LowChargeBattery_SMS){							// Если ранее отправлялась СМС
					switch(EEPROM.read(E_ReactToMinVCC)){
						case REACTION_ERORR_VCC_SEND_SMS:
							Send_SMS(String (F("Controller. Input VCC OK: ")) + VCC, GSM_WARNING_SMS);
							break;
						case REACTION_ERORR_VCC_SEND_GET:
							Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Input VCC OK: ")) + VCC + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
							break;
						case REACTION_ERORR_VCC_SEND_SMS_AND_GET:
							Send_SMS(String (F("Controller. Input VCC OK: ")) + VCC, GSM_WARNING_SMS);
							Send_GET_request(String(F("AT+HTTPPARA=\"URL\",\"")) + Link_LogWebServer + (F("&Log=")) + (F("Input VCC OK: ")) + VCC + (F("\"")), GSM_WAITING_ANSWER, GSM_OUTPUT_TO_SERIAL, GET_LOG_REQUEST);
							break;
					}
					LowChargeBattery_SMS = false;
				}
			}
		}
		
		// =================================== Затираем рабочий массив и счетчик ===================================
		for(byte i = 1; i < Number_Measurements_VCC; i ++){
			GridMeasuredIndications[i] = 0;
		}
	}
}


// ============================== Поиск измеренных значений датчика по привязке в группе ==============================
void Read_Value_Sensor(byte _NumberChannel){								
	WorkValueSensor.NumberSensor = 0;
	WorkValueSensor.NumberSGB = 0;
	WorkValueSensor.OldValue = 0;
	WorkValueSensor.Value = 0;
	for(byte Sensor = 1; Sensor <= QUANTITY_SENSORS; Sensor ++){					// Проходим по байтам конфигурации всех датчиков
		if(EEPROM.read(E_StatusSensor + Sensor) == 1){								// Если датчик включен
			for(byte SGB = 0; SGB < 3; SGB ++){										// Ищем датчики прикрепленные к группе по байтам конфигурации E_SBG_A_*, E_SBG_B_*, E_SBG_C_*
				if(EEPROM.read(E_SBG + ((Sensor) * 3) + SGB) == _NumberChannel){	// если нашли
					WorkValueSensor.NumberSGB = SGB;								// Сохраняем номер SGB
					WorkValueSensor.NumberSensor = Sensor;							// Сохраняем номер датчика		
					if(SensorsError[Sensor][SGB] == 0){								// если не висит флаг ошибочности значений						
						WorkValueSensor.Error = false;								// На всякий случай снимаем ошибку показаний датчика
						WorkValueSensor.Value = RealValueSensors[Sensor][SGB];		// Присваиваем переменной измеренное показание датчика
						WorkValueSensor.OldValue = OldValueSensors[Sensor][SGB];	// Получаем старое показание датчика
						return;
					}
					WorkValueSensor.Error = true;					// если висит, то возвращаем ошибку что данные не валидны
				}
			}
		}
		else{
			WorkValueSensor.Error = true;							// если датчик выключен, то возвращаем ошибку что выключен
		}
	}
}


void FindExecModule(byte _Module, byte _NumberChannel){				// Поиск исполнительного модуля привязанного к группе
	WorkExecModule.ModuleFound = false;								// Перед работой трем все значения структуры
	WorkExecModule.Module = 0;										// --//--//--//--
	WorkExecModule.TypeModule = 0;									// --//--//--//--
	if(EEPROM.read(E_BindExecModile + _Module) == _NumberChannel){	// Ищем модуль привязанный к NumberChannel и если нашли
		if(EEPROM.read(E_StatusModule + _Module) == 1){				// И если модуль включен
			WorkExecModule.ModuleFound = true;						// Поднимаем флаг что модуль найден (нужно для ф-ции управления группой)
			WorkExecModule.Module = _Module;						// Записываем номер найденного модуля
			WorkExecModule.TypeModule = EEPROM.read(E_TypeExecModule + _Module);		// И тип управления модулем
		}
	}
}


void ExecModules(byte _NumberChannel, byte _Mode){
	byte _MinValueChannel = EEPROM.read(E_MinValueChannel + _NumberChannel);
	byte _MaxValueChannel = EEPROM.read(E_MaxValueChannel + _NumberChannel);
	int _StepValue;
	int _StepValesPWM;				// Количество шагов для PWM управления
	int _StepValesPDM;				// Количество шагов для серво привода
	int _StepValueStepper;			// Количество шагов передаваемое исполнительному модулю.
	byte _DigitalLevel;				// В режиме "Digital Port" просто включает порт
	byte _QuantityDigital;
	switch(WorkExecModule.TypeModule){										// По типу модуля определяем нужное кол-во шагов для перемешения
		case 1:																// --- Шаговый мотор
			if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
				Serial.print(F("\t\t\t\t\t\t...Executive module: ")); Serial.print(WorkExecModule.Module); Serial.println(F(". It is ready as Stepper motor "));
			}
			if(_Mode == 1){													// Если канал в режиме пропорционального управления
				if(WorkValueSensor.Value != 0){
					_StepValue = map(WorkValueSensor.Value, _MinValueChannel, _MaxValueChannel, 0, EEPROM.read(E_MaxLimitRotation + WorkExecModule.Module));
				}
				else _StepValue = 0;
			}
			else{															// Если нет, то работаем в режиме ключа, вкл\выкл мотор на максимум
				if(WorkValueSensor.Value > 0){								// 
					_StepValue = EEPROM.read(E_MaxLimitRotation + WorkExecModule.Module);	// Открываем на максимум
				}
				else _StepValue = 0;										// Или спускаем мотор в "нули"
			}				
			_StepValueStepper = _StepValue - UpStepValue[WorkExecModule.Module - 1];
			RunStepperMotor(WorkExecModule.Module, _StepValueStepper, 0);	// Запускаем шаговый мотор
			UpStepValue[WorkExecModule.Module - 1] = _StepValue;			// Сохраняем количество шагов для дальшейшего использования
			break;
		case 2:																// --- ШИМ
			if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
				Serial.print(F("\t\t\t\t\t\t...Executive module: ")); Serial.print(WorkExecModule.Module); Serial.println(F(". It is ready as PWM"));
				//Serial.print(F("\t\t\t\t\t\t...Executive module: ") + String(WorkExecModule.Module) + F(". It is ready as PWM"));
			}
			if(AllowPWMport(EEPROM.read(E_PortExecModule + WorkExecModule.Module))){	// Если порты настроены на работу с PWM пинами
				if(_Mode == 1){
					if(WorkValueSensor.Value != 0){							// Костыль, почему то при значении ValueSensor.Value < MinValueChannel ШИМ не уходит в нули
						_StepValesPWM = map(WorkValueSensor.Value, _MinValueChannel, _MaxValueChannel, 0, 255);
					}
					else _StepValesPWM = 0;									// Отправляю в нули принудительно
				}
				else{
					if(WorkValueSensor.Value > 0){
						_StepValesPWM = 255;
					}
					else _StepValesPWM = 0;
				}
				RunPWMonPort(EEPROM.read(E_PortExecModule + WorkExecModule.Module), _StepValesPWM);		// Запускаем исполнящую ф-цию
			}
			else{
				if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
					Serial.println(F("\t\t\t\t\t\t...The port isn't configured for work with PWM"));
				}
			}
			break;
		case 3:															// --- Серво мотор
			if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
				Serial.print(F("\t\t\t\t\t\t...Executive module: ")); Serial.print(WorkExecModule.Module); Serial.println(F(". It is ready as Servo"));
			}
			if(AllowServoPort(EEPROM.read(E_PortExecModule + WorkExecModule.Module))){	// Если порты в числе разрешенных для работы с сервой
				switch(_Mode){
					case 1:												// Канал в режиме пропорционального управления
						if(WorkValueSensor.Value != 0){					// Костыль, почему то при значении ValueSensor.Value < MinValueChannel серва не ухоит в нули
							_StepValesPDM = map(WorkValueSensor.Value, _MinValueChannel, _MaxValueChannel, 0, EEPROM.read(E_MaxLimitRotation + WorkExecModule.Module));
						}
						else _StepValesPDM = 0;							// Отправляю в нули принудительно
						break;
					case 2:
						if(WorkValueSensor.Value > 0){
							_StepValesPDM = EEPROM.read(E_MaxLimitRotation + WorkExecModule.Module);	
						}
						else _StepValesPDM = 0;
						break;
					case 3:												// Канал не в режиме пропорционального управления
						if(WorkValueSensor.Value > 0){
							_StepValesPDM = 0;
						}
						else _StepValesPDM = EEPROM.read(E_MaxLimitRotation + WorkExecModule.Module);
						break;
				}
				RunServoMotor(WorkExecModule.Module, _StepValesPDM);
			}
			break;
		case 4:													// --- Digitall port
			if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
				Serial.print(F("\t\t\t\t\t\t...Executive module: ")); Serial.print(WorkExecModule.Module); Serial.println(F(". It is ready as Digital port"));
			}
			if(WorkValueSensor.Value > 0){
				_QuantityDigital = 1;
			}
			else{
				_QuantityDigital = 0;
			}
			if (_Mode == 2 || _Mode == 3 || _Mode == 4 || _Mode == 5){		// Если каналами не в режиме пропорционального управления
				if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
					Serial.print(F("\t\t\t\t\t\t\t...port "));
					DigitalPort(EEPROM.read(E_PortExecModule + WorkExecModule.Module), DIGITAL_PORT_OFF, DIGITAL_PORT_RETURN_NAME_PORT, NO_LOG_TO_UART);	// Выводим название порта в консоль
				}
				DigitalPort(EEPROM.read(E_PortExecModule + WorkExecModule.Module), _QuantityDigital, DIGITAL_PORT_SWITCH_PORT, NO_LOG_TO_UART);	// Запускаем управление портом
				StateDigitalPorts[E_PortExecModule + WorkExecModule.Module, _QuantityDigital];									// Сохраняем состояние порта в массив
				if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
					if(_QuantityDigital == 1){
						Serial.println(F(": HIGH"));
					}
					else{
						Serial.println(F(": LOW"));
					}
				}
			} 
			else{
				if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
					Serial.println(F("\t\t\t\t\t\t\t...Management of digital port does not work in the mode in proportion of management"));
				}
			}
			break;
		case 5:											// Коллекторный мотор
			break;
		case 6:											// GSM модуль
			break;
		default:
			if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
				Serial.println(F("\t\t\t\t\t...isn't present the module is configured"));
			}
	}
}


void WorkThermostatMode(byte _Mode){								// Рабочая ф-ция для режима термостата
/*
	NumberChannel - номер управляемой группы
	Mode - режим управления группой
*/
	boolean _State = true;			// Включать или нет сам испольнительный механизм. По default стоит "включать"
	int _StepValue;
	static float _OldValue[QUANTITY_SENSORS];
	boolean _ViewStateMode_1 = false;
	boolean _ViewStateMode_2 = false;
	boolean _ViewStateMode_3 = false;
	boolean _ViewStateMode_4 = false;
	boolean _ModuleFound = false;
	if(OUTPUT_LEVEL_UART_CHANNEL){
		//Serial.print(F("\t\t\tManagement of channel is started in Termostat mode (")); Serial.print(Mode); Serial.println(F(")"));
		switch(_Mode){
			case 1:
				Serial.println(F("\t\t\t\tBoundary values of channel: "));
				Serial.print(F("\t\t\t\t\t...MinValueChannel: ")); Serial.println(WorkChannel.MinValue);
				Serial.print(F("\t\t\t\t\t...MaxValueChannel: ")); Serial.println(WorkChannel.MaxValue);
			break;
		}
	}
	if(!WorkValueSensor.Error){												// Если не висит ошибка показаний датчика 
		if(OUTPUT_LEVEL_UART_CHANNEL){
			Serial.print(F("\t\t\t\tValue sensor: ")); Serial.println(WorkValueSensor.Value);
			Serial.println(F("\t\t\t\t...Manage channel---> "));
		}
		for(byte Module = 1; Module <= QUANTITY_EXEC_MODULES; Module ++){	// Проходим по всем модулям
			FindExecModule(Module, WorkChannel.Number);						// Ф-ция поиска рабочего модуля
			if(WorkExecModule.ModuleFound){									// Если нашли модуль
				_ModuleFound = true;										// Поднимаем флаг что модуль найден
				switch(_Mode){
					// ========================================================================================
					case 1:															// Пропорциональный режим управления
						if (WorkValueSensor.Value >= WorkChannel.MinValue && WorkValueSensor.Value <= WorkChannel.MaxValue){	// Значение сенсора в рабочем диапазоне
							if(OUTPUT_LEVEL_UART_CHANNEL){
								if(!_ViewStateMode_1){												// Флаг чтобы не выводить два раза
									Serial.print(F("\t\t\t\t\t...MinValueChannel (")); Serial.print(WorkChannel.MinValue); 
										Serial.print(F(") < ")); Serial.print(F("Value (")); Serial.print(WorkValueSensor.Value); 
										Serial.print(F(") < MaxValueChannel (")); Serial.print(WorkChannel.MaxValue); Serial.println(F(")"));
									if(WorkValueSensor.Value < _OldValue[WorkChannel.Number - 1]){			// Показания идут вниз
										Serial.println(F("\t\t\t\t\t...The value goes down "));
									}
									else if(WorkValueSensor.Value > _OldValue[WorkChannel.Number - 1]){		// Показания идут вверх
										Serial.println(F("\t\t\t\t\t...The value goes up "));
									}
									else if(WorkValueSensor.Value == _OldValue[WorkChannel.Number - 1]){		// Показания стоят на месте (ничего не делаем)
										Serial.println(F("\t\t\t\t\t...The value stands still "));
									}
								}
							}
							_OldValue[WorkChannel.Number - 1] = WorkValueSensor.Value;
						}					
						else if (WorkValueSensor.Value > WorkChannel.MaxValue){					// Значение сенсора выше максимального
							if(OUTPUT_LEVEL_UART_CHANNEL){
								if(!_ViewStateMode_1){
									Serial.print(F("\t\t\t\t\t...Value (")); Serial.print(WorkValueSensor.Value); Serial.print(F(") > MaxValueChannel ("));  Serial.print(EEPROM.read(E_MaxValueChannel + WorkChannel.Number)); Serial.println(F(")"));
								}
							}
							WorkValueSensor.Value = WorkChannel.MaxValue;						// Заставляем контроллер "открыть" исполняемый модуль
						}
						else if(WorkValueSensor.Value < WorkChannel.MinValue){					// Значение сенсора меньше минимального
							if(OUTPUT_LEVEL_UART_CHANNEL){
								if(!_ViewStateMode_1){
									Serial.print(F("\t\t\t\t\t...Value (")); Serial.print(WorkValueSensor.Value); Serial.print(F(") < MinValueChannel ("));  Serial.print(WorkChannel.MinValue); Serial.println(F(")"));
								}
							}
							WorkValueSensor.Value = 0;										// "Закрываем" исполнительный модуль
						}
						_ViewStateMode_1 = true;											// Поднимаем флаг чтобы повторно не выводить данные
						ExecModules(WorkChannel.Number, _Mode);								// Запускаем управление исполнительными модулями. Данные берем из структур ValueSensor и ExecModule
						break;
					// ========================================================================================
					case 2:																	// Управление при значении датчика > E_MinValueChannel
						Read_Value_Sensor(WorkChannel.Number);								// Получаем показание датчика
						_StepValue = EEPROM.read(E_MinValueChannel + WorkChannel.Number);
						if(WorkExecModule.ModuleFound){	
							if(WorkValueSensor.Value > _StepValue){	
								if(OUTPUT_LEVEL_UART_CHANNEL){
									if(!_ViewStateMode_2){
										Serial.print(F("\t\t\t\t\t...RealValue (")); Serial.print(WorkValueSensor.Value); Serial.print(F(") > ")); Serial.print(F("MinValueChannel (")); Serial.print(_StepValue); Serial.println(F(")"));
									}
								}
							WorkValueSensor.Value = _StepValue;						// Заставляем контроллер "открыть" исполняемый модуль
							}
							else{
								if(OUTPUT_LEVEL_UART_CHANNEL){
									if(!_ViewStateMode_2){
										Serial.print(F("\t\t\t\t\t...RealValue (")); Serial.print(WorkValueSensor.Value); Serial.print(F(") < ")); Serial.print(F("MinValueChannel (")); Serial.print(_StepValue); Serial.println(F(")"));
									}
								}
								WorkValueSensor.Value = -_StepValue;				// Заставляем контроллер "открыть" исполняемый модуль
							}
							ExecModules(WorkChannel.Number, _Mode);					// Запускаем управление исполнительными модулями. Данные берем из структур ValueSensor и ExecModule	
						}
						_ViewStateMode_2 = true;									// Поднимаем флаг чтобы повторно не выводить данные
						break;
					// ========================================================================================
					case 3:															// Режим управления при значении датчика меньше E_MinValueChannel (например включаем обогрев)
						_StepValue = EEPROM.read(E_MinValueChannel + WorkChannel.Number);
						if(WorkValueSensor.Value < _StepValue){						// Показания датчика ниже настроек группы (запускаем исполнительные модули)
							if(OUTPUT_LEVEL_UART_CHANNEL){
								if(!_ViewStateMode_3){
									Serial.print(F("\t\t\t\t\t...RealValue (")); Serial.print(WorkValueSensor.Value); Serial.print(F(") < ")); Serial.print(F("MinValueChannel (")); Serial.print(_StepValue); Serial.println(F(")"));	
								}
							}
							WorkValueSensor.Value = _StepValue;						// Заставляем контроллер "открыть" исполняемый модуль
						}
						else{
							if(OUTPUT_LEVEL_UART_CHANNEL){
								if(!_ViewStateMode_3){
									Serial.print(F("\t\t\t\t...RealValue (")); Serial.print(WorkValueSensor.Value); Serial.print(F(") > ")); Serial.print(F("MinValueChannel (")); Serial.print(_StepValue); Serial.println(F(")"));
								}
							}
							WorkValueSensor.Value = -_StepValue;					// "Закрываем" исполнительный модуль	
						}
						_ViewStateMode_3 = true;										// Поднимаем флаг чтобы повторно не выводить данные
						ExecModules(WorkChannel.Number, _Mode);						// Запускаем управление исполнительными модулями. Данные берем из структур ValueSensor и ExecModule
						break;
				}
			}
			if((Module + 1) == QUANTITY_EXEC_MODULES){								// Если прошли по всем модулям
				if(!_ModuleFound){													// И не нашли ни одного нужного
					if(OUTPUT_LEVEL_UART_CHANNEL){
						Serial.println(F("\t\t\t\t\t...Management of channel is stopped."));
						Serial.println(F("\t\t\t\t\t...Exec modules is not found"));
					}
					_ModuleFound = false;
				}
			}
		}
		// ===================== Блок СМС уведомлений =========================
		if (WorkValueSensor.Value < EEPROM.read(E_MinValueToReact + (WorkChannel.Number - 1))){
			if(EEPROM.read(E_AllowReactToMinValue + (WorkChannel.Number - 1)) == 1){											// Если настроено на отправку этого уведомления
				if(EEPROM.read(E_SendingSMSminValue + (WorkChannel.Number - 1)) == 1){										// Если СМС не отправлялось
					Send_SMS(String(F("The value of channel ")) + (WorkChannel.Number - 1) + (F(" is lower than minimum")), GSM_ERROR_SMS);  
					EEPROM.write(E_SendingSMSmaxValue + (WorkChannel.Number - 1), 1);
				}
			}
		}
		if (WorkValueSensor.Value > EEPROM.read(E_MaxValueToReact + (WorkChannel.Number - 1))){
			if(EEPROM.read(E_AllowReactToMaxValue + (WorkChannel.Number - 1)) == 1){											// Если настроено на отправку этого уведомления
				if(EEPROM.read(E_SendingSMSmaxValue + (WorkChannel.Number - 1)) == 1){										// Если СМС не отправлялось
					Send_SMS(String(F("The value of channel ")) + (WorkChannel.Number - 1) + (F(" is higher than maximum")), GSM_ERROR_SMS);  
					EEPROM.write(E_SendingSMSmaxValue + (WorkChannel.Number - 1), 1);
				}
			}
		}
		// ===================================================================
	}
	else{								// Если висит ошибка данных датчика или он отключен
		if(OUTPUT_LEVEL_UART_CHANNEL){
			Serial.println(F("\t\t\t\t...Management of channel is stopped. There are no data of the sensor"));
		}
	}
}


 
void WorkTimerMode(byte _Mode){										// Рабочая ф-ция для режима таймера
	boolean ModuleFound = false;
	if(OUTPUT_LEVEL_UART_CHANNEL){
		Serial.print(F("Channel ")); Serial.print(WorkChannel.Number); Serial.println(F(" is started"));
		Serial.println(F("Management of channel is started in timer mode"));
		switch(_Mode){
			case 4:
				Serial.print(F("Time start: ")); Serial.print(WorkChannel.TimerStart_hours); Serial.print(F(":")); Serial.println(WorkChannel.TimerStart_minute);
				Serial.print(F("Time stop: ")); Serial.print(WorkChannel.TimerStop_hours); Serial.print(F(":")); Serial.println(WorkChannel.TimerStop_minute);
				break;
			case 5:
				Serial.print(F("Time start: ")); Serial.print(WorkChannel.TimerStart_hours); Serial.print(F(":")); Serial.println(WorkChannel.TimerStart_minute);
				Serial.print(F("Stop interval: ")); Serial.println(WorkChannel.Timer_delta); 
				break;
			case 6:
				Serial.print(F("Interval of turning on of the module: ")); Serial.println(WorkChannel.Timer_delta);
				break;
		}
	}
	for(byte Module = 1; Module <= QUANTITY_EXEC_MODULES; Module ++){	// Проходим по всем модулям
		FindExecModule(Module, WorkChannel.Number);						// Ищем модуль привязанный к группе
		if(WorkExecModule.ModuleFound){									// Если нашли модуль
			ModuleFound = true;											// Поднимаем флаг что модуль найден
			switch(_Mode){												// Определяем тип управления модулем
				case 4:
					if(time.Hours == EEPROM.read(E_TimerStart_hours + WorkChannel.Number)){
						if(time.minutes == EEPROM.read(E_TimerStart_minute + WorkChannel.Number)){
							WorkValueSensor.Value = WorkChannel.MaxValue;				// "Открываем" исполняемый модуль
							ExecModules(WorkChannel.Number, _Mode);
						}
					}
					else{
						if(time.minutes == EEPROM.read(E_TimerStart_minute + WorkChannel.Number)){
							WorkValueSensor.Value = WorkChannel.MaxValue;				// "Открываем" исполняемый модуль
							ExecModules(WorkChannel.Number, _Mode);
						}
					}
					if(time.Hours == EEPROM.read(E_TimerStop_hours + WorkChannel.Number)){
						if(time.minutes == EEPROM.read(E_TimerStop_minute + WorkChannel.Number)){
							WorkValueSensor.Value = 0;										// "Закрываем" исполнительный модуль
							ExecModules(WorkChannel.Number, _Mode);
						}
					}
					else{
						if(time.minutes == EEPROM.read(E_TimerStart_minute + WorkChannel.Number)){
							WorkValueSensor.Value = 0;										// "Закрываем" исполнительный модуль
							ExecModules(WorkChannel.Number, _Mode);
						}
					}
					break;
				case 5:
					if(time.Hours == EEPROM.read(E_TimerStart_hours + WorkChannel.Number)){
						if(time.minutes == EEPROM.read(E_TimerStart_minute + WorkChannel.Number)){
							WorkValueSensor.Value = WorkChannel.MaxValue;				// "Открываем" исполняемый модуль
							ExecModules(WorkChannel.Number, _Mode);
						}
					}
					break;
				case 6:
					break;
			}
		}
		if(Module == QUANTITY_EXEC_MODULES){								// Если прошли по всем модулям
			if(!ModuleFound){												// И не нашли ни одного нужного
				if(OUTPUT_LEVEL_UART_CHANNEL){
					Serial.println(F("\t\t...Управление каналом остановлено. Исполнительный модуль не найден"/*"\t\t...Management of channel is stopped. Modules is not found"*/));
				}
				ModuleFound = false;
			}
		}
	}
}


boolean ValidDataSensors(byte _NumberChannel){							// Ф-ция для получения и проверки значения датчика 
	Read_Value_Sensor(_NumberChannel);									// Получаем показание датчика
	if(!WorkValueSensor.Error){											// Если нет ошибок показаний
		if(WorkValueSensor.Value != WorkValueSensor.OldValue){			// Если показание датчика привязанного к группе изменилось
			OldValueSensors[WorkValueSensor.NumberSensor][WorkValueSensor.NumberSGB] = WorkValueSensor.Value; // Сохраняем значение
			return true;
		}
		else{
			if(OUTPUT_LEVEL_UART_CHANNEL){
				Serial.println(F("\t\t\t\t...Показание датчика не изменилось"/*"\t\t\t\t...The value of the sensor did not change"*/));
			}
			return false;
		}
	}
	else{ 
		if(OUTPUT_LEVEL_UART_CHANNEL){
			Serial.println(F("\t\t\t\t...Ошибка показания датчика"/*"\t\t\t\t...The value of the sensor is error"*/));
		}
		return false;
	}
}


void TermostatFunc(byte _NumberChannel){				// Запуск управления каналами в режиме термостата
	WorkChannel.Number = _NumberChannel;				// Сохраняем номер группы
	WorkChannel.Status = 0;
	WorkChannel.MinValue = 0;
	WorkChannel.MaxValue = 0;
	WorkChannel.TypeControll = EEPROM.read(E_Controll_Channel + _NumberChannel);				// Сохраняем тип управления каналом
	if(OUTPUT_LEVEL_UART_CHANNEL){
		Serial.print(F("\t\t\tManagement of channel is started in Termostat mode (")); Serial.print(WorkChannel.TypeControll); Serial.println(F(")"));
	}
	if(ValidDataSensors(_NumberChannel)){				// Проверяем изменилось ли измеренное значение датчика
		switch(WorkChannel.TypeControll){		
			case 1:										// Пропорциональное управление по max\min
				WorkChannel.MinValue = EEPROM.read(E_MinValueChannel + _NumberChannel);			// Сохраняем минимальное значение датчика в группе
				WorkChannel.MaxValue = EEPROM.read(E_MaxValueChannel + _NumberChannel);			// Сохраняем максимальное значение датчика в группе
				break;
			case 2:										// PID управление
				break;
			case 3:										// Управление при превышениий значения E_MinValueChannel
				WorkChannel.MinValue = EEPROM.read(E_MinValueChannel + _NumberChannel);			// Сохраняем минимальное значение датчика в группе
				break;
			case 4:										// Управление при значении датчика меньше E_MinValueChannel
				WorkChannel.MinValue = EEPROM.read(E_MinValueChannel + _NumberChannel);			// Сохраняем минимальное значение датчика в группе
				break;
			default:
				if(OUTPUT_LEVEL_UART_CHANNEL){
					Serial.print(F("\t\t\tManagement of channel '")); Serial.print(_NumberChannel); Serial.println(F("' is not configured"));
				}
		}
		WorkThermostatMode(WorkChannel.TypeControll);	// Запускаем исполняемую ф-цию
	}
}



void TimerFunc(/*byte _NumberChannel*/){						// Запуск управления каналами в режиме таймера
	WorkChannel.Number = 0;	
	WorkChannel.Status = 0;	
	WorkChannel.TypeControll = 0;
	WorkChannel.TimerStart_hours = 0;
	WorkChannel.TimerStart_minute = 0;
	WorkChannel.TimerStop_hours = 0;
	WorkChannel.TimerStop_minute = 0;
	WorkChannel.Timer_delta = 0;
	if(OUTPUT_LEVEL_UART_CHANNEL){
		Serial.println(F("=================================================="));
		Serial.println(F(" =========== Control unit of channel ============ "));
		Serial.println(F("  ==============================================  "));
	}	
	for(byte NumberChannel = 1; NumberChannel <= QUANTITY_CHANNEL; NumberChannel ++){		// Проходим по всем каналам
		if(EEPROM.read(E_StatusChannel + NumberChannel) == 1){								// Если канал включен
			if(OUTPUT_LEVEL_UART_CHANNEL){
				Serial.print(F("----- Channel ")); Serial.print(NumberChannel); Serial.println(F(" is started -----"));
			}
			WorkChannel.Number = NumberChannel;												// Сохраняем номер группы
			WorkChannel.TypeControll = EEPROM.read(E_Controll_Channel + NumberChannel);		// Сохраняем тип управления группой
			switch(WorkChannel.TypeControll){
				case 4:											// Работа по таймеру. Включение по E_TimeRun и выключение по E_TimeStop
					WorkChannel.TimerStart_hours = EEPROM.read(E_TimerStart_hours + NumberChannel);
					WorkChannel.TimerStart_minute = EEPROM.read(E_TimerStart_minute + NumberChannel);
					WorkChannel.TimerStop_hours = EEPROM.read(E_TimerStop_hours + NumberChannel);
					WorkChannel.TimerStop_minute = EEPROM.read(E_TimerStop_minute + NumberChannel);
					WorkTimerMode(4);
					break;
				case 5:											// Работа по таймеру. Включение по таймеру, выключение по длительности E_Delta
					WorkChannel.TimerStart_hours = EEPROM.read(E_TimerStart_hours + NumberChannel);
					WorkChannel.TimerStart_minute = EEPROM.read(E_TimerStart_minute + NumberChannel);
					WorkChannel.Timer_delta = EEPROM.read(E_Timer_delta + NumberChannel);
					WorkTimerMode(5);
					break;
				case 6:											// Работа по таймеру. Включение и выключение по интервалу E_Delta
					WorkChannel.Timer_delta = EEPROM.read(E_Timer_delta + NumberChannel);
					WorkTimerMode(6);
					break;
				default:
					if(OUTPUT_LEVEL_UART_CHANNEL){
						Serial.print(F("Management of channel '")); Serial.print(NumberChannel); Serial.println(F("' is not configured"));
					}
			}
		}
	}
	if(OUTPUT_LEVEL_UART_CHANNEL_AND_SETUP){
		Serial.println(F("============ Close management channel's ============"));
		Serial.println(F(""));
	}
}