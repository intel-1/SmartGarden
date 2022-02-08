

===================================================
============ Управляющие СМС комманды: ============
===================================================
  - reset		- Выполнение аппаратного reset
  - vcc			- Отправка по СМС напряжения питания
  - manual		- Перевод контроллера в ручной режим работы
  - wifi_run	- Включение Wi-Fi модуля (если установлен не постоянный режим работы)
  - status		- Отправка по СМС данных о работе контроллера


======================================================
=========== Управляющие по UART0 команды =============
======================================================
====== Протокол обмена данными: ======
Начало - 'Управляющий символ'
Разделитель - пробел
Завершающий символ - "/n"
Пример пакета: '$ 110 25 600 920/n' будет раскидан в массив согласно порядку слева направо
--------------------------------------
Уже задействованные управляющие символы:
case 't'	-- Настройка даты и времени
case 'c'	-- Настройка контроллера
case 'm'	-- Настройка рабочих модулей
case 's'    -- Настройка датчиков
case 'd'    -- Режим debug.
case 'u'	-- Время работы контроллера
case 'l'	-- Зарезервировано под настройки LCD экрана
case 'e'	-- Зарезервировано под настройки управления через GSM модуль 
case 'f'	-- Поиск адресов датчиков. Работает только на передачу в ф-ции debug
case 'g'    -- Настройка групп управления
case 'w'	-- Служебный байт для Windows приложения. Нужно чтобы приложение понимало что подключилось именно к контроллеру. 
			   При подключении шлется байт 'w', в ответ контроллер посылает строку "w <Версия прошивки>"
case 'p'	-- Перевод в режим конфигурирования. 
							======== Первый байт =================== Второй байт ========
									0:	Получить параметр			0:	Выключить режим конфигурирования
									1:	Отправить в ответ			1:  Включить --//--//--//--
case 'k'	-- Команды управления:	1 -- отправка СМС со значениями датчиков:
																			0 - 16 -- номера датчиков
									2 -- отправка СМС о состоянии исполнительных модулей:
																			0 - 16 -- номер модуля
									3 -- Открытие модуля:	1 - номер модуля
															2 - служебный байт: 0 - прием команды для исполнения.
																				1 - отправка состояния модуля (величины его открытия). Работает даже для модулей привязанных к 
																					группе настроенной на автоматическое управление.
															3 - величина открытия. Принимает значение в процентах от максимальной величины открытия
															4 - 


================================
--- Настройка даты и времени ---
Управляющий символ для получения данных - 't'
  '0  сек, 19 мин, 21 час, 27, октября, 2018 года, вторник'
  't 00 00 09 09 04 2018 2'
    hours
    minutes
    second 
    day
    month
    year
    weekday


==============================================
--- Конфигурирование команд управления GSM ---
Управляющий символ для получения данных - 'e'
'e '
=== Первый байт (команда) =============	Второй байт
	- 0:								
	- 1:	Отправка ответной СМС
	- 2:	Выполнение команды



================================
--- Параметры команды debug ---
Управляющий символ для получения данных - 'd'
'd 0 0 0'

=== Первый байт (Уровень отладки) =============	Второй байт =========================================================== Третий байт ==============
	- 0:	Не выводить ничего					- 0:	Выключение циклического вывода данных в консоль					- 0:	
	- 1:	Вывод всех данных					- 1:	Вывод состояния концевиков (выводятся в цикле, прекратить "0")	- 1:	Очистка ошибок в EEPROM
	- 2:	Только данные датчиков				- 2:	I2C сканер														- 2:	Очистка настроек датчиков
	- 3:	Только группы управления			- 3:	Сканер DS18B20 (вывод результатов с описаниями)					- 3:	Очистка настроек групп
	- 4:	Только исполнительные модули		- 4:	Сканер DS18B20 (вывод результатов поиска в виде байтов)			- 4:	Очистка настроек исполнительных модулей
	- 5:	Только GSM модуль					- 5:	Вывод в консоль всех ошибок из EEPROM							- 5:	Очистка настроек контроллера
												- 6:	Перезагрузка контроллера										- 6:	Полная очистка EEPROM
												/*- 7:	Перевод контроллера в ручной режим	*/											
												- 8:	Сбрасывание таймера выключения Bluetooth																
												/*- 9:	Включение режима конфигурирования контроллера*/
												/*- 10:	Выключение режима конфигурирования контроллера*/
												- 11:	Вывод времени работы контроллера (в виде байтов)
												- 12:	Вывод времени работы контроллера (с описаниями)
													======= Блок управления GSM модемом =======
												- 13:	Инициализация GSM модуля
												- 14:	"AT" - Готовность модуля к работе
												- 15:	"ATI" - Запрос информации об устройстве
												- 16:	"AT+CSQ" - Запрос качества связи
												- 17:	"AT+CREG?" - Тип регистрации в сети
												- 18:	"AT+COPS?" - Информация об операторе
												- 19:	"AT+CPAS" - Статус телефона GSM-модуля
												- 20:	"AT+CGMR" - Запрос версии установленного ПО
												- 21:	"AT+CCALR?" - Готовность модуля совершать звонки
												- 22:	"AT+GSN" - Запрос IMEI-модуля
												- 23:	"AT+GSV" - Запрос идентификационной информации модуля
												- 24:	"AT+CBC" - Напряжение питания
												- 25:	"AT+COPN" - Получение списка всех операторов
												- 26:	"AT+CFUN?" - Проверка функциональности модуля
													============================================
												- 27:	AlowRunMotor = false;	
												- 28:	Возвращать или нет модули в нули при старте контроллера (1\0, Да\Нет)	


================================
--- Конфигурация контроллера ---
  Управляющий символ для получения данных - 'c'	                                 
  ----------------------------------------------
    0	Служебный байт				- Если '1', то выводится инфа с описаниями, если '0', то только параметры
	1	E_ControllVCC				- Контролировать или нет напряжение питания
	2	E_MinInputVCC				- Минимальное напряжение питания
	3	E_SentSMSorStartController	- Отправлять или нет СМС о старте контроллера (Да\Нет, 1\0)
    4	E_LOGING_TO_SERIAL			- Уровень логирования в Serial (0 - молчать, 1 - только ошибки, 2 - только данные, 3 - все) 
    5	E_ENABLE_LOGING_TO_SD		- Уровень логирования на SD карту (0 - молчать, 1 - только ошибки, 2 - только данные, 3 - все)
    6	E_InputVCC					- Напряжение питания подаваемое на контроллер контроллера (5 или 12 вольт)
    7	E_CfCalcDC					- Поправочный коэффициент для правильного измерения напряжения питания
    8	E_ModeController			- Режим работы контроллера (Ручной\Автоматический\Отладки) !!!!!!!! Не понятно нужно или нет !!!!!!!!
	9   E_ConfigPowerBluetooth		- Режим подачи питания на Bluetooth модуль:	1 - Держать всегда включеным
																				2 - Включать по требованию и выключать после n минут бездействия
	10	E_BluetoothTimeOff			- Время отключения модуля при бездействии (сек). Работает только если E_ConfigPowerBluetooth выставлен в "2" 	
	11	E_LoopLCDLightTime			- Выключение подсветки экрана
	12	E_LoopSensorTime			- Измерение показаний датчиков
	13	E_LoopWriteValueSensorOnSD	- Логирование данных на карту памяти
	14	E_LoopChannelRun			- Управление группами	
	15	E_ReactToMinVCC				- Реакция на понижение напряжения питания:	0 - не реагировать
																				1 - отправлять СМС
																				2 - писать на SD карту
	16	Зарезервировано на будущее
	17	E_ResetTimeGSM					- Время перезагрузки модуля
	18	E_MaximumTimeResponseGSM		- Максимальное время ожидания ответа от модуля
	19	E_IntervalCheckRegistrationGSM	- Интервал проверки регистрации GSM (сек). Если стоит "0", то проверка не осуществляется
	20	E_WorkSIM800					- Режим питания SIM800:		0 - Всегда выключен
																	1 - Всегда включен
																	2 - Включать при отправке СМС
	21	E_AllowGPRS						- GPRS:	0 - Запрещен
												1 - Разрешен
	22-40 E_NameController				- Имя контроллера			



============================================================================
======================= Конфигурация датчиков ==============================
============================================================================
Управляющий символ для получения данных - 's'
's 1 1 0 1 2 2 28 FF 8B 5C 61 17 4 D4 0 0 0 0'
'  1 2 3 4 5 6 7  8  9  10 11 12 13 14 16 17 18 18 19 20 21 22 23 24 25-43 44 45 46 48 49'
---------------------------------------------
0		Номер датчика
1		Cлужебный байт
2		E_StatusSensor					- Состояние датчика (включен\выключен)(1\0)	
3		E_Type_A_Sensor					- Наименование датчиков:
																- 1  - DS18B20
																- 2  - AM2302*
																- 3  - Si7013, Si7020, Si7021, HTU21D, SHT21
																- 4  - BME280
																- 5  - BMP280
																- 6  - INA219
																- 7  - Analog Sensor
																- 8  - TSL2561
																- 9  - BH1750**
																- 10 - MAX44009
																- 11 - LM75*
																- 12 - INA3221*
4		E_Type_B_Sensor					- Тип данных измеряемых датчиком:
																			- 1   - "Temp sensor"				- Температура
																			- 2   - "Humidity Air"				- Влажности воздуха
																			- 3   - "Humidity Soil"				- Влажности почвы
																			- 4   - "Light Sensor"				- Освещение
																			- 5   - "Atmospheric Pressure"		- Атмосферное давление
																			- 6   - "External Tension"			- Напряжение
																			- 7   - "External Current"			- Ток
																			- 8	  - "Analog Value"				- Аналоговое значение
																			- 12  - "Multi(Temp and Humm)"				- Температура и влажность воздуха
																			- 15  - "Multi(Temp and Press)"				- Температура и атмосферное давление
																			- 125 - "Multi(Temp, Press and Humm Air)"	- Температура, влажность и атмосферное давление
																			- 67  - "Multi(Ext Tens and Curr)"			- Напряжение и ток
5		E_SGB_A							- Привязка первого значения датчика к группам (1-16)
6		E_SGB_B							- Привязка второго значения датчика к группам (1-16)
7		E_SGB_C							- Привязка третьего значения датчика к группам (1-16)
8-15	E_Address_Sensor				- Адрес датчика:
														- для DS18B20 8-и битный адрес
														- для I2C датчика одно битный
														- для Analog Port номер порта
16		E_AllowManualIntervalSensor		- Включение ручного интервала опроса датчика (вкл\выкл, 1\0)
17		E_SensorPollInterval			- Интервал опроса датчика
18		E_AllowSensorControllPort		- Использовать или нет Controll порт
19		E_SensorControllPort			- Адрес Controll порта
20		E_DelayToRunControllPort		- Задержка для включения Controll порта
21		E_ConfigSensor_A				- Конфигурационный байт "A":
																	- Для DS18B20 разрешение датчика.
																	- Для TSL2951 тип измеряемых данных:
																										1 - Видимый спектр
																										2 - Полный спектр
																										3 - IR спектр
																	- Для BME280 единицы измерения давления:
																										0 - мм ртутного столба (default значение)
																										1 - паскали
																	- Для Analog port единицы измеренения:
																										0 - проценты (default значение)
																										1 - абсолютное значение  ADC
																	- Для MAX44009 интервал измерения:
																										0 - Задержка 100ms (для тусклого света)
																										1 - Задержка 800ms (для яркого света) (default значение)
																	- Для Lm75 
22		E_ConfigSensor_B				- Конфигурационный байт "B":
																	- Для TSL2951 единицы измерения датчика:
																										0 - Люмены
																										1 - Люксы
																	- Для Analog port величина показаний для 0% если нужно измерять показания в %
23		E_ConfigSensor_C				- Конфигурационный байт "C":
																	- Для TSL2951 усиление:				1 - 16x (желательно для яркого света)
																										2 - 1x ( для тусклого света)
																										3 - Автоматическое усиление (default значение)
																	- Для Analog port величина показаний для 100% если нужно измерять показания в %
24		E_ConfigSensor_D				- Конфигурационный байт "D":
																	- Для TSL2951 время конвертации:	1 - 13 ms (default значение)
																										2 - 101 ms
																										3 - 402 ms
25-43	E_NameSensor					- Название датчика (не более 19-ти символов)	
44		E_QuantityReadSensors			- Количество измерений показаний (для увеличения точности)
45		E_QuantityErrorReadSensor		- Количество ошибок чтения, после которой идет реакция		
46		E_QuantityErrors				- Счетчик ошибок чтения показаний					
47		E_ReactToMistakes_Ext 			- Внешние уведомления:	0 - ничего не делать
																1 - отправлять СМС
																2 - отправлять по GPRS
48		E_ReactToMistakes_SD 			- Писать или нет на SD карту
49		E_ReactToMistakes_LED 			- Сигнализация LED



============================================================================
=================== Конфигурация рабочих модулей ===========================
============================================================================
Управляющий символ для получения данных - 'm'
'm 9 2 3 10 1500 3000 1600 1600'
'  0 1 2 3  4    5    6    7    8'
---------------------------------------------
0   Номер модуля
1	Служебный байт. Если '0', то выводим полную инфу (для отладки),  если '1', то выводим только байты конфигурации
2	E_StatusModule					- Статус модуля (вкл\выкл)
3	E_BindExecModile				- Привязка модуля к группе
4   E_TypeExecModule				- Тип модуля:	1 - шаговый мотор,
													2 - ШИМ,
													3 - серво мотор,
													4 - цифровой порт
													5 - коллекторный мотор
													6 - GSM модуль
5   E_MaxLimitRotation				- Максимальная величина открытия модуля
6	E_PortExecModule				- Адрес управляющего порта
7	E_StepperMotor_setMaxSpeed 		- Шаговый мотор. setMaxSpeed
8	E_StepperMotor_setAcceleration	- Шаговый мотор. setAcceleration
9	E_StepperMotor_Step				- Шаговый мотор. Step
10	E_StepperMotor_DriverStep		- Делитель шага Stepper драйвера:	1 - 1\1
																		2 - 1\2
																		4 - 1\4
																		8 - 1\8
																		16- 1\16					
11	E_Servo_MinImp					- Серво привод. Величина импульса для поворота в 0*
12	E_Servo_MaxImp					- Серво привод. Величина импульса для поворота в 180*
13	E_ManualModeModule				- Стандартный или ручной режим конфигурирования модуля. По умолчанию стоит '0'
											- Для шагового мотора:  
																	0 - управление мотором идет из разчета об\см.
																	1 - шаговик превращается в серво мотор и параметр E_MaxLimitRotation будет
																		приниматься как максимальный угол поворота в шагах.
											- Для серво привода:
																0 - стандартные величины импульсов для 0* и 180*
																1 - ручные значения для поворота в 0* и 180*
14	E_LowSwitchPortModule			- Номер порта для концевика MIN
15	E_HighSwitchPortModule			- Номер порта для концевика MAX
16	E_TypeHighSwitchModule			- Тип концевика HIGH


	
==================================
--- Настройка рабочих групп -----
	Управляющий символ для получения данных - 'g'
	'g 1 0'
	'  1 2 3 4 5 6 7 8 9 10'
	---------------------------------
	0	Номер группы
	1	Служебный байт. Если '0', то выводим полную инфу (для отладки),  если '1', то выводим только байты конфигурации
	2	Статус группы (вкл\выкл)(1\0)
	3	E_Controll_Channel			- Управление группой:	0 - Ручное управление, только смотреть за датчиками									(не работает)
															1 - Работа по термостату. Запускать исполняемый модуль пропорционально max\min 
																значениями сенсоров (ф-ция map()).
															2 - Работа по термостату. Управление при значении датчика > E_MinValueChannel
															3 - Работа по термостату. Управление при значении датчика < E_MinValueChannel
															4 - Работа по таймеру. Включение по E_TimeRun и выключение по E_TimeStop			(не работает)
															5 - Работа по таймеру. Включение по таймеру, выключение по длительности E_Delta		(не работает)
															6 - Включение\выключение через определенный интервал								(не работает)
	4	E_MinValueChannel			- Минимальное значение паказаний датчика для срабатывания модуля
	5	E_MaxValueChannel			- Максимальные значение. Применяется в режиме управления (1)
	6 	E_TimerStart_hours			- Время запуска модуля (часы)
	7	E_TimerStart_minute			- Время запуска модуля (минуты)
	8 	E_TimerStop_hours			- Время остановки модуля (часы). Для режима (4)
	9	E_TimerStop_minute			- Время остановки модуля (минуты). Для режима (4)
	10  E_Timer_delta				- Для работы в режиме управления (5)
	11	
	12
	13
	14
	
															

 
=============================================================================================================
====================== Описание массива (RealValueSensors[][]) со значениями датчиков: ======================
=============================================================================================================	
|------|--------------------------------|---------------|-----------------------|---------------------------|
| Байт |   	Тип измеряемых данных		|  Значение 1	|		Значение 2		|		  Значение 3		|
|------|--------------------------------|---------------|-----------------------|---------------------------|
| 1	   |Temp							| температура	|	------------------	|	--------------------	|															
| 2    |Humidity Air					| ------------	|	влажность воздуха	|	--------------------	|
| 3    |Humidity Soil					| ------------	|	влажность почвы		|	--------------------	|
| 4    |Light Sensor					| ------------	|  интенсивность света  |	--------------------	|
| 5    |Atmospheric Pressure			| ------------	|	------------------	|	атмосферное давление	|
| 6    |External Tension				| напряжение	|	------------------	|	--------------------	|
| 7    |External Current				| ------------	|			ток			|	--------------------	|
| 8    |Analog Value                    | аналоговое зн.|   ------------------  |   --------------------    |
| 12   |Multi(Temp and Humm)			| температура	|	влажность воздуха	|	--------------------	|
| 15   |Multi(Temp and Press)			| температура	|	------------------	|	атмосферное давление	|
| 25   |Multi(Press and Humm Air)		| ------------	|	влажность воздуха	|	атмосферное давление	|
| 125  |Multi(Temp, Press and Humm Air)	| температура	|	влажность воздуха	|	атмосферное давление	|
| 67   |Multi(Ext Tens and Curr)		| напряжение	|			ток			|	--------------------	|
|------|--------------------------------|---------------|-----------------------|---------------------------|



Описание управления группами:
1. В режиме управления группой (1) модули работают только в режимах (1) - Stepper, (2) - PWM и (3) - Servo. С модулями в режиме (4) - DigitalPort группа не работает.
2. В режимах (2) и (3) можно использовать модули в режиме (4 - цифровой порт) включая порт, а в режимах модуля (1) (2) и (3) величина
перемещения будет получена из параметра E_MaxLiftChannel


Параметр E_StepperMotor_setMaxSpeed_Motor можно настроить и диапазоне 0<Value<10000
Параметр E_StepperMotor_setAcceleration_Motor можно настроить и диапазоне 0<Value<10000