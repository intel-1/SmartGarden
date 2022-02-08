#include <./Arduino.h>
#include <./EEPROM.h>

#include "RecievedControllCommand.h"
#include "../ParsingDataOnSerial.h"
#include "../EEPROM_ADR.h"
#include "../DigitalPorts.h"
#include "../ExecModules.h"


void RerecievedControllCommand(){
	if(InputFromSerial0[2] == 0){			// Если получаем команды управления
		byte LevelDigitalPort;
		boolean AllowSendCommand = true;
		byte NumberChannel = EEPROM.read(E_BindExecModile + InputFromSerial0[1]);		// Узнаем номер группы к которой привязан модуль
		byte TypeControllChannel = EEPROM.read(E_Controll_Channel + NumberChannel);		// Узнаем тип управления группой
		if(TypeControllChannel != 0){		// Если группа в режиме автоматического управления
			AllowSendCommand = false;		// Значит нельзя в ручную управлять модулем, нужно сначала перевести группу
											// в режим ручного управления или выдернуть из нее нужный исполнительный модуль
		}	
	
		if(AllowSendCommand){				// Если разрешено отправлять команды управления модулю
			byte Level = map(InputFromSerial0[3], 0, 100, 0, EEPROM.read(E_MaxLimitRotation + InputFromSerial0[1]));
			switch(InputFromSerial0[0]){
				case 1:
					break;
				case 2:
					break;
				case 3:
					switch(EEPROM.read(E_TypeExecModule + InputFromSerial0[1])){		// По типу модуля определяем что запускать
						case 1:											// --- Шаговый мотор
							break;
						case 2:											// --- ШИМ
							RunPWMonPort(EEPROM.read(E_PortExecModule + InputFromSerial0[1]), Level);
							break;
						case 3:											// --- Серво мотор
							RunServoMotor(InputFromSerial0[1], Level);
							break;
						case 4:											// --- Digitall port
							if(InputFromSerial0[3] <= 0){			// Если уровень полученного открывания больше "0"
								LevelDigitalPort = 1;
							}
							else LevelDigitalPort = 0;
						
							DigitalPort(EEPROM.read(E_PortExecModule + InputFromSerial0[1]), LevelDigitalPort, 2);		
							break;
						case 5:											// Коллекторный мотор
							break;
					}
					break;
			}
		}
	}
	
// 	for (byte i = 0; i < sizeof(InputFromSerial0) / sizeof(int); i ++){			// Затираем массив после работы
// 		InputFromSerial0[i] = 0;
// 	}
	CleanInputFromSerial0();
	recievedFlag_date = false;
}


//map(InputFromSerial0[3], 0, 100, 0, EEPROM.read(E_MaxLimitRotation + InputFromSerial0[1]));
/*

x = 100 * (4,2 - InputFromSerial0[3]) / (4,2 - 2,5)


x = 100 * (4,2 - y) / (4,2 - 2,5)

где x - искомые проценты, а y - число в диапазоне от 2,5 до 4,2.

	void RunStepperMotor(byte Module, int Step, byte Mode);
	*/