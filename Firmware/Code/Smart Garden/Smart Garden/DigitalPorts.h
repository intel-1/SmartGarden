#ifndef DIGITALPORTS_H
#define DIGITALPORTS_H

#include <stdint.h>
#include <Arduino.h>
#include "Configuration.h"


#define DIGITAL_PORT_RETURN_NAME_PORT 1					// Вывод имени порта
#define DIGITAL_PORT_SWITCH_PORT 2						// Переключение порта
#define DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT 3			// Вывод имя порта в формате Arduino
#define DIGITAL_PORT_READ_STATE_PORT 4					// Вывод состояния порта

#define LOG_TO_UART 1
#define NO_LOG_TO_UART 0

#define DIGITAL_PORT_OFF 0
#define DIGITAL_PORT_ON 1

#define OFF_STEPPER_DRIVER 0
#define ON_STEPPER_DRIVER 1


// External Digital Pins (Пины исполнительных модулей):
#define PORT_EXT_PIN_1  1
#define PORT_EXT_PIN_2  2
#define PORT_EXT_PIN_3  3
#define PORT_EXT_PIN_4  4
#define PORT_EXT_PIN_5  5
#define PORT_EXT_PIN_6  6
#define PORT_EXT_PIN_7  7
#define PORT_EXT_PIN_8  8
#define PORT_EXT_PIN_9  9
#define PORT_EXT_PIN_10 10
#define PORT_EXT_PIN_11 11
#define PORT_EXT_PIN_12 12
#define PORT_EXT_PIN_13 13
#define PORT_EXT_PIN_14 14
// Output Digital Pins (Группа выходных цифровых портов):
#define PORT_GPIO_1 21
#define PORT_GPIO_2 22
#define PORT_GPIO_3 23
#define PORT_GPIO_4 24
#define PORT_GPIO_5 25
#define PORT_GPIO_6 26
#define PORT_GPIO_7 27
#define PORT_GPIO_8 28
// Input GPIO:
#define PORT_INPUT_GPIO_P1 31
#define PORT_INPUT_GPIO_P2 32
#define PORT_INPUT_GPIO_P3 33
#define PORT_INPUT_GPIO_P4 34
#define PORT_INPUT_GPIO_P5 35
#define PORT_INPUT_GPIO_P6 36
// Input Digital Pins:
#define PORT_INPUT_D_PIN_1 41
#define PORT_INPUT_D_PIN_2 42
#define PORT_INPUT_D_PIN_3 43
#define PORT_INPUT_D_PIN_4 44
#define PORT_INPUT_D_PIN_5 45
#define PORT_INPUT_D_PIN_6 46
#define PORT_INPUT_D_PIN_7 47
#define PORT_INPUT_D_PIN_8 48
#define PORT_INPUT_D_PIN_9 49


extern int StateDigitalPorts[16];		// Текущее состоянее цифровых портов	([Номер порта][Состояние])
extern boolean StatusSwitchPorts[10];	// Текущее состояние концевиков 
extern boolean StateSwitch;


boolean ReadInputDigitalPorts(byte Port);								// Чтение состояния цифрового входа
void ViewNameInputDigitalPorts(byte Port);								// Вывод названия цифрового входа	
byte DigitalPort(byte NumberPort, byte Level, byte State, bool Log);	// Ф-ция включения цифровых портов
void ViewStateAllSwitch();												// Ф-ция вывода в консоль состояния всех концевиков


#endif