#include <./Arduino.h>
#include <./EEPROM.h>

#include "RecievedChannel.h"
#include "../EEPROM_ADR.h"
#include "../ParsingDataOnSerial.h"
#include "../Sensors.h"
#include "../DigitalPorts.h"
#include "../ExecModules.h"
#include "../main.h"


#define ONE_BYTE	0
#define ALL_BYTES	1

#define UART0_OUTPUT_DESCRIPTIONS	1
#define UART0_OUTPUT_ONE_BYTE		11
#define UART0_OUTPUT_ALL_BYTE		12
#define UART0_WRITE_ONE_BYTE		21
#define UART0_WRITE_ALL_BYTE		22
#define UART0_CLEAR_ALL_BYTE		3 

#define POS_NUMBER_GROUP		0
#define POS_SERVICE_BIT			1
#define POS_ADDRESS_WRITE_BIT	2
#define POS_STATUS_GROUP		3
#define POS_CONTROLL_GROUP		4
#define POS_MIN_VALUE_GROUP		5
#define POS_MAX_VALUE_GROUP		6
#define POS_TIMER_START_HOURS	7
#define POS_TIMER_START_MINUTE	8
#define POS_TIMER_STOP_HOURS	9
#define POS_TIMER_STOP_MINUTE	10
#define POS_TIMER_DELTA			11






// =============================================================================================================================================================================
// ============================================================================= Блок чтения в UART ============================================================================
// =============================================================================================================================================================================
void Output_Config_Bites(byte address_write_bit){
	switch(address_write_bit){
		case POS_STATUS_GROUP:
			Serial.print(EEPROM.read(E_StatusChannel + InputFromSerial0[POS_NUMBER_GROUP]));		Serial.print(F(" "));
			break;
		case POS_CONTROLL_GROUP:
			Serial.print(EEPROM.read(E_Controll_Channel + InputFromSerial0[POS_NUMBER_GROUP]));		Serial.print(F(" "));
			break;
		case POS_MIN_VALUE_GROUP:
			Serial.print(EEPROM.read(E_MinValueChannel + InputFromSerial0[POS_NUMBER_GROUP]));		Serial.print(F(" "));
			break;
		case POS_MAX_VALUE_GROUP:
			Serial.print(EEPROM.read(E_MaxValueChannel + InputFromSerial0[POS_NUMBER_GROUP]));		Serial.print(F(" "));
			break;
		case POS_TIMER_START_HOURS:
			Serial.print(EEPROM.read(E_TimerStart_hours + InputFromSerial0[POS_NUMBER_GROUP]));		Serial.print(F(" "));
			break;
		case POS_TIMER_START_MINUTE:
			Serial.print(EEPROM.read(E_TimerStart_minute + InputFromSerial0[POS_NUMBER_GROUP]));	Serial.print(F(" "));
			break;
		case POS_TIMER_STOP_HOURS:
			Serial.print(EEPROM.read(E_TimerStop_hours + InputFromSerial0[POS_NUMBER_GROUP]));		Serial.print(F(" "));
			break;
		case POS_TIMER_STOP_MINUTE:
			Serial.print(EEPROM.read(E_TimerStop_minute + InputFromSerial0[POS_NUMBER_GROUP]));		Serial.print(F(" "));
			break;
		case POS_TIMER_DELTA:
			Serial.print(EEPROM.read(E_Timer_delta + InputFromSerial0[POS_NUMBER_GROUP]));			Serial.println(F(" "));
			break;
	}
}


void Output_Config_debug_view(){
	Serial.print(F("=========== Config group '")); Serial.print(InputFromSerial0[POS_NUMBER_GROUP]); Serial.println(F("' ==========="));
	Serial.print(F("Status: "));
	switch(EEPROM.read(E_StatusChannel + InputFromSerial0[POS_NUMBER_GROUP])){
		case 1:
			Serial.println(F("on"));
			break;
		case 0:
			Serial.println(F("off"));
			break;
		default:
			Serial.println(F("It isn't configured"));
			
	}

	// ---------------------------------------------------------------------------------------------------------------------
	Serial.print(F("Controll: "));
	switch(EEPROM.read(E_Controll_Channel + InputFromSerial0[POS_NUMBER_GROUP])){
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
	
	// ---------------------------------------------------------------------------------------------------------------------
	Serial.print(F("Sensor: "));
	for(byte i = 0; i < 3; i ++){
		for(byte y = 1; y <= 16; y ++){
			if(EEPROM.read(E_SBG + y*3 + i) == InputFromSerial0[POS_NUMBER_GROUP]){
				Serial.print(y); Serial.print(F("  Value: ")); Serial.print(i + 1);
				break;
			}
		}
	}
	Serial.println();
	switch(EEPROM.read(E_Controll_Channel + InputFromSerial0[POS_NUMBER_GROUP])){
		case 1:
			Serial.print(F("MinValueChannel: ")); Serial.println(EEPROM.read(E_MinValueChannel + InputFromSerial0[POS_NUMBER_GROUP]));
			Serial.print(F("MaxValueChannel: ")); Serial.println(EEPROM.read(E_MaxValueChannel + InputFromSerial0[POS_NUMBER_GROUP]));
			break;
		case 2:
			Serial.print(F("MinValueChannel: ")); Serial.println(EEPROM.read(E_MinValueChannel + InputFromSerial0[POS_NUMBER_GROUP]));
			break;
		case 3:
			Serial.print(F("E_MinValueChannel: ")); Serial.println(EEPROM.read(E_MinValueChannel + InputFromSerial0[POS_NUMBER_GROUP]));
			Serial.print(F(""));
			break;
		case 4:
			Serial.print(F("Time start: ")); Serial.print(EEPROM.read(E_TimerStart_hours + InputFromSerial0[POS_NUMBER_GROUP])); Serial.print(F(":")); Serial.println(EEPROM.read(E_TimerStart_minute + InputFromSerial0[POS_NUMBER_GROUP]));
			Serial.print(F("Time stop: ")); Serial.print(EEPROM.read(E_TimerStop_hours + InputFromSerial0[POS_NUMBER_GROUP])); Serial.print(F(":")); Serial.println(EEPROM.read(E_TimerStop_minute + InputFromSerial0[POS_NUMBER_GROUP]));
			break;
		case 5:
			Serial.print(F("Time start: ")); Serial.print(EEPROM.read(E_TimerStart_hours + InputFromSerial0[POS_NUMBER_GROUP])); Serial.print(F(":")); Serial.println(EEPROM.read(E_TimerStart_minute + InputFromSerial0[POS_NUMBER_GROUP]));
			Serial.print(F("Timer_delta")); Serial.println(EEPROM.read(E_Timer_delta + InputFromSerial0[POS_NUMBER_GROUP]));
			break;
		default:
			Serial.println(F("It isn't configured"));
	}
	
	// ---------------------------------------------------------------------------------------------------------------------
	Serial.print(F("Exec module: "));
	for(byte Module = 1; Module <= QUANTITY_EXEC_MODULES; Module ++){
		if(EEPROM.read(E_BindExecModile + Module) == InputFromSerial0[POS_NUMBER_GROUP]){
			Serial.print(Module); Serial.print(F(" "));
		}
	}
	
	Serial.println();
	Serial.println(F("======================================="));
}



// =============================================================================================================================================================================
// ============================================================================= Блок записи в UART ============================================================================
// =============================================================================================================================================================================
void Write_Config_Byte(byte Pos){
	switch(Pos){
		case POS_STATUS_GROUP:
			if(InputFromSerial0[POS_STATUS_GROUP] == 0 || InputFromSerial0[POS_STATUS_GROUP] == 1){									// Разрешенное значение "0" или "1"
				EEPROM.update(E_StatusChannel + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_STATUS_GROUP]);
			}
			// Нужно при смене типа управления группой обнулять значения сохраненных шагов модулей
			if(0 < InputFromSerial0[3] && InputFromSerial0[3] <= 5 ){																// Разрешенное значение от 0-5.
				if(EEPROM.read(E_Controll_Channel + InputFromSerial0[POS_NUMBER_GROUP]) != InputFromSerial0[POS_CONTROLL_GROUP]){	// Если новое значение отлично от старого
					for(byte Module; Module <= QUANTITY_EXEC_MODULES; Module ++){													// Ищем модуль привязанный к номеру модуля
						if(EEPROM.read(E_BindExecModile + Module) == InputFromSerial0[POS_NUMBER_GROUP]){							// и если нашли
							UpStepValue[Module - 1] = 0;																			// то обнуляем ее рабочее значение в массиве
							PassableSteperMotor[Module] = 0;
						}
					}
					EEPROM.write(E_Controll_Channel + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_CONTROLL_GROUP]);
				}
			}
			break;
		case POS_MAX_VALUE_GROUP || POS_MIN_VALUE_GROUP || POS_TIMER_START_HOURS:
			if(	EEPROM.read(E_MinValueChannel + InputFromSerial0[POS_NUMBER_GROUP]) != InputFromSerial0[POS_MIN_VALUE_GROUP] ||
			EEPROM.read(E_MaxValueChannel + InputFromSerial0[POS_NUMBER_GROUP]) != InputFromSerial0[POS_MAX_VALUE_GROUP] ||		// При изменении любого из этих параметров запускается переинициализация модуля
			EEPROM.read(E_TimerStart_hours + InputFromSerial0[POS_NUMBER_GROUP]) != InputFromSerial0[POS_TIMER_START_HOURS]){
				switch(Pos){
					case POS_MIN_VALUE_GROUP:
						EEPROM.write(E_MinValueChannel + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_MIN_VALUE_GROUP]);
						break;
					case POS_MAX_VALUE_GROUP:
						EEPROM.write(E_MaxValueChannel + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_MAX_VALUE_GROUP]);
						break;
					case POS_TIMER_START_HOURS:
						EEPROM.write(E_TimerStart_hours + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_TIMER_START_HOURS]);
						break;
				}
				//InitializingExecModule(InputFromSerial0[POS_NUMBER_GROUP]);														// Инициализируем модуль, т.е. перемещаем его в нули
			}
			break;
		case POS_TIMER_START_HOURS:
			EEPROM.update(E_TimerStart_hours + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_TIMER_START_HOURS]);
			break;
		case POS_TIMER_START_MINUTE:
			EEPROM.update(E_TimerStart_minute + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_TIMER_START_MINUTE]);
			break;
		case POS_TIMER_STOP_HOURS:
			EEPROM.update(E_TimerStop_hours + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_TIMER_STOP_HOURS]);
			break;
		case POS_TIMER_STOP_MINUTE:
			EEPROM.update(E_TimerStop_minute + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_TIMER_STOP_MINUTE]);
			break;
		case POS_TIMER_DELTA:
			EEPROM.update(E_Timer_delta + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_TIMER_DELTA]);
			break;
	}
}


void Write_All_Config(){
	if(InputFromSerial0[POS_STATUS_GROUP] == 0 || InputFromSerial0[POS_STATUS_GROUP] == 1){									// Разрешенное значение "0" или "1"
		EEPROM.update(E_StatusChannel + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_STATUS_GROUP]);
	}
	// Нужно при смене типа управления группой обнулять значения сохраненных шагов модулей
	if(0 < InputFromSerial0[POS_CONTROLL_GROUP] && InputFromSerial0[POS_CONTROLL_GROUP] <= 5 ){								// Разрешенное значение от 0-5.
		if(EEPROM.read(E_Controll_Channel + InputFromSerial0[POS_NUMBER_GROUP]) != InputFromSerial0[POS_CONTROLL_GROUP]){	// Если новое значение отлично от старого
			for(byte Module; Module <= QUANTITY_EXEC_MODULES; Module ++){													// Ищем модуль привязанный к номеру модуля
				if(EEPROM.read(E_BindExecModile + Module) == InputFromSerial0[POS_NUMBER_GROUP]){							// и если нашли
					UpStepValue[Module - 1] = 0;																			// то обнуляем ее рабочее значение в массиве
					PassableSteperMotor[Module] = 0;
				}
			}
			EEPROM.write(E_Controll_Channel + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_CONTROLL_GROUP]);
		}
	}
	if(	EEPROM.read(E_MinValueChannel + InputFromSerial0[POS_NUMBER_GROUP]) != InputFromSerial0[POS_MIN_VALUE_GROUP] ||
		EEPROM.read(E_MaxValueChannel + InputFromSerial0[POS_NUMBER_GROUP]) != InputFromSerial0[POS_MAX_VALUE_GROUP] ||		// При изменении любого из этих параметров запускается переинициализация модуля
		EEPROM.read(E_TimerStart_hours + InputFromSerial0[POS_NUMBER_GROUP]) != InputFromSerial0[POS_TIMER_START_HOURS]){
			EEPROM.write(E_MinValueChannel + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_MIN_VALUE_GROUP]);
			EEPROM.write(E_MaxValueChannel + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_MAX_VALUE_GROUP]);
			EEPROM.write(E_TimerStart_hours + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_TIMER_START_HOURS]);
			InitializingExecModule(InputFromSerial0[POS_NUMBER_GROUP]);														// Инициализируем модуль, т.е. перемещаем его в нули
	}
	EEPROM.update(E_TimerStart_hours + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_TIMER_START_HOURS]);
	EEPROM.update(E_TimerStart_minute + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_TIMER_START_MINUTE]);
	EEPROM.update(E_TimerStop_hours + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_TIMER_STOP_HOURS]);
	EEPROM.update(E_TimerStop_minute + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_TIMER_STOP_MINUTE]);
	EEPROM.update(E_Timer_delta + InputFromSerial0[POS_NUMBER_GROUP], InputFromSerial0[POS_TIMER_DELTA]);
	Serial.println(F("Data channel in EEPROM is updated"));
}



// =============================================================================================================================================================================
// =============================================================================================================================================================================
// =============================================================================================================================================================================
void Work_Type(byte type){
	switch(type){
		case UART0_OUTPUT_DESCRIPTIONS:
			Output_Config_debug_view();
			break;
		case UART0_OUTPUT_ONE_BYTE:
			Output_Config_Bites(InputFromSerial0[POS_ADDRESS_WRITE_BIT]);
			break;
		case UART0_OUTPUT_ALL_BYTE:
			byte address_write_bit = POS_STATUS_GROUP;
			Serial.print(F("g "));
			Serial.print(InputFromSerial0[POS_NUMBER_GROUP]);	Serial.print(F(" "));
			Serial.print(F("0"));								Serial.print(F(" "));
			Serial.print(F("0"));								Serial.print(F(" "));
			for(byte pos = POS_STATUS_GROUP; pos <= 14; pos ++){
				Output_Config_Bites(address_write_bit);
				address_write_bit ++;
			}
			break;
	}
}

void Output(byte type){
	if(InputFromSerial0[POS_NUMBER_GROUP] != 255){
		Work_Type(type);
	}
	else{																// Выводим полную инфу для отладки
		for(byte Channel = 1; Channel <= QUANTITY_CHANNEL; Channel ++){
			InputFromSerial0[POS_NUMBER_GROUP] = Channel;
			Work_Type(type);
		}
	}
}

void RerecievedChannel(){		
	if(1 <= InputFromSerial0[POS_NUMBER_GROUP] && InputFromSerial0[POS_NUMBER_GROUP] <= QUANTITY_CHANNEL || InputFromSerial0[POS_NUMBER_GROUP] == 255){			// Разрешенное кол-во групп. Нельзя вводить больше
		switch (InputFromSerial0[POS_SERVICE_BIT]){									// Служебный байт
			case UART0_OUTPUT_ALL_BYTE:
				Output(InputFromSerial0[POS_SERVICE_BIT]);
				break;
			case UART0_OUTPUT_DESCRIPTIONS:
				Output(InputFromSerial0[POS_SERVICE_BIT]);
				break;
			case UART0_OUTPUT_ONE_BYTE:
				Output(InputFromSerial0[POS_SERVICE_BIT]);
				break;
			case UART0_WRITE_ONE_BYTE:
				Write_Config_Byte(InputFromSerial0[POS_ADDRESS_WRITE_BIT]);
				break;
			case UART0_WRITE_ALL_BYTE:
				boolean flag;
				for (byte i = POS_STATUS_GROUP; i < sizeof(InputFromSerial0)/sizeof(int); i++){		// Проверка что строка параметров не пустая. Из поиска исключаем первые служебные байты
					if (InputFromSerial0[i] == 0){
						flag = false;
					}
					else{
						flag = true;
						break;
					}
				}
				if (flag){ Write_All_Config();}
				break;
			case UART0_CLEAR_ALL_BYTE:
				break;
		}
	}
	
	CleanInputFromSerial0();
	recievedFlag_channel = false;
}




















// void RerecievedChannel(){
// 	boolean flag;
// 	for (byte i = POS_STATUS_GROUP; i < sizeof(InputFromSerial0)/sizeof(int); i++){		// Проверка что строка параметров не пустая. Из поиска исключаем первые служебные байты
// 		if (InputFromSerial0[i] == 0){
// 			flag = false;
// 		}
// 		else{
// 			flag = true;
// 			break;
// 		}
// 	}
// 	
// 	if(1 <= InputFromSerial0[POS_NUMBER_GROUP] && InputFromSerial0[POS_NUMBER_GROUP] <= QUANTITY_CHANNEL || InputFromSerial0[POS_NUMBER_GROUP] == 255){			// Разрешенное кол-во групп. Нельзя вводить больше
// 		if (flag){
// 			if(InputFromSerial0[POS_NUMBER_GROUP] != 255){
// 				Write_All_Config();
// 			}
// 		}
// 		else{
// 			switch (InputFromSerial0[POS_SERVICE_BIT]){									// Служебный байт
// 				case 0:																	// Выводим только байты конфигурации
// 					if(InputFromSerial0[POS_NUMBER_GROUP] != 255){
// 						SentDebugConfigModuleUART();
// 					}
// 					else{																// Выводим полную инфу для отладки
// 						for(byte Channel = 1; Channel <= QUANTITY_CHANNEL; Channel ++){
// 							InputFromSerial0[POS_NUMBER_GROUP] = Channel;
// 							SentDebugConfigModuleUART();
// 						}
// 					}
// 					break;
// 				case 1:
// 					if(InputFromSerial0[POS_NUMBER_GROUP] != 255){
// 						SentConfigChannelExtApp();
// 					}
// 					break;
// 				case 2:
// 					Write_Config_Byte(InputFromSerial0[POS_ADDRESS_WRITE_BIT]);
// 					break;
// 				case 3:
// 					break;
// 			}
// 		}
// 	}
// 	
// 	CleanInputFromSerial0();
// 	recievedFlag_channel = false;
// 	flag = false;
// }
