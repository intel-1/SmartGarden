#include <./Arduino.h>
#include <./EEPROM.h>

#include "RecievedChannel.h"
#include "../EEPROM_ADR.h"
#include "../ParsingDataOnSerial.h"
#include "../Sensors.h"
#include "../DigitalPorts.h"
#include "../ExecModules.h"
#include "../main.h"
//#include "../SDcard.h"



void SentConfigChannelExtApp(){
	Serial.print(F("g "));
	Serial.print(InputFromSerial0[0]);										Serial.print(F(" "));
	Serial.print(F("0"));													Serial.print(F(" "));
	Serial.print(EEPROM.read(E_StatusChannel + InputFromSerial0[0]));		Serial.print(F(" "));
	Serial.print(EEPROM.read(E_Controll_Channel + InputFromSerial0[0]));	Serial.print(F(" "));
	Serial.print(EEPROM.read(E_MinValueChannel + InputFromSerial0[0]));		Serial.print(F(" "));
	Serial.print(EEPROM.read(E_MaxValueChannel + InputFromSerial0[0]));		Serial.print(F(" "));
	Serial.print(EEPROM.read(E_TimerStart_hours + InputFromSerial0[0]));	Serial.print(F(" "));
	Serial.print(EEPROM.read(E_TimerStart_minute + InputFromSerial0[0]));	Serial.print(F(" "));
	Serial.print(EEPROM.read(E_TimerStop_hours + InputFromSerial0[0]));		Serial.print(F(" "));
	Serial.print(EEPROM.read(E_TimerStop_minute + InputFromSerial0[0]));	Serial.print(F(" "));
	Serial.print(EEPROM.read(E_Timer_delta + InputFromSerial0[0]));			Serial.println(F(" "));
}


void SentDebugConfigModuleUART(){
	Serial.print(F("=========== Config channel '")); Serial.print(InputFromSerial0[0]); Serial.println(F("' ==========="));
	Serial.print(F("Status: "));
	if (EEPROM.read(E_StatusChannel + InputFromSerial0[0]) == 1){
		Serial.println(F("on"));
	}
	else {
		Serial.println(F("off"));
	}
	Serial.print(F("Controll_Channel: "));
	switch(EEPROM.read(E_Controll_Channel + InputFromSerial0[0])){
		case 0:
			Serial.println("");
			break;
		case 1:
			Serial.println(F("(1) Thermostat mode. Proportional management"));
			break;
		case 2:
			Serial.println(F("(2) Thermostat mode. Management at excess of E_MinValueChannel value"));
			break;
		case 3:
			Serial.println(F("(3) Thermostat mode. Management at value of the sensor is less than E_MinValueChannel"));
			break;
		case 4:
			Serial.println(F("(4) Timer mode. Parameter 4"));
			break;
		case 5:
			Serial.println(F("(5) Timer mode. Parameter 5"));
			break;
		default:
			Serial.println(F("It isn't configured"));
	}
	Serial.print(F("Sensor: "));
	for(byte i = 0; i < 3; i ++){
		for(byte y = 1; y <= 16; y ++){
			if(EEPROM.read(E_SBG + y*3 + i) == InputFromSerial0[0]){
				Serial.print(y); Serial.print(F("  Value: ")); Serial.print(i + 1);
				break;
			}
		}
	}
	Serial.println(F(""));
	switch(EEPROM.read(E_Controll_Channel + InputFromSerial0[0])){
		case 0:
			Serial.print(F(""));
			break;
		case 1:
			Serial.print(F("MinValueChannel: ")); Serial.println(EEPROM.read(E_MinValueChannel + InputFromSerial0[0]));
			Serial.print(F("MaxValueChannel: ")); Serial.println(EEPROM.read(E_MaxValueChannel + InputFromSerial0[0]));
			break;
		case 2:
			Serial.print(F("MinValueChannel: ")); Serial.println(EEPROM.read(E_MinValueChannel + InputFromSerial0[0]));
			break;
		case 3:
			Serial.print(F("E_MinValueChannel: ")); Serial.println(EEPROM.read(E_MinValueChannel + InputFromSerial0[0]));
			Serial.print(F(""));
			break;
		case 4:
			Serial.print(F("Time start: ")); Serial.print(EEPROM.read(E_TimerStart_hours + InputFromSerial0[0])); Serial.print(F(":")); Serial.println(EEPROM.read(E_TimerStart_minute + InputFromSerial0[0]));
			Serial.print(F("Time stop: ")); Serial.print(EEPROM.read(E_TimerStop_hours + InputFromSerial0[0])); Serial.print(F(":")); Serial.println(EEPROM.read(E_TimerStop_minute + InputFromSerial0[0]));
			break;
		case 5:
			Serial.print(F("Time start: ")); Serial.print(EEPROM.read(E_TimerStart_hours + InputFromSerial0[0])); Serial.print(F(":")); Serial.println(EEPROM.read(E_TimerStart_minute + InputFromSerial0[0]));
			Serial.print(F("Timer_delta")); Serial.println(EEPROM.read(E_Timer_delta + InputFromSerial0[0]));
			break;
		default:
			Serial.println(F("External mode: It isn't configured"));
	}
	Serial.print(F("Exec module: "));
	
	for(byte Module = 1; Module <= QUANTITY_EXEC_MODULES; Module ++){
		if(EEPROM.read(E_BindExecModile + Module) == InputFromSerial0[0]){
			Serial.print(Module); Serial.print(F(" "));
		}
	}
	
	Serial.println(F(""));
	Serial.println(F("======================================="));
}


void WriteConfigChannel(){
	if(InputFromSerial0[2] == 0 || InputFromSerial0[2] == 1){								// Разрешенное значение "0" или "1"
		EEPROM.update(E_StatusChannel + InputFromSerial0[0], InputFromSerial0[2]);
	}
	// Нужно при смене типа управления группой обнулять значения сохраненных шагов модулей
	if(0 < InputFromSerial0[3] && InputFromSerial0[3] <= 5 ){								// Разрешенное значение от 0-5.
		if(EEPROM.read(E_Controll_Channel + InputFromSerial0[0]) != InputFromSerial0[3]){	// Если новое значение отлично от старого
			for(byte Module; Module <= QUANTITY_EXEC_MODULES; Module ++){					// Ищем модуль привязанный к номеру модуля
				if(EEPROM.read(E_BindExecModile + Module) == InputFromSerial0[0]){			// и если нашли
					UpStepValue[Module - 1] = 0;											// то обнуляем ее рабочее значение в массиве
					PassableSteperMotor[Module] = 0;
				}
			}
			EEPROM.write(E_Controll_Channel + InputFromSerial0[0], InputFromSerial0[3]);
		}
	}
	if(	EEPROM.read(E_MinValueChannel + InputFromSerial0[0]) != InputFromSerial0[4] ||
		EEPROM.read(E_MaxValueChannel + InputFromSerial0[0]) != InputFromSerial0[5] ||		// При изменении любого из этих параметров запускается переинициализация модуля
		EEPROM.read(E_TimerStart_hours + InputFromSerial0[0]) != InputFromSerial0[6]){
			EEPROM.write(E_MinValueChannel + InputFromSerial0[0], InputFromSerial0[4]);
			EEPROM.write(E_MaxValueChannel + InputFromSerial0[0], InputFromSerial0[5]);
			EEPROM.write(E_TimerStart_hours + InputFromSerial0[0], InputFromSerial0[6]);
			InitializingExecModule(InputFromSerial0[0]);									// Инициализируем модуль, т.е. перемещаем его в нули
	}
	EEPROM.update(E_TimerStart_hours + InputFromSerial0[0], InputFromSerial0[6]);
	EEPROM.update(E_TimerStart_minute + InputFromSerial0[0], InputFromSerial0[7]);
	EEPROM.update(E_TimerStop_hours + InputFromSerial0[0], InputFromSerial0[8]);
	EEPROM.update(E_TimerStop_minute + InputFromSerial0[0], InputFromSerial0[9]);
	EEPROM.update(E_Timer_delta + InputFromSerial0[0], InputFromSerial0[10]);
	Serial.println(F("Data channel in EEPROM is updated"));
}


void RerecievedChannel(){
	boolean flag;
	for (byte i = 2; i < sizeof(InputFromSerial0)/sizeof(int); i++){
		if (InputFromSerial0[i] == 0){
			flag = false;
		}
		else{
			flag = true;
			break;
		}
	}
	
	if(1 <= InputFromSerial0[0] && InputFromSerial0[0] <= QUANTITY_CHANNEL || InputFromSerial0[0] == 255){			// Разрешенное кол-во групп не более 16-ти. Нельзя вводить больше
		if (flag){
			if(InputFromSerial0[0] != 255){
				WriteConfigChannel();
			}
		}
		else{
			switch (InputFromSerial0[1]){
				case 1:
				if(InputFromSerial0[0] != 255){
					SentConfigChannelExtApp();
				}
				break;
				case 0:													// Выводим только байты конфигурации
				if(InputFromSerial0[0] != 255){
					SentDebugConfigModuleUART();
				}
				else{												// Выводим полную инфу для отладки
					for(byte Channel = 1; Channel <= QUANTITY_CHANNEL; Channel ++){
						InputFromSerial0[0] = Channel;
						SentDebugConfigModuleUART();
					}
				}
				break;
			}
		}
	}
	
// 	for (byte i=0; i<sizeof(InputFromSerial0)/sizeof(int); i++){				// Затираем массив после работы
// 		InputFromSerial0[i] = 0;
// 	}
	CleanInputFromSerial0();
	recievedFlag_channel = false;
	flag = false;
}
