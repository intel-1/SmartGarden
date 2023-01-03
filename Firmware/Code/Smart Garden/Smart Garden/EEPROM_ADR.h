/*
 *  --------------- Адреса ячеек в EEPROM ------------------
 */

#ifndef EEPROM_ADR_H
#define EEPROM_ADR_H

//void EEPROM_int_write(int addr, int val);	// Запись в EEPROM 
int EEPROM_int_read(int addr);				// Чтение int из EEPROM   

float EEPROM_float_read(int addr);
void EEPROM_float_write(int addr, float num);

unsigned long EEPROM_ulong_read(int addr);
void EEPROM_ulong_write(int addr, unsigned long num);

long EEPROM_long_read(int addr);
void EEPROM_long_write(int addr, long num);

unsigned int EEPROM_uint_read(int addr);
void EEPROM_uint_write(int addr, unsigned int num);

String EEPROM_str_read(int addr);
void EEPROM_str_write(int addr, byte size, String text);

void cleanEEPROM(byte ThatClean);			/* Очистка всех ошибок в EEPROM: 1	- Очистка всех ошибок в EEPROM
																			 2	- Очистка настроек датчиков
																			 3	- Очистка настроек групп
																			 4	- Очистка настрек исполнительных модулей
																			 5	- Полная очистка EEPROM */
void View_DataEEPROM();						// Вывод в Serial ошибок из EEPROM

 
/*
 *  --------------- Адреса ячеек в EEPROM ------------------
 */
 
#include <EEPROM.h>

#define E_NameController 1				// Имя контроллера (максимально 19 байт)

// ===============================================
// ====== Временные интервалы запуска ф-ций ======
// ===============================================
#define E_LoopLCDLightTime 20			// (тип int) Выключение подсветки экрана
#define E_LoopSensorTime 22				// (тип int) Измерение показаний датчиков
#define E_LoopWriteValueSensorOnSD 24	// (тип int) Запись измеренных значений всех датчиков на SD карту
#define E_LoopChannelRun 26				// (тип int) Управление группами
#define E_LoopOutputGPRS 28				// (тип int) Отправка GET запроса со значениями датчиков


// ========================================
// ======= Конфигурация контроллера =======
// ========================================
#define E_MaxQuantityErrorToEeprom 60			// Максимальное количество ошибок после которого они перестают писаться в EEPROM
#define E_SentSMSorStartController 61			// Отправлять или нет СМС о старте контроллера (Да\Нет, 1\0)

#define E_ControllVCC 64						// Управлять или нет напряжением питания
#define E_MinInputVCC 66						// (тип int) Минимальное напряжение питания
#define E_ReactToMinVCC	68						// Реакция на понижение напряжения питания

#define E_STATE_TERMOSTAT_INT_LM75 69			// Использовать или нет термостат встроенного датчика температуры LM75A
#define E_Mode_OS_INT_LM75 70					// Режим работы встроенное выходного порта
#define E_INT_LM75_TOS 71						// Значение бита Tos встроенного LM75
#define E_INT_LM75_THYST 72						// Значение бита Thyst встроенного LM75

#define E_ConfigPowerBluetooth 73				// Режим подачи питания на Bluetooth модуль
#define E_BluetoothTimeOff 74					// (тип int) Время отключения модуля при бездействии (сек). Работает только если E_ConfigPowerBluetooth выставлен в "2" 

#define E_LOGING_TO_SERIAL 77					// Уровень логирования в Serial (0 - ничего, 1 - только ошибки, 2 - только данные, 3 - все)
#define E_ENABLE_LOGING_TO_SD 78				// Глобальная настройка. Писать на SD карту или нет.
#define E_ModeController 79						// Ручной\Автоматический\Конфигурирования !!!!!!!! Не понятно нужно или нет !!!!!!!!

#define E_WORK_SIM800 80						// Режим питания SIM800 (0 - всегда выключен, 1 - всегда включен, 2 - включать по требованию)
#define E_GSM_STATE_MIN_VCC 81					// Что делать с GSM модулем есть питание контроллера ниже минимального (0 - выключен, 1 - включен)
#define E_IntervalCheckRegistrationGSM 82		// Интервал проверки регистрации GSM (сек)(максимально 250). Если стоит "0", то проверка не осуществляется
#define E_MaximumTimeResponseGSM 85				// Максимальное время ожидания ответа от модуля
#define E_ResetTimeGSM 86						// Время перезагрузки GSM модуля
#define E_RebutingFromGSM 87					// Флаг перезагрузки. Поднимается для отправки уведомления по СМС
#define E_AllowGPRS 88							// Разрешение работы GPRS (0 - отключено, 1 - включено)




// ================================================
// ======= Настройки исполнительных модулей =======		
// ================================================
#define E_TypeExecModule 110						// Тип модуля (1 - шаговый мотор, 2 - ШИМ, 3 - серво мотор, 4 - цифровой пин)		(16 значений по одному байту)
#define E_PortExecModule 130						// Адреса портов управления модулями												(16 значений по одному байту)
#define E_StepperMotor_setMaxSpeed 150				// (тип int) Максимальная скорость вращения ротора двигателя (шагов/секунду)		(16 значений по два байта)
#define E_StepperMotor_setAcceleration 190			// (тип int) Ускорение мотора (шагов/секунду^2)										(16 значений по два байта)
#define E_StepperMotor_Step 230						// (тип int) Кол-во оборотов на 1см подъема,										(16 значений по два байта)
													// например шпилька М8, шаг винта 1,25, 8 об\см, 200 шагов на оборот, 1600 об\см	(16 значений по два байта)
#define E_BindExecModile 270						// Привязка модуля к группе															(16 значений по одному байту)
#define E_Servo_MinImp 310							// (тип int) Серво привод. Величина импульса для поворота в 0*						(16 значений по два байта)
#define E_Servo_MaxImp 350							// (тип int) Серво привод. Величина импульса для поворота в 180*					(16 значений по два байта)
#define E_StatusModule 390							// Статус модуля (вкл\выкл)															(16 значений по одному байту)
#define E_MaxLimitRotation 410						// Максимальная величина перемещения модуля											(16 значений по одному байту)
#define E_ManualModeModule 430						// Стандартный или ручной режим работы модуля										(16 значений по одному байту)
#define E_LowSwitchPortModule 450					// Адрес концевика DOWN	(0 - отсутствует)											(16 значений по одному байту)
#define E_HighSwitchPortModule 470					// Адрес концевика UP (0 - отсутствует)												(16 значений по одному байту)
#define E_TypeHighSwitchModule 490					// Тип концевика UP: 																(16 значений по одному байту)
#define E_StepperMotor_DividerStep 510				// Делитель шага Stepper драйвера (Нужент для режима шагового мотора)


// ===============================================
// ====== Конфигурация каналов управления ========
// ===============================================
#define E_StatusChannel 700			// Состояние группы (включен\выключен)
#define E_Controll_Channel 720		// Параметры управление группой
#define E_MinValueChannel 740		// Минимальное значение паказаний датчика для срабатывания модуля	(16 значений по одному байту)(суммарно 16)
#define E_MaxValueChannel 760		// Максимальные значение. Применяется в режиме управления (1, 2, 3)	(16 значений по одному байту)(суммарно 16)
#define E_TimerStart_hours 780		// Время запуска модуля (часы). Режим управления (4 и 5)			(16 значений по одному байту)(суммарно 16)
#define E_TimerStart_minute 800		// Время запуска модуля (минуты). Режим управления (4 и 5)			(16 значений по одному байту)(суммарно 16)
#define E_TimerStop_hours 820		// Время остановки модуля (часы). Для режима (4)					(16 значений по одному байту)(суммарно 16)
#define E_TimerStop_minute 840		// Время остановки модуля (минуты). Для режима (4)					(16 значений по одному байту)(суммарно 16)
#define E_Timer_delta 860			// Период остановки модуля. Для работы в режиме управления (5, 6)	(16 значений по одному байту)(суммарно 16)
#define E_AllowReactToMaxValue 900	/* Реакция на превышение максимального значения датчика:	0 - ничего не делать
																								1 - отправлять СМС*/
#define E_MaxValueToReact 920		// (тип int) Максимальное значение датчика для реакции				(16 значений по два байта)(суммарно 32)
#define E_AllowReactToMinValue 960	/* Реакция на превышение максимального значения датчика:	0 - ничего не делать
																								1 - отправлять СМС*/
#define E_MinValueToReact 980		// (тип int) Минимальное значение датчика для реакции				(16 значений по два байта)(суммарно 32)
#define E_SendingSMSmaxValue 1000	// Флаг что СМС отправлялось										(16 значений по одному байту)(суммарно 16)
#define E_SendingSMSminValue 1020	// --//--//--//--													(16 значений по одному байту)(суммарно 16)


// =======================================================
// ================ Конфигурация датчиков ================
// =======================================================
#define E_StatusSensor 1300					// Статус датчика (включен\выключен)									(16 значений по одному байту)	(суммарно 16)
#define E_Address_Sensor 1340				// Адреса датчиков														(16 значений по 10 байт)		(суммарно 160)
#define E_Type_A_Sensor 1540				// Наименование датчиков												(16 значений по одному байту)	(суммарно 16)
#define E_Type_B_Sensor 1560				// Тип измеряемых данных												(16 значений по одному байту)	(суммарно 16)
#define E_SBG 1580							/* Привязка датчиков к группам ("Sensor Bing Channel")					(16 значений по три байта)		(суммарно 48)
													E_SBG состоит из трех байтов: E_SBG_A, E_SBG_B, E_SBG_C */  
#define E_AllowManualIntervalSensor 1760	// Включить ручной интервал опроса датчка. По default выключен			(16 значений по одному байту)	(суммарно 16)
#define E_LogingValueSensors 1780			// Логирование измеренных показаний										(16 значений по одному байту)	(суммарно 16)
#define E_AllowSensorControllPort 1800		// Использовать или нет порт управления									(16 значений по одному байту)	(суммарно 16)
#define E_SensorControllPort 1820			// Адрес порта управления датчиком										(16 значений по одному байту)	(суммарно 16)
#define E_ConfigSensor_A 1840				//			 "A" байт конфигурации датчика								(16 значений по одному байту)	(суммарно 16)
#define E_ConfigSensor_B 1860				// (тип int) "B" байт конфигурации датчика								(16 значений по два байта)		(суммарно 32)
#define E_ConfigSensor_C 1900				// (тип int) "C" байт конфигурации датчика								(16 значений по два байта)		(суммарно 32)
#define E_ConfigSensor_D 1940				// (тип int) "D" байт конфигурации датчика								(16 значений по два байта)		(суммарно 32)
#define E_NameSensor 1980					// Имя датчика (по 20 байт на каждый датчик)							(20 значений по 20 байт)		(Суммарно 420)
#define E_QuantityReadSensors 2400			// Количество измерений показаний для увеличения точности				(16 значений по одному байту)	(суммарно 16)
#define E_QuantityErrorReadSensor 2420		// Количество ошибок измерений датчика для отправки уведомлений			(16 значений по одному байту)	(суммарно 16)
											// Реакции на ошибки чтения:
#define E_ReactToMistakes_Ext 2440			/* Внешние уведомления:	0 - ничего не делать
																	1 - отправлять СМС
																	2 - отправлять по GPRS */
#define E_ReactToMistakes_SD 2460			// Писать или нет на SD карту											(16 значений по одному байту)	(суммарно 16)
#define E_ReactToMistakes_LED 2480			// Сигнализация LED														(16 значений по одному байту)	(суммарно 16)
#define E_QuantityErrors 2500				// (тип int) Счетчик ошибок чтения показаний							(16 значений по два байта)		(суммарно 32)
#define E_DelayToRunControllPort 2540		// (тип int) Задержка перед измерениeм показаний датчиком для включения Controll порта (16 значений по два байта)	(суммарно 32)
#define E_ErrorReadSensor_SMS 2580			// Отправлялось ли СМС об ошибке чтения датчиком						(16 значений по одному байту)	(суммарно 16)
#define E_SensorOff_SMS 2600				// Отправлялось ли СМС об отключении датчика							(16 значений по одному байту)	(суммарно 16)
#define E_ErrorReadSensor_SD 2620			// Писалось ли на SD об ошибке чтения датчиком							(16 значений по одному байту)	(суммарно 16)
#define E_SensorOff_SD 2640					// Писалось ли на SD об отключении датчика								(16 значений по одному байту)	(суммарно 16)
#define E_ReadSensorOK_SMS 2660				// Флаг что датчик работает корректно									(16 значений по одному байту)	(суммарно 16)
#define E_ArrangSensor 2680					/* Расположение датчика:	0 - основной контроллер
																		1-255 - удаленный, подключен по воздуху*/
#define E_SensorPollInterval 2700			// (тип int) Временной интервал опроса датчика (сек)(максимально ...)	(16 значений по два байта)		(суммарно 32)



// ================
// Различные ошибки
// ================
#define E_MaxQuantityErrorMinVCC 3001  // Счетчик количества максимальных разрядов аккумулятора


// ======================================================================
// Статусы отправки СМС уведомлений чтобы не отправлялись деблирующие СМС
// ======================================================================

#define E_LowLow_Input_VCC_SMS 3301
#define E_HighLow_Input_VCC_SMS 3301





// ===========================================================================================
// Статусы записи на SD карту (писались ли логи, чтобы не было дублированный записей на карте)
// ===========================================================================================
#define E_LowLow_Input_VCC_SD 3600
#define E_HighLow_Input_VCC_SD 3601

#define E_ReturnModulesToZeros 3699			// Возвращать или нет исполнительные модули в нули при старте контроллера
#define E_UpStepValue 3700					// 39 байт
#define E_ArchiveOldValueSensors 3740		// 200 байт		


#endif