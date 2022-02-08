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
			Serial.print(StatusSwitchPorts[i - 30]);
			Serial.print(F(" "));
			//Serial.print(ReadInputDigitalPorts(i, false)); Serial.print(F(" "));
			if(i == 39){
				Serial.println();
			}
		}
	}
}
	

void ViewNameInputDigitalPorts(byte Port){
	switch(Port){
		case 31:
			Serial.print(F("INPUT_DIGITAL_PIN_1"));
			break;
		case 32:
			Serial.print(F("INPUT_DIGITAL_PIN_2"));
			break;
		case 33:
			Serial.print(F("INPUT_DIGITAL_PIN_3"));
			break;
		case 34:
			Serial.print(F("INPUT_DIGITAL_PIN_4"));
			break;
		case 35:
			Serial.print(F("INPUT_DIGITAL_PIN_5"));
			break;
		case 36:
			Serial.print(F("INPUT_DIGITAL_PIN_6"));
			break;
		case 37:
			Serial.print(F("INPUT_DIGITAL_PIN_7"));
			break;
		case 38:
			Serial.print(F("INPUT_DIGITAL_PIN_8"));
			break;
		case 39:
			Serial.print(F("INPUT_DIGITAL_PIN_9"));
			break;
		default:
			Serial.print(F("Error data"));
	}
}
	

boolean ReadInputDigitalPorts(byte Port){
	switch(Port){
		case 31:
			if(!(PINB & (1 << INPUT_D_PIN_1))){				// Если на порту '0'
				return true;
			}
			else return false;
			break;
		case 32:
			if(!(PINB & (1 << INPUT_D_PIN_2))){
				return true;
			}
			else return false;
			break;
		case 33:
			if(!(PINB & (1 << INPUT_D_PIN_3))){
				return true;
			}
			else return false;
			break;
		case 34:
			if(!(PINJ & (1 << INPUT_D_PIN_4))){
				return true;
			}
			else return false;
			break;
		case 35:
			/*if(!(PINJ & (1 << INPUT_D_PIN_5))){
				return true;
			}
			else return false;*/
			break;
		case 36:
			if(!(PINJ & (1 << INPUT_D_PIN_6))){
				return true;
			}
			else return false;
			break;
		case 37:
			if(!(PINJ & (1 << INPUT_D_PIN_7))){
				return true;
			}
			else return false;
			break;
		case 38:
			if(!(PINJ & (1 << INPUT_D_PIN_8))){
				return true;
			}
			else return false;
			break;
		case 39:
			if(!(PINJ & (1 << INPUT_D_PIN_9))){
				return true;
			}
			else return false;
			break;
		default:
			return false;
	}
}


byte DigitalPort(byte NumberPort, byte Level, byte State){
/*
	NumberPort - номер порта 
	Level - уровень который нужно выставить:
									0 - выключить порт
									1 - включить порт
	State - как работать с ф-цией:
									1 - выводить в консоль названия портов
									2 - включать\выключать порт
									3 - возвращать номер порта в Arduino формате
									4 - считать состояние порта
*/
	
	StateDigitalPorts[NumberPort - 1] = Level;				// Сохраняем состояние порта в массив
	switch(NumberPort){
		// ============== External Digital Pins ==============
		case 1:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("EXT_PIN_1"));
					}
					break;
				case 2:
					if(Level == 1){
						EXT_PIN_1_on();
					}
					else {
						EXT_PIN_1_off();
					}
					break;
				case 3:
					return 13;
					break;
			}
			break;
		case 2:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("EXT_PIN_2"));
					}
					break;
				case 2:
					if(Level == 1){
						EXT_PIN_2_on();
					}
					else {
						EXT_PIN_2_off();
					}
					break;
				case 3:
					return 6;
					break;
			}
			break;
		case 3:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("EXT_PIN_3"));
					}
					break;
				case 2:
					if(Level == 1){EXT_PIN_3_on();}
					else {EXT_PIN_3_off();}
					break;
				case 3:
					return 10;
					break;
			}
			break;
		case 5:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("EXT_PIN_4"));
					}
					break;
				case 2:
					if(Level == 1){
						EXT_PIN_4_on();
					}
					else {
						EXT_PIN_4_off();
					}
					break;
				case 3:
					return 4;
					break;
			}
			break;
		case 4:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("EXT_PIN_5"));
					}
					break;
				case 2:
					if(Level == 1){
						EXT_PIN_5_on();
					}
					else {
						EXT_PIN_5_off();
					}
					break;
				case 3:
					return A7;
					break;
			}
			break;
		case 6:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("EXT_PIN_6"));
					}
					break;
				case 2:
					if(Level == 1){
						EXT_PIN_6_on();
					}
					else {
						EXT_PIN_6_off();
					}
					break;
				case 3:
					return A8;
					break;
			}
			break;
		case 7:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("EXT_PIN_7"));
					}
					break;
				case 2:
					if(Level == 1){
						EXT_PIN_7_on();
					}
					else {
						EXT_PIN_7_off();
					}
					break;
				case 3:
					return 8;
					break;
			}
			break;
		case 8:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("EXT_PIN_8"));
					}
					break;
				case 2:
					if(Level == 1){
						EXT_PIN_8_on();
					}
					else {
						EXT_PIN_8_off();
					}
					break;
				case 3:
					return A9;
					break;
			}
			break;
		case 9:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("EXT_PIN_9"));
					}
					break;
				case 2:
					if(Level == 1){
						EXT_PIN_9_on();
					}
					else {
						EXT_PIN_9_off();
					}
					break;
				case 3:
					return A9;
					break;
			}
			Serial.print(F("Port isn't connected"));
			break;
		case 10:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("EXT_RUN_STEPPER"));
					}
					break;
				case 2:
					if(Level == 1){
						EXT_PIN_10_off();
					}
					else {
						EXT_PIN_10_off();
					}
					break;
				case 3:
					return A0;
					break;
			}
			break;
		
		// ============== Output Digital Pins ==============
		case 11:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("GPIO-1"));
					}
					break;
				case 2:
					if(Level == 1){
						GPIO1_on();
					}
					else {
						GPIO1_off();
					}
					break;
				case 3:
					return 32;
					break;
			}
			break;
		case 12:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("GPIO-2"));
					}
					break;
				case 2:
					if(Level == 1){
						GPIO2_on();
					}
					else {
						GPIO2_off();
					}
					break;
				case 3:
					return 31;
					break;
			}
			break;
		case 13:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("GPIO-3"));
					}
					break;
				case 2:
					if(Level == 1){
						GPIO3_on();
					}
					else {
						GPIO3_off();
					}
					break;
				case 3:
					return 39;
					break;
			}
			break;
		case 14:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("GPIO-4"));
					}
					break;
				case 2:
					if(Level == 1){
						GPIO4_on();
					}
					else {
						GPIO4_off();
					}
					break;
				case 3:
					return 29;
					break;
			}
			break;
		case 15:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("GPIO-5"));
					}
					break;
				case 2:
					if(Level == 1){
						GPIO5_on();
					}
					else {
						GPIO5_off();
					}
					break;
				case 3:
					return 25;
					break;
			}
			break;
		case 16:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("GPIO-6"));
					}
					break;
				case 2:
					if(Level == 1){
						GPIO6_on();
					}
					else {
						GPIO6_off();
					}
					break;
				case 3:
					return 26;
					break;
			}
			break;
		case 17:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("GPIO-7"));
					}
					break;
				case 2:
					if(Level == 1){
						GPIO7_on();
					}
					else {
						GPIO7_off();
					}
					break;
				case 3:
					return 24;
					break;
			}
			break;
		case 18:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("GPIO-8"));
					}
					break;
				case 2:
					if(Level == 1){
						GPIO8_on();
					}
					else {
						GPIO8_off();
					}
					break;
				case 3:
					return 23;
					break;
			}
			break;
			
		// ============== Analog Input Pins ==============
		case 20:							
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("Analog S1"));
					}
					break;
				case 2:
					if(Level == 1){
						INPUT_A_S1_on();
					}
					else {
						INPUT_A_S1_off();
					}
					break;
				case 3:
					return 9;
					break;
			}
			break;
		case 21:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("Analog S2"));
					}
					break;
				case 2:
					if(Level == 1){
						INPUT_A_S2_on();
					}
					else {
						INPUT_A_S2_off();
					}
					break;
				case 3:
					return 8;
					break;
			}
			break;
		case 22:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("Analog S3"));
					}
					break;
				case 2:
					if(Level == 1){
						INPUT_A_S3_on();
					}
					else {
						INPUT_A_S3_off();
					}
					break;
				case 3:
					return 60;
					break;
			}
			break;
		case 23:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("Analog S4"));
					}
					break;
				case 2:
					if(Level == 1){
						INPUT_A_S4_on();
					}
					else {
						INPUT_A_S4_off();
					}
					break;
				case 3:
					return 57;
					break;
			}
			break;
		case 24:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("Analog S5"));
					}
					break;
				case 2:
					if(Level == 1){
						INPUT_A_S5_on();
					}
					else {
						INPUT_A_S5_off();
					}
					break;
				case 3:
					return 58;
					break;
			}
			break;
		// ENABLE пин концевика
		case 40:
			switch(State){
				case 1:
					if (OUTPUT_LEVEL_UART_SENSOR){
						Serial.print(F("Enable Stepper Port"));
					}
					break;
				case 2:
					if(Level == 1){
						STEPPER_on();
					}
					else {
						STEPPER_off();
					}
					break;
				case 3:
					return 58;
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


byte ViewNameDigitalPort(byte NumberPort, byte TypeView){
/*
	NumberPort - номер порта 
	TypeView - в каком виде выводить имя:
									1 - в человеческом виде
									2 - в Arduino формате
*/
	
	switch(NumberPort){
		// ============== External Digital Pins ==============
		case 1:
			switch(TypeView){
				case 1:
					Serial.print(F("EXT_PIN_1"));
					break;
				case 2:
					return 13;
					break;
			}
			break;
		case 2:
			switch(TypeView){
				case 1:
					Serial.print(F("EXT_PIN_2"));
					break;
				case 2:
					return 6;
					break;
			}
			break;
		case 3:
			switch(TypeView){
				case 1:
					Serial.print(F("EXT_PIN_3"));
					break;
				case 2:
					return 10;
					break;
			}
			break;
		case 5:
			switch(TypeView){
				case 1:
					Serial.print(F("EXT_PIN_4"));
					break;
				case 2:
					return 4;
					break;
			}
			break;
		case 4:
			switch(TypeView){
				case 1:
					Serial.print(F("EXT_PIN_5"));
					break;
				case 2:
					return A7;
					break;
			}
			break;
		case 6:
			switch(TypeView){
				case 1:
					Serial.print(F("EXT_PIN_6"));
					break;
				case 2:
					return A8;
					break;
			}
			break;
		case 7:
			switch(TypeView){
				case 1:
					Serial.print(F("EXT_PIN_7"));
					break;
				case 2:
					return 8;
					break;
			}
			break;
		case 8:
			switch(TypeView){
				case 1:
					Serial.print(F("EXT_PIN_8"));
					break;
				case 2:
					return A9;
					break;
			}
			break;
		case 9:
			switch(TypeView){
				case 1:
					Serial.print(F("EXT_PIN_9"));
					break;
				case 2:
					return A9;
					break;
			}
			Serial.print(F("Port isn't connected"));
			break;
		case 10:
			switch(TypeView){
				case 1:
					Serial.print(F("EXT_RUN_STEPPER"));
					break;
				case 2:
					return A0;
					break;
			}
			break;
		
		// ============== Output Digital Pins ==============
		case 11:
			switch(TypeView){
				case 1:
					Serial.print(F("GPIO-1"));
					break;
				case 2:
					return 32;
					break;
			}
			break;
		case 12:
			switch(TypeView){
				case 1:
					Serial.print(F("GPIO-2"));
					break;
				case 2:
					return 31;
					break;
			}
			break;
		case 13:
			switch(TypeView){
				case 1:
					Serial.print(F("GPIO-3"));
					break;
				case 2:
					return 39;
					break;
			}
			break;
		case 14:
			switch(TypeView){
				case 1:
					Serial.print(F("GPIO-4"));
					break;
				case 2:
					return 29;
					break;
			}
			break;
		case 15:
			switch(TypeView){
				case 1:
					Serial.print(F("GPIO-5"));
					break;
				case 2:
					return 25;
					break;
			}
			break;
		case 16:
			switch(TypeView){
				case 1:
					Serial.print(F("GPIO-6"));
					break;
				case 2:
					return 26;
					break;
			}
			break;
		case 17:
			switch(TypeView){
				case 1:
					Serial.print(F("GPIO-7"));
					break;
				case 2:
					return 24;
					break;
			}
			break;
		case 18:
			switch(TypeView){
				case 1:
					Serial.print(F("GPIO-8"));
					break;
				case 2:
					return 23;
					break;
			}
			break;
			
		// ============== Analog Input Pins ==============
		case 20:							
			switch(TypeView){
				case 1:
					Serial.print(F("Analog S1"));
					break;
				case 2:
					return 9;
					break;
			}
			break;
		case 21:
			switch(TypeView){
				case 1:
					Serial.print(F("Analog S2"));
					break;
				case 3:
					return 8;
					break;
			}
			break;
		case 22:
			switch(TypeView){
				case 1:
					Serial.print(F("Analog S3"));
					break;
				case 3:
					return 60;
					break;
			}
			break;
		case 23:
			switch(TypeView){
				case 1:
					Serial.print(F("Analog S4"));
					break;
				case 3:
					return 57;
					break;
			}
			break;
		case 24:
			switch(TypeView){
				case 1:
					Serial.print(F("Analog S5"));
					break;
				case 2:
					return 58;
					break;
			}
			break;
		// ENABLE пин концевика
		case 40:
			switch(TypeView){
				case 1:
					Serial.print(F("Enable Stepper Port"));
					break;
				case 2:
					return 58;
					break;
			}
			break;
		default:
			if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
				if(TypeView != 3){			// Если не запрошен вывод номера порта в Arduino формате
// 					Serial.println();
// 					Serial.println(F("\t\t\tThe port isn't configured"));
				}
			}
			return 255;
	}
	if(TypeView == 4){
		Serial.println();
	}
}



/*

boolean ReadInputDigitalPorts(byte Port, boolean ViewName){
	switch(Port){
		case 31:
			if(!(PINB & (1 << INPUT_D_PIN_1))){				// Если на порту '0'
				return true;
			}
			else return false;
			break;
		case 32:
			if(!(PINB & (1 << INPUT_D_PIN_2))){
				return true;
			}
			else return false;
			break;
		case 33:
			if(!(PINB & (1 << INPUT_D_PIN_3))){
				return true;
			}
			else return false;
			break;
		case 34:
			if(!(PINJ & (1 << INPUT_D_PIN_4))){
				return true;
			}
			else return false;
			break;
		case 35:
//			if(!(PINJ & (1 << INPUT_D_PIN_5))){
//				return true;
//			}
//			else return false;
// 			break;
		case 36:
			if(!(PINJ & (1 << INPUT_D_PIN_6))){
				return true;
			}
			else return false;
			break;
		case 37:
			if(!(PINJ & (1 << INPUT_D_PIN_7))){
				return true;
			}
			else return false;
			break;
		case 38:
			if(!(PINJ & (1 << INPUT_D_PIN_8))){
				return true;
			}
			else return false;
			break;
		case 39:
			if(!(PINJ & (1 << INPUT_D_PIN_9))){
				return true;
			}
			else return false;
			break;
		default:
			return false;
	}
}

*/