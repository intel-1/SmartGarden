#ifndef DIGITALPORTS_H
#define DIGITALPORTS_H

#include <stdint.h>
#include <Arduino.h>
#include "Configuration.h"


extern int StateDigitalPorts[16];		// Текущее состоянее цифровых портов	([Номер порта][Состояние])
extern boolean StatusSwitchPorts[10];	// Текущее состояние концевиков 
extern boolean StateSwitch;


boolean ReadInputDigitalPorts(byte Port);						// Чтение состояния цифрового входа
void ViewNameInputDigitalPorts(byte Port);						// Вывод названия цифрового входа	
byte DigitalPort(byte NumberPort, byte Level, byte State);		// Ф-ция включения цифровых портов
void ViewStateAllSwitch();										// Ф-ция вывода в консоль состояния всех концевиков


#endif