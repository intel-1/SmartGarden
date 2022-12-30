#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "lib\OneWire.h"
#include "lib\DallasTemperature.h"
#include "EEPROM_ADR.h"


#define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))		// Макрос для определения количества элементов в массиве


extern char VersionFirmware[];
extern char Short_VersionFirmware[];
extern char TextOfStartController[];

// ======================================================================
extern boolean ControllerSetup;			// Для обозначения стадии загрузки контроллера (Setup)
extern byte LOGING_TO_SERIAL;
extern char NameSensor[17][20];			// Всего датчиков 16 шт, но счет для удобства идет с единицы.
// ======================================================================
#define ADDRESS_INPUT_LM75 0x4c			// Адрес встроенного датчика температуры LM75
#define ADDRESS_INPUT_LCD 0x3f			// Адрес LCD экрана
#define ADDRESS_INPUT_INA 0x44			// Адрес распаяного ina219
#define ADDRESS_INPUT_EEPROM 0x50		// Адрес распаянной EEPROM AT24C32
#define ADDRESS_INPUT_RTC 0x68			// Адрес RTC DS3231
#define ADDRESS_INPUT_PCF8574 0x
// ======================================================================
#define QUANTITY_EXEC_MODULES 16		// Количество исполнительных модулей. Счет идет с единицы, нулевого нет!!!!!
#define QUANTITY_CHANNEL 16				// Количество групп. Счет идет с единицы, нулевого нет!!!!!
#define QUANTITY_SENSORS 16				// Количество сенсоров. Счет идет с единицы, нулевого нет!!!!!+
#define QuantityInputGPIO 6				// Количество портов группы "Input GPIO"
// ======================================================================
#define SENSOR_0 0
#define SENSOR_1 1
#define SENSOR_2 2
#define SENSOR_3 3
#define SENSOR_4 4
#define SENSOR_5 5
#define SENSOR_6 6
#define SENSOR_7 7
#define SENSOR_8 8
#define SENSOR_9 9
#define SENSOR_10 10
#define SENSOR_11 11
#define SENSOR_12 12
#define SENSOR_13 13
#define SENSOR_14 14
#define SENSOR_15 15
#define SENSOR_16 16
#define SENSOR_17 17
#define SENSOR_18 18
#define SENSOR_19 19
// ======================================================================
#define VALUE_1 0
#define VALUE_2 1
#define VALUE_3 2
// ======================================================================
#define ON 1
#define YES 1
#define OFF 0
#define NO 0
#define RESET 2
// ======================================================================
#define LCD_LINE_1 0
#define LCD_LINE_2 1
#define LCD_LINE_3 2
#define LCD_LINE_4 3
// ======================================================================
#define LCD_START_SYMBOL_1 0
#define LCD_START_SYMBOL_2 1
#define LCD_START_SYMBOL_3 2
#define LCD_START_SYMBOL_4 3
#define LCD_START_SYMBOL_5 4
#define LCD_START_SYMBOL_6 5
#define LCD_START_SYMBOL_7 6
#define LCD_START_SYMBOL_8 7
#define LCD_START_SYMBOL_9 8
#define LCD_START_SYMBOL_10 9
#define LCD_START_SYMBOL_11 10
#define LCD_START_SYMBOL_12 11
#define LCD_START_SYMBOL_13 12
#define LCD_START_SYMBOL_14 13
#define LCD_START_SYMBOL_15 14
#define LCD_START_SYMBOL_16 15
#define LCD_START_SYMBOL_17 16
#define LCD_START_SYMBOL_18 17
#define LCD_START_SYMBOL_19 18
#define LCD_START_SYMBOL_20 19
// ======================================================================
#define LED_START_CONTROLLER 0
#define LED_ERROR_VCC 1
#define LED_ERROR_SD_CARD 2
#define LED_NOT_SD_CARD 11


// =============================================================================
// ========================== Уровни логирования в UART ========================
// =============================================================================
#define UART_LOG_LEVEL_OFF 0
#define UART_LOG_LEVEL_ALL 1
#define UART_LOG_LEVEL_SENSOR 2
#define UART_LOG_LEVEL_CHANNEL 3
#define UART_LOG_LEVEL_MODULE 4
#define UART_LOG_LEVEL_GSM 5
#define UART_LOG_LEVEL_ERROR 7

#define OUTPUT_LEVEL_UART_GSM					LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL || ControllerSetup
#define OUTPUT_LEVEL_UART_SENSOR				LOGING_TO_SERIAL == UART_LOG_LEVEL_SENSOR || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL || ControllerSetup
#define OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP	LOGING_TO_SERIAL == UART_LOG_LEVEL_MODULE || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL
#define OUTPUT_LEVEL_UART_MODULE_AND_SETUP		LOGING_TO_SERIAL == UART_LOG_LEVEL_MODULE || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL && !ControllerSetup
#define OUTPUT_LEVEL_UART_CHANNEL				LOGING_TO_SERIAL == UART_LOG_LEVEL_CHANNEL || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL
#define OUTPUT_LEVEL_UART_CHANNEL_AND_SETUP		LOGING_TO_SERIAL == UART_LOG_LEVEL_CHANNEL || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL && !ControllerSetup


// =============================================================================
// ============================== Порты контроллера ============================
// =============================================================================
#define SD_VCC_RUN 45			// PL4 - Включение питания SD карточки
#define SD_CARD_CS 53			// CS порт карты памяти. Высокий уровень для активации

// Статусные светодиоды
#define STATUS_LED_GREEN PC2	//PC1 - Зеленый
#define STATUS_LED_RED PC1		//PC2 - Красный
#define STATUS_LED_BLUE PC3		//PC3 - Синий

// Пины драйверов шаговых моторов
#define STEPPER_STEP 67			// PK5
#define STEPPER_DIR 66			// PK4

// Порт включения питания драйверов исполнительных модулей
#define VCC_STEPPER_DRIVERS EXT_PIN_4	// PE7		

// NRF24L01
#define NRF_VCC_RUN PH7
#define NRF_CSN PH2

// GSM:
#define GSM_PWR_KEY PC4		
#define GSM_STATUS PA6
#define GSM_VCC_RUN PA2
#define GSM_DTR	PJ7

// Датчики
//#define DCPIN A15			//PK7 - Порт для измерения входного напряжения				ADC15

// Encoder:
#define ENCODER_A 56		//PF2
#define ENCODER_B 3			//PE5(INT5)
#define ENCODER_KEY 2		//PE4(INT4) - Кнопка энкодера

// Bluetooth модуль
#define BT_RUN_VCC PK6


/*
--------------------------------|---------------|-----------|---------------------------|
	Наименование порта			|   Тип порта	| Мнемоника |   Порт в Arduino формате	|
--------------------------------|---------------|-----------|---------------------------|	*/
// External Digital Pins (Пины исполнительных модулей):
#define EXT_PIN_1 PH3	//		| D,PWM			|	 (1)	|		    (6)				|
#define EXT_PIN_2 PE3	//		| D,PWM			|	 (2)	|		    (5)				|
#define EXT_PIN_3 PE6	//		| D,PWM,PDM		|	 (3)	|		    (-)				|
#define EXT_PIN_4 PE7	//		| D,PWM,PMD		|	 (4)	|		    (-)				|
#define EXT_PIN_5 PH5	//		| D,PWM,PDM		|	 (5)	|		    (8)				|
#define EXT_PIN_6 PH4	//		| D,PMD			|	 (6)	|		    (7)				|
#define EXT_PIN_7 PK5	//		| D,PWM,PDM		|	 (7)	|			(67)			|	A13
#define EXT_PIN_8 PK1	//		| D,PMD			|	 (8)	|		    (63)			|	A9
#define EXT_PIN_9 PK2	//		| D,PMD			|	 (9)	|			(64)			|	A10
#define EXT_PIN_10 PG5	//		| D,PWM,PMD		|	 (10)	|		    (4)				|
//#define EXT_PIN_11	
#define EXT_PIN_12 PK3	//		| D,PMD			|	 (12)	|			(65)			|	A11
#define EXT_PIN_13 PF6	//		| D,PMD			|	 (13)	|			(60)			|	A6
#define EXT_PIN_14 PK4	//		| D,PMD			|	 (14)	|			(66)			|	A12
// Output Digital Pins (Группа выходных цифровых портов):
#define GPIO_1 PC5		//		| D.MOSFET		|	 (21)	|		    (32)			|
#define GPIO_2 PG2		//		| D.MOSFET		|	 (22)	|		    (39)			|
#define GPIO_3 PA7		//		| D.MOSFET		|	 (23)	|		    (29)			|
#define GPIO_4 PA5		//		| D.MOSFET		|	 (24)	|		    (27)			|
#define GPIO_5 PA3		//		| D.MOSFET		|	 (25)	|		    (25)			|
#define GPIO_6 PA4		//		| D.MOSFET		|	 (26)	|		    (26)			|
#define GPIO_7 PA1		//		| D.MOSFET		|	 (27)	|		    (23)			|
#define GPIO_8 PA0		//		| D.MOSFET		|	 (28)	|		    (22)			|
// Input GPIO:
#define INPUT_GPIO_P1 9	//		| D.INPUT		|	 (31)	|			(9)				|	PH6		
#define INPUT_GPIO_P2 A0//		| A.INPUT,D		|	 (32)	|			(A0)			|	PF0		ADC0
#define INPUT_GPIO_P3 A1//		| A.INPUT,D		|	 (33)	|			(A1)			|	PF1		ADC1
#define INPUT_GPIO_P4 A6//		| A.INPUT,D		|	 (34)	|			(A6)			|	PF7		ADC7
#define INPUT_GPIO_P5 A3//		| A.INPUT,D		|	 (35)	|			(A3)			|	PF3		ADC3
#define INPUT_GPIO_P6 A4//		| A.INPUT,D		|	 (36)	|			(A4)			|	PF4		ADC4
// Input Digital Pins:
#define INPUT_D_PIN_1 PB5	//	| D.INPUT		|	 (41)	|		    (11)			|	PCINT5
#define INPUT_D_PIN_2 PB6	//	| D.INPUT		|	 (42)	|			(9)				|	PCINT6
#define INPUT_D_PIN_3 PB7	//	| D.INPUT		|	 (43)	|			(13)			|	PCINT7
#define INPUT_D_PIN_4 PJ2	//	| D.INPUT		|	 (44)	|			()				|	PCINT11
#define INPUT_D_PIN_5 PB4	//	| D.INPUT		|	 (45)	|			(15)			|	PCINT4
#define INPUT_D_PIN_6 PJ4	//	| D.INPUT		|	 (46)	|			()				|	PCINT13
#define INPUT_D_PIN_7 PJ3	//	| D.INPUT		|	 (47)	|			()				|	PCINT12
#define INPUT_D_PIN_8 PJ6	//	| D.INPUT		|	 (48)	|			()				|	PCINT15
#define INPUT_D_PIN_9 PJ5	//	| D.INPUT		|	 (49)	|			()				|	PCINT14


// =============================================================================
// ========================== Ф-ции управления портами =========================
// =============================================================================

boolean AllowPWMport(byte Port);
boolean AllowServoPort(byte Port);
boolean AllowAnalogPort(byte Port);
boolean AllowDigitalPort(byte Port);

// ============= STATUS LED =======================
void status_led_blue_on();
void status_led_blue_off();
void status_led_green_on();
void status_led_green_off();
void status_led_red_on();
void status_led_red_off();
void status_led_blue_invert();
void status_led_green_invert();
void status_led_red_invert();
void status_led_all_off();

// ============= GSM ===================
void gsm_vcc_on();
void gsm_vcc_off();
void gsm_pwr_key();
void gsm_dtr_on();
void gsm_dtr_off();

// ============= SD CARD =====================
void sd_vcc_on();
void sd_vcc_off();

// ============= BLUETOOTH =====================
void bt_vcc_on();
void bt_vcc_off();

// ============== EXT VCC EN ===================
void ext_vcc_en_on();
void ext_vcc_en_off();

// ============== Analog Input Pins ==============
void INPUT_GPIO_P1_on();
void INPUT_GPIO_P1_off();
void INPUT_GPIO_P2_on();
void INPUT_GPIO_P2_off();
void INPUT_GPIO_P3_on();
void INPUT_GPIO_P3_off();
void INPUT_GPIO_P4_on();
void INPUT_GPIO_P4_off();
void INPUT_GPIO_P5_on();
void INPUT_GPIO_P5_off();
void INPUT_GPIO_P6_on();
void INPUT_GPIO_P6_off();

// ============== Output Digital Pins ==============
void GPIO1_on();
void GPIO1_off();
void GPIO2_on();
void GPIO2_off();
void GPIO3_on();
void GPIO3_off();
void GPIO4_on();
void GPIO4_off();
void GPIO5_on();
void GPIO5_off();
void GPIO6_on();
void GPIO6_off();
void GPIO7_on();
void GPIO7_off();
void GPIO8_on();
void GPIO8_off();

// ============== External Digital Pins ==============
void EXT_PIN_1_on();
void EXT_PIN_1_off();
void EXT_PIN_2_on();
void EXT_PIN_2_off();
void EXT_PIN_3_on();
void EXT_PIN_3_off();
void EXT_PIN_4_on();
void EXT_PIN_4_off();
void EXT_PIN_5_on();
void EXT_PIN_5_off();
void EXT_PIN_6_on();
void EXT_PIN_6_off();
void EXT_PIN_7_on();
void EXT_PIN_7_off();
void EXT_PIN_8_on();
void EXT_PIN_8_off();
void EXT_PIN_9_on();
void EXT_PIN_9_off();
void EXT_PIN_10_on();
void EXT_PIN_10_off();
void EXT_PIN_11_on();
void EXT_PIN_11_off();
void EXT_PIN_12_on();
void EXT_PIN_12_off();
void EXT_PIN_13_on();
void EXT_PIN_13_off();
void EXT_PIN_14_on();
void EXT_PIN_14_off();

// ============== ENABLE пин питания драйвера шаговика ==============
void STEPPER_VCC_on();
void STEPPER_VCC_off();


// ============== Выставление всех портов на выход ============
void ControllAllPortsOutput();

#endif
