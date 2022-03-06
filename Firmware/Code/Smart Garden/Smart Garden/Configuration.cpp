
#include <Arduino.h>
#include "Configuration.h"


char VersionFirmware[] = "firmware 2.0.2";
char Short_VersionFirmware[] = "ver. 2.0.2";



String TextOfStartController = "Controller is started";


boolean ControllerSetup;					// Для обозначения стадии загрузки контроллера (Setup)
byte LOGING_TO_SERIAL;
char NameSensor[QuantitySensors + 1][20];	// Максимум 16 датчиков по 20 символов (в названии) максимум, счет идет с единицы


// ============= Ф-ции для проверки портов =================
boolean AllowAnalogPort(byte Port){
	if(20 <= Port && Port <= 24){		// Разрешенные порты с 20-го по 24-ый
		return true;
	}
	else return false;
}

boolean AllowPWMport(byte Port){
	if(1 <= Port && Port <= 5){			// Разрешенные порты с 1-го по 5-ый
		return true;
	}
	if(Port == 7){						// Разрешенный порт 7
		return true;
	}
	if(Port == 40){						// Разрешенный порт 40
		return true;
	}
	else return false;
}

boolean AllowServoPort(byte Port){
	if(1 <= Port && Port <= 4){			// Разрешенные порты с 1-го по 4-ый
		return true;
	}
	if(Port == 40){						// Разрешенный порт 40
		return true;
	}
	else return false;
}

boolean AllowDigitalPort(byte Port){
	// External Digital Pins
	if(1 <= Port && Port <= 10){		// Разрешенные порты с 1-го по 8-ый
		return true;
	}
	// Output Digital Pins
	if(11 <= Port && Port <= 18){		// Разрешенные порты с 11-го по 18-ый
		return true;
	}
	if(Port == 40){						// Разрешенный порт 40
		return true;
	}
	else return false;
}




// ============= STATUS LED =======================
void status_led_all_off(){
	status_led_green_off();
	status_led_red_off();
	status_led_blue_off();
}
// Зеленый
void status_led_green_on(){
	DDRC |= _BV(STATUS_LED_GREEN);	// OUTPUT
	PORTC |= _BV(STATUS_LED_GREEN);	// 1
}
void status_led_green_off(){
	DDRC |= _BV(STATUS_LED_GREEN);	// OUTPUT
	PORTC &= ~_BV(STATUS_LED_GREEN);	// 0
}
void status_led_green_invert(){
	PORTC ^= _BV(STATUS_LED_GREEN);
}
// Red
void status_led_red_on(){
	DDRC |= _BV(STATUS_LED_RED);	// OUTPUT
	PORTC |= _BV(STATUS_LED_RED);	// 1
}
void status_led_red_off(){
	DDRC |= _BV(STATUS_LED_RED);	// OUTPUT
	PORTC &= ~_BV(STATUS_LED_RED);	// 0
}
void status_led_red_invert(){
	PORTC ^= _BV(STATUS_LED_RED);
}
// Blue
void status_led_blue_on(){
	DDRC |= _BV(STATUS_LED_BLUE);	// OUTPUT
	PORTC |= _BV(STATUS_LED_BLUE);	// 1
}
void status_led_blue_off(){
	DDRC |= _BV(STATUS_LED_BLUE);	// OUTPUT
	PORTC &= ~_BV(STATUS_LED_BLUE);	// 0
}
void status_led_blue_invert(){
	PORTC ^= _BV(STATUS_LED_BLUE);
}



// ============= GSM ===================
void gsm_vcc_on(){
	DDRA |= _BV(GSM_VCC_RUN);	// OUTPUT
	PORTA |= _BV(GSM_VCC_RUN);	// 1
}
void gsm_vcc_off(){
	DDRA |= _BV(GSM_VCC_RUN);	// OUTPUT
	PORTA &= ~_BV(GSM_VCC_RUN);	// 0
}
void gsm_pwr_key(){
	DDRC |= _BV(GSM_PWR_KEY);	// OUTPUT
	_delay_ms(600);				// Ждем
	PORTC |= _BV(GSM_PWR_KEY);	// 1 (подаем "0" на ногу pwr_key)
	//_delay_ms(2000);			// Ждем
	//PORTC &= ~_BV(GSM_PWR_KEY);	// 0
	//gsm_dtr_on();
}
void gsm_dtr_on(){
	DDRJ |= _BV(GSM_DTR);		// OUTPUT
	PORTJ |= _BV(GSM_DTR);		// 1
}
void gsm_dtr_off(){
	DDRJ |= _BV(GSM_DTR);		// OUTPUT
	PORTJ &= ~_BV(GSM_DTR);		// 0
}



// ============= SD CARD =====================
void sd_vcc_on(){
	//pinMode(SD_VCC_RUN, OUTPUT);
	//digitalWrite(SD_VCC_RUN, HIGH);
	DDRL |= _BV(SD_VCC_RUN);		// OUTPUT
	PORTL |= _BV(SD_VCC_RUN);	// 1
}
void sd_vcc_off(){
	//digitalWrite(SD_VCC_RUN, LOW);
	DDRL |= _BV(SD_VCC_RUN);	// OUTPUT
	PORTL &= ~_BV(SD_VCC_RUN);	// 0
}




// ============= BLUETOOTH =====================
void bt_vcc_on(){
	DDRK |= _BV(BT_RUN_VCC);	// OUTPUT
	PORTK &= ~_BV(BT_RUN_VCC);	// 0
}
void bt_vcc_off(){
	DDRK |= _BV(BT_RUN_VCC);	// OUTPUT
	PORTK |= _BV(BT_RUN_VCC);	// 1
}



// ============== Output Digital Pins ==============
void GPIO1_on(){
	DDRC |= _BV(GPIO_1);	// OUTPUT
	PORTC &= ~_BV(GPIO_1);	// 0
}
void GPIO1_off(){
	DDRC |= _BV(GPIO_1);	// OUTPUT
	PORTC |= _BV(GPIO_1);	// 1
}
void GPIO2_on(){
	DDRG |= _BV(GPIO_2);	// OUTPUT
	PORTG &= ~_BV(GPIO_2);	// 0
}
void GPIO2_off(){
	DDRG |= _BV(GPIO_2);	// OUTPUT
	PORTG |= _BV(GPIO_2);	// 1
}
void GPIO3_on(){
	DDRA |= _BV(GPIO_3);	// OUTPUT
	PORTA &= ~_BV(GPIO_3);	// 0
}
void GPIO3_off(){
	DDRA |= _BV(GPIO_3);	// OUTPUT
	PORTA |= _BV(GPIO_3);	// 1
}
void GPIO4_on(){
	DDRA |= _BV(GPIO_4);	// OUTPUT
	PORTA &= ~_BV(GPIO_4);	// 0
}
void GPIO4_off(){
	DDRA |= _BV(GPIO_4);	// OUTPUT
	PORTA |= _BV(GPIO_4);	// 1
}
void GPIO5_on(){
	DDRA |= _BV(GPIO_5);	// OUTPUT
	PORTA &= ~_BV(GPIO_5);	// 0
}
void GPIO5_off(){
	DDRA |= _BV(GPIO_5);	// OUTPUT
	PORTA |= _BV(GPIO_5);	// 1
}
void GPIO6_on(){
	DDRA |= _BV(GPIO_6);	// OUTPUT
	PORTA &= ~_BV(GPIO_6);	// 0
}
void GPIO6_off(){
	DDRA |= _BV(GPIO_6);	// OUTPUT
	PORTA |= _BV(GPIO_6);	// 1
}
void GPIO7_on(){
	DDRA |= _BV(GPIO_7);	// OUTPUT
	PORTA &= ~_BV(GPIO_7);	// 0
}
void GPIO7_off(){
	DDRA |= _BV(GPIO_7);	// OUTPUT
	PORTA |= _BV(GPIO_7);	// 1
}
void GPIO8_on(){
	DDRC |= _BV(GPIO_8);	// OUTPUT
	PORTC &= ~_BV(GPIO_8);	// 0
}
void GPIO8_off(){
	DDRA |= _BV(GPIO_8);	// OUTPUT
	PORTA |= _BV(GPIO_8);	// 1
}



// ============== Analog Input Pins ==============
void INPUT_A_S1_on(){
	DDRF |= _BV(INPUT_A_S1);		// OUTPUT
	PORTF |= _BV(INPUT_A_S1);		// 1
}
void INPUT_A_S1_off(){
	DDRF |= _BV(INPUT_A_S1);		// OUTPUT
	PORTF &= ~_BV(INPUT_A_S1);		// 0
}
void INPUT_A_S2_on(){
	DDRF |= _BV(INPUT_A_S2);		// OUTPUT
	PORTF |= _BV(INPUT_A_S2);		// 1
}
void INPUT_A_S2_off(){
	DDRF |= _BV(INPUT_A_S2);		// OUTPUT
	PORTF &= ~_BV(INPUT_A_S2);		// 0
}
void INPUT_A_S3_on(){
	DDRF |= _BV(INPUT_A_S3);		// OUTPUT
	PORTF |= _BV(INPUT_A_S3);		// 1
}
void INPUT_A_S3_off(){
	DDRF |= _BV(INPUT_A_S3);		// OUTPUT
	PORTF &= ~_BV(INPUT_A_S3);		// 0
}
void INPUT_A_S4_on(){
	DDRF |= _BV(INPUT_A_S4);		// OUTPUT
	PORTF |= _BV(INPUT_A_S4);		// 1
}
void INPUT_A_S4_off(){
	DDRF |= _BV(INPUT_A_S4);		// OUTPUT
	PORTF &= ~_BV(INPUT_A_S4);		// 0
}
void INPUT_A_S5_on(){
	DDRF |= _BV(INPUT_A_S5);		// OUTPUT
	PORTF |= _BV(INPUT_A_S5);		// 1
}
void INPUT_A_S5_off(){
	DDRF |= _BV(INPUT_A_S5);		// OUTPUT
	PORTF &= ~_BV(INPUT_A_S5);		// 0
}



// ============== External Digital Pins ==============
void EXT_PIN_1_on(){
	DDRH |= _BV(EXT_PIN_1);		// OUTPUT
	PORTH |= _BV(EXT_PIN_1);	// 1
}
void EXT_PIN_1_off(){
	DDRH |= _BV(EXT_PIN_1);		// OUTPUT
	PORTH &= ~_BV(EXT_PIN_1);	// 0
}
void EXT_PIN_2_on(){
	DDRE |= _BV(EXT_PIN_2);		// OUTPUT
	PORTE |= _BV(EXT_PIN_2);	// 1
}
void EXT_PIN_2_off(){
	DDRE |= _BV(EXT_PIN_2);		// OUTPUT
	PORTE &= ~_BV(EXT_PIN_2);	// 0
}
void EXT_PIN_3_on(){
	DDRE |= _BV(EXT_PIN_3);		// OUTPUT
	PORTE |= _BV(EXT_PIN_3);	// 1
}
void EXT_PIN_3_off(){
	DDRE |= _BV(EXT_PIN_3);		// OUTPUT
	PORTE &= ~_BV(EXT_PIN_3);	// 0
}
void EXT_PIN_4_on(){
	DDRH |= _BV(EXT_PIN_4);		// OUTPUT
	PORTH |= _BV(EXT_PIN_4);	// 1
}
void EXT_PIN_4_off(){
	DDRH |= _BV(EXT_PIN_4);		// OUTPUT
	PORTH &= ~_BV(EXT_PIN_4);	// 0
}
void EXT_PIN_5_on(){
	DDRE |= _BV(EXT_PIN_5);		// OUTPUT
	PORTE |= _BV(EXT_PIN_5);	// 1
}
void EXT_PIN_5_off(){
	DDRE |= _BV(EXT_PIN_5);		// OUTPUT
	PORTE &= ~_BV(EXT_PIN_5);	// 0
}
void EXT_PIN_6_on(){
	DDRK |= _BV(EXT_PIN_6);		// OUTPUT
	PORTK |= _BV(EXT_PIN_6);	// 1
}
void EXT_PIN_6_off(){
	DDRK |= _BV(EXT_PIN_6);		// OUTPUT
	PORTK &= ~_BV(EXT_PIN_6);	// 0
}
void EXT_PIN_7_on(){
	DDRH |= _BV(EXT_PIN_7);		// OUTPUT
	PORTH |= _BV(EXT_PIN_7);	// 1
}
void EXT_PIN_7_off(){
	DDRH |= _BV(EXT_PIN_7);		// OUTPUT
	PORTH &= ~_BV(EXT_PIN_7);	// 0
}
void EXT_PIN_8_on(){
	DDRK |= _BV(EXT_PIN_8);		// OUTPUT
	PORTK |= _BV(EXT_PIN_8);	// 1
}
void EXT_PIN_8_off(){
	DDRK |= _BV(EXT_PIN_8);		// OUTPUT
	PORTK &= ~_BV(EXT_PIN_8);	// 0
}
void EXT_PIN_9_on(){
	DDRF |= _BV(EXT_PIN_9);		// OUTPUT
	PORTF |= _BV(EXT_PIN_9);	// 1
}
void EXT_PIN_9_off(){
	DDRF |= _BV(EXT_PIN_9);		// OUTPUT
	PORTF &= ~_BV(EXT_PIN_9);	// 0
}
void EXT_PIN_10_on(){
	DDRK |= _BV(EXT_PIN_10);	// OUTPUT
	PORTK |= _BV(EXT_PIN_10);	// 1
}
void EXT_PIN_10_off(){
	DDRK |= _BV(EXT_PIN_10);	// OUTPUT
	PORTK &= ~_BV(EXT_PIN_10);	// 0
}

					
// ============== ENABLE пин шаговика ==============
void STEPPER_on(){
	DDRG |= _BV(EXT_RUN_STEPPER);	// OUTPUT
	PORTG |= _BV(EXT_RUN_STEPPER);	// 1
}
void STEPPER_off(){
	DDRG |= _BV(EXT_RUN_STEPPER);	// OUTPUT
	PORTG &= ~_BV(EXT_RUN_STEPPER);	// 0
}

// ============== Питание драйвера шаговика ==============
void STEPPER_VCC_on(){
	DDRH |= _BV(VCC_STEPPER_DRIVERS);	// OUTPUT
	PORTH |= _BV(VCC_STEPPER_DRIVERS);	// 1
}
void STEPPER_VCC_off(){
	DDRH |= _BV(VCC_STEPPER_DRIVERS);	// OUTPUT
	PORTH &= ~_BV(VCC_STEPPER_DRIVERS);	// 0
}

void ControllAllPortsOutput(){
	DDRG &= ~_BV(PG0);
	DDRG &= ~_BV(PG1);
	DDRG &= ~_BV(PG2);
	DDRG &= ~_BV(PG3);
	DDRG &= ~_BV(PG4);
	DDRG &= ~_BV(PG5);
	
	DDRL &= ~_BV(PL0);
	DDRL &= ~_BV(PL1);
	DDRL &= ~_BV(PL2);
	DDRL &= ~_BV(PL3);
	DDRL &= ~_BV(PL4);
	DDRL &= ~_BV(PL5);
	DDRL &= ~_BV(PL6);
	DDRL &= ~_BV(PL7);
	
	DDRD &= ~_BV(PD2);
	DDRD &= ~_BV(PD3);
	DDRD &= ~_BV(PD4);
	DDRD &= ~_BV(PD5);
	DDRD &= ~_BV(PD6);
	DDRD &= ~_BV(PD7);
	
	DDRC &= ~_BV(PC0);
	DDRC &= ~_BV(PC1);
	DDRC &= ~_BV(PC2);
	DDRC &= ~_BV(PC3);
	DDRC &= ~_BV(PC4);
	DDRC &= ~_BV(PC5);
	DDRC &= ~_BV(PC6);
	DDRC &= ~_BV(PC7);

	DDRA &= ~_BV(PA0);
	DDRA &= ~_BV(PA1);
	DDRA &= ~_BV(PA2);
	DDRA &= ~_BV(PA3);
	DDRA &= ~_BV(PA4);
	DDRA &= ~_BV(PA5);
	DDRA &= ~_BV(PA6);

	//DDRJ &= ~_BV(PJ0);
	//DDRJ &= ~_BV(PJ1);
	DDRJ &= ~_BV(PJ2);
	DDRJ &= ~_BV(PJ3);
	DDRJ &= ~_BV(PJ4);
	DDRJ &= ~_BV(PJ5);
	DDRJ &= ~_BV(PJ6);
	DDRJ &= ~_BV(PJ7);
	
	DDRF &= ~_BV(PF0);
	DDRF &= ~_BV(PF1);
	DDRF &= ~_BV(PF2);
	DDRF &= ~_BV(PF3);
	DDRF &= ~_BV(PF4);
	DDRF &= ~_BV(PF5);
	DDRF &= ~_BV(PF6);
	DDRF &= ~_BV(PF7);
	
	DDRK &= ~_BV(PK0);
	DDRK &= ~_BV(PK1);
	DDRK &= ~_BV(PK2);
	DDRK &= ~_BV(PK3);
	DDRK &= ~_BV(PK4);
	DDRK &= ~_BV(PK5);
	DDRK &= ~_BV(PK6);
	DDRK &= ~_BV(PK7);

	DDRE &= ~_BV(PE0);
	DDRE &= ~_BV(PE1);
	DDRE &= ~_BV(PE2);
	DDRE &= ~_BV(PE3);
	DDRE &= ~_BV(PE4);
	DDRE &= ~_BV(PE5);
	DDRE &= ~_BV(PE6);
	DDRE &= ~_BV(PE7);
	
	DDRH &= ~_BV(PH0);
	DDRH &= ~_BV(PH1);
	DDRH &= ~_BV(PH2);
	DDRH &= ~_BV(PH3);
	DDRH &= ~_BV(PH4);
	DDRH &= ~_BV(PH5);
	DDRH &= ~_BV(PH7);
	DDRH &= ~_BV(PH7);
	
	DDRB &= ~_BV(PB4);
	DDRB &= ~_BV(PB5);
	DDRB &= ~_BV(PB6);
	DDRB &= ~_BV(PB7);
}