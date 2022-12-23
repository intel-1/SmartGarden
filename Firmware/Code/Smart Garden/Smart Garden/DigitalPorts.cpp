#include <Arduino.h>
#include <stdint.h>
#include <util/delay.h>

#include "DigitalPorts.h"
#include "EEPROM_ADR.h"
#include "Configuration.h"



int StateDigitalPorts[16];				// Текущее состоянее цифровых портов
boolean StatusSwitchPorts[10];			// Текущее состояние концевиков 
boolean StateSwitch = false;


void ViewStateAllSwitch(){				// Ф-ция вывода в консоль состояния всех концевиков
	while(StateSwitch){
		delay(800);
		for(byte i = 31; i <= 39; i ++ ){
			Serial.print(StatusSwitchPorts[i - 40]);
			Serial.print(F(" "));
			if(i == 39){
				Serial.println();
			}
		}
	}
}
	

void ViewNameInputDigitalPorts(byte Port){
	switch(Port){
		case 41:
			Serial.print(F("INPUT_DIGITAL_PIN_1"));
			break;
		case 42:
			Serial.print(F("INPUT_DIGITAL_PIN_2"));
			break;
		case 43:
			Serial.print(F("INPUT_DIGITAL_PIN_3"));
			break;
		case 44:
			Serial.print(F("INPUT_DIGITAL_PIN_4"));
			break;
		case 45:
			Serial.print(F("INPUT_DIGITAL_PIN_5"));
			break;
		case 46:
			Serial.print(F("INPUT_DIGITAL_PIN_6"));
			break;
		case 47:
			Serial.print(F("INPUT_DIGITAL_PIN_7"));
			break;
		case 48:
			Serial.print(F("INPUT_DIGITAL_PIN_8"));
			break;
		case 49:
			Serial.print(F("INPUT_DIGITAL_PIN_9"));
			break;
		default:
			Serial.print(F("Error data"));
	}
}
	

boolean ReadInputDigitalPorts(byte Port){
	switch(Port){
		case 41:
			if(!(PINB & (1 << INPUT_D_PIN_1))){				// Если на порту '0'
				return true;
			}
			else return false;
			break;
		case 42:
			if(!(PINB & (1 << INPUT_D_PIN_2))){
				return true;
			}
			else return false;
			break;
		case 43:
			if(!(PINB & (1 << INPUT_D_PIN_3))){
				return true;
			}
			else return false;
			break;
		case 44:
			if(!(PINJ & (1 << INPUT_D_PIN_4))){
				return true;
			}
			else return false;
			break;
		case 45:
			if(!(PINB & (1 << INPUT_D_PIN_5))){
				return true;
			}
			else return false;
			break;
		case 46:
			if(!(PINJ & (1 << INPUT_D_PIN_6))){
				return true;
			}
			else return false;
			break;
		case 47:
			if(!(PINJ & (1 << INPUT_D_PIN_7))){
				return true;
			}
			else return false;
			break;
		case 48:
			if(!(PINJ & (1 << INPUT_D_PIN_8))){
				return true;
			}
			else return false;
			break;
		case 49:
			if(!(PINJ & (1 << INPUT_D_PIN_9))){
				return true;
			}
			else return false;
			break;
		default:
			return false;
	}
}



byte DigitalPort(byte NumberPort, byte Level, byte State, bool Log){
/*
	NumberPort - номер порта 
	Level - уровень который нужно выставить:
									DIGITAL_PORT_OFF	- выключить порт
									DIGITAL_PORT_ON		- включить порт
	State - как работать с ф-цией:
									DIGITAL_PORT_RETURN_NAME_PORT			- выводить в консоль названия портов
									DIGITAL_PORT_SWITCH_PORT				- включать\выключать порт
									DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT	- возвращать номер порта в Arduino формате
									DIGITAL_PORT_READ_STATE_PORT			- считать состояние порта
*/
	
	StateDigitalPorts[NumberPort - 1] = Level;				// Сохраняем состояние порта в массив
	switch(NumberPort){
		// ============== External Digital Pins ==============
		case PORT_EXT_PIN_1:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_1"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_1_on();
					}
					else {
						EXT_PIN_1_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 6;
					break;
			}
			break;
		case PORT_EXT_PIN_2:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_2"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_2_on();
					}
					else {
						EXT_PIN_2_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 5;
					break;
			}
			break;
		case PORT_EXT_PIN_3:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_3"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_3_on();
					}
					else {
						EXT_PIN_3_off();
						}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 0;
					break;
			}
			break;
		case PORT_EXT_PIN_4:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_4"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_4_on();
					}
					else {
						EXT_PIN_4_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 0;
					break;
			}
			break;
		case PORT_EXT_PIN_5:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_5"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_5_on();
					}
					else {
						EXT_PIN_5_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 8;
					break;
			}
			break;
		case PORT_EXT_PIN_6:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_6"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_6_on();
					}
					else {
						EXT_PIN_6_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 7;
					break;
			}
			break;
		case PORT_EXT_PIN_7:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_7"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_7_on();
					}
					else {
						EXT_PIN_7_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 67;
					break;
			}
			break;
		case PORT_EXT_PIN_8:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_8"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_8_on();
					}
					else {
						EXT_PIN_8_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 63;
					break;
			}
			break;
		case PORT_EXT_PIN_9:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_9"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_9_on();
					}
					else {
						EXT_PIN_9_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 64;
					break;
			}
			break;
		case PORT_EXT_PIN_10:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_10"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_10_on();
					}
					else {
						EXT_PIN_10_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 4;
					break;
			}
			break;
		case PORT_EXT_PIN_11:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_11"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_11_on();
					}
					else {
						EXT_PIN_11_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return A9;
					break;
			}
			break;
		case PORT_EXT_PIN_12:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_12"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_12_on();
					}
					else {
						EXT_PIN_12_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 65;
					break;
			}
			break;
		case PORT_EXT_PIN_13:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_13"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_13_on();
					}
					else {
						EXT_PIN_13_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 60;
					break;
			}
			break;
		case PORT_EXT_PIN_14:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("EXT_PIN_14"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						EXT_PIN_14_on();
					}
					else {
						EXT_PIN_14_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 66;
					break;
			}
			break;
			
		
		// =====================================================================
		// ======================== Output Digital Pins ========================
		// =====================================================================
		case PORT_GPIO_1:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("GPIO-1"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						GPIO1_on();
					}
					else {
						GPIO1_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 32;
					break;
			}
			break;
		case PORT_GPIO_2:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("GPIO-2"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						GPIO2_on();
					}
					else {
						GPIO2_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 31;
					break;
			}
			break;
		case PORT_GPIO_3:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("GPIO-3"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						GPIO3_on();
					}
					else {
						GPIO3_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 39;
					break;
			}
			break;
		case PORT_GPIO_4:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("GPIO-4"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						GPIO4_on();
					}
					else {
						GPIO4_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 29;
					break;
			}
			break;
		case PORT_GPIO_5:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("GPIO-5"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						GPIO5_on();
					}
					else {
						GPIO5_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 25;
					break;
			}
			break;
		case PORT_GPIO_6:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("GPIO-6"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						GPIO6_on();
					}
					else {
						GPIO6_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 26;
					break;
			}
			break;
		case PORT_GPIO_7:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("GPIO-7"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						GPIO7_on();
					}
					else {
						GPIO7_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 24;
					break;
			}
			break;
		case PORT_GPIO_8:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("GPIO-8"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						GPIO8_on();
					}
					else {
						GPIO8_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 23;
					break;
			}
			break;
			
		// ============== Analog Input Pins ==============
		case PORT_INPUT_GPIO_P1:							
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("Input GPIO P1"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						INPUT_GPIO_P1_on();
					}
					else {
						INPUT_GPIO_P1_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return 9;
					break;
			}
			break;
		case PORT_INPUT_GPIO_P2:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("Input GPIO P2"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						INPUT_GPIO_P2_on();
					}
					else {
						INPUT_GPIO_P2_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return A0;
					break;
			}
			break;
		case PORT_INPUT_GPIO_P3:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("Input GPIO P3"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						INPUT_GPIO_P3_on();
					}
					else {
						INPUT_GPIO_P3_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return A1;
					break;
			}
			break;
		case PORT_INPUT_GPIO_P4:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("Input GPIO P4"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						INPUT_GPIO_P4_on();
					}
					else {
						INPUT_GPIO_P4_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return A6;
					break;
			}
			break;
		case PORT_INPUT_GPIO_P5:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("Input GPIO P5"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						INPUT_GPIO_P5_on();
					}
					else {
						INPUT_GPIO_P5_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return A3;
					break;
			}
			break;
		case PORT_INPUT_GPIO_P6:
			switch(State){
				case DIGITAL_PORT_RETURN_NAME_PORT:
					if (OUTPUT_LEVEL_UART_SENSOR || Log){
						Serial.print(F("Input GPIO P6"));
					}
					break;
				case DIGITAL_PORT_SWITCH_PORT:
					if(Level == DIGITAL_PORT_ON){
						INPUT_GPIO_P6_on();
					}
					else {
						INPUT_GPIO_P6_off();
					}
					break;
				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
					return A4;
					break;
			}
			break;
		default:
			return 255;
	}
	if(State == 4){
		if (OUTPUT_LEVEL_UART_SENSOR){
			Serial.println();
		}
	}
}
// 
// 
// byte ViewNameDigitalPort(byte NumberPort, byte TypeView){
// /*
// 	NumberPort - номер порта 
// 	TypeView - в каком виде выводить имя:
// 									1 - в человеческом виде
// 									3 - в Arduino формате
// */
// 	
// 	switch(NumberPort){
// 		// ============== External Digital Pins ==============
// 		case DIGITAL_PORT_RETURN_NAME_PORT:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_1"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 6;
// 					break;
// 			}
// 			break;
// 		case 2:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_2"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 5;
// 					break;
// 			}
// 			break;
// 		case 3:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_3"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return -;
// 					break;
// 			}
// 			break;
// 		case 4:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_4"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return -;
// 					break;
// 			}
// 			break;
// 		case 5:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_5"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 8;
// 					break;
// 			}
// 			break;
// 		case 6:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_6"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 7;
// 					break;
// 			}
// 			break;
// 		case 7:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_7"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 67;
// 					break;
// 			}
// 			break;
// 		case 8:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_8"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 63;
// 					break;
// 			}
// 			break;
// 		case 9:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_9"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 64;
// 					break;
// 			}
// 			break;
// 			
// 		case 10:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_10"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 4;
// 					break;
// 			}
// 		break;	
// 		case 12:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_12"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 65;
// 					break;
// 			}
// 		break;
// 		case 13:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_13"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 60;
// 					break;
// 			}
// 		break;
// 		case 14:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("EXT_PIN_14"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 66;
// 					break;
// 			}
// 		break;
// 
// 		
// 		// ============== Output Digital Pins ==============
// 		case 21:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("GPIO-1"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 32;
// 					break;
// 			}
// 			break;
// 		case 22:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("GPIO-2"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 31;
// 					break;
// 			}
// 			break;
// 		case 23:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("GPIO-3"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 39;
// 					break;
// 			}
// 			break;
// 		case 24:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("GPIO-4"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 29;
// 					break;
// 			}
// 			break;
// 		case 25:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("GPIO-5"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 25;
// 					break;
// 			}
// 			break;
// 		case 26:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("GPIO-6"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 26;
// 					break;
// 			}
// 			break;
// 		case 27:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("GPIO-7"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 24;
// 					break;
// 			}
// 			break;
// 		case 28:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("GPIO-8"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 23;
// 					break;
// 			}
// 			break;
// 			
// 		// ============== Analog Input Pins ==============
// 		case 20:							
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("Analog S1"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 9;
// 					break;
// 			}
// 			break;
// 		case 21:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("Analog S2"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 8;
// 					break;
// 			}
// 			break;
// 		case 22:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("Analog S3"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 60;
// 					break;
// 			}
// 			break;
// 		case 23:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("Analog S4"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 57;
// 					break;
// 			}
// 			break;
// 		case 24:
// 			switch(TypeView){
// 				case DIGITAL_PORT_RETURN_NAME_PORT:
// 					Serial.print(F("Analog S5"));
// 					break;
// 				case DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT:
// 					return 58;
// 					break;
// 			}
// 			break;
// 		default:
// 			if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
// 				if(TypeView != 3){			// Если не запрошен вывод номера порта в Arduino формате
// 					Serial.println();
// 					Serial.println(F("\t\t\tThe port isn't configured"));
// 				}
// 			}
// 			return 255;
// 	}
// 	if(TypeView == 4){
// 		Serial.println();
// 	}
// }