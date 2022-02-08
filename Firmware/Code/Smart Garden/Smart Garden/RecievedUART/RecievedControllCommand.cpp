#include <./Arduino.h>
#include <./EEPROM.h>

#include "RecievedControllCommand.h"
#include "../ParsingDataOnSerial.h"
#include "../EEPROM_ADR.h"
#include "../DigitalPorts.h"
#include "../ExecModules.h"


void RerecievedControllCommand(){
	if(InputFromSerial0[2] == 0){			// ���� �������� ������� ����������
		byte LevelDigitalPort;
		boolean AllowSendCommand = true;
		byte NumberChannel = EEPROM.read(E_BindExecModile + InputFromSerial0[1]);		// ������ ����� ������ � ������� �������� ������
		byte TypeControllChannel = EEPROM.read(E_Controll_Channel + NumberChannel);		// ������ ��� ���������� �������
		if(TypeControllChannel != 0){		// ���� ������ � ������ ��������������� ����������
			AllowSendCommand = false;		// ������ ������ � ������ ��������� �������, ����� ������� ��������� ������
											// � ����� ������� ���������� ��� ��������� �� ��� ������ �������������� ������
		}	
	
		if(AllowSendCommand){				// ���� ��������� ���������� ������� ���������� ������
			byte Level = map(InputFromSerial0[3], 0, 100, 0, EEPROM.read(E_MaxLimitRotation + InputFromSerial0[1]));
			switch(InputFromSerial0[0]){
				case 1:
					break;
				case 2:
					break;
				case 3:
					switch(EEPROM.read(E_TypeExecModule + InputFromSerial0[1])){		// �� ���� ������ ���������� ��� ���������
						case 1:											// --- ������� �����
							break;
						case 2:											// --- ���
							RunPWMonPort(EEPROM.read(E_PortExecModule + InputFromSerial0[1]), Level);
							break;
						case 3:											// --- ����� �����
							RunServoMotor(InputFromSerial0[1], Level);
							break;
						case 4:											// --- Digitall port
							if(InputFromSerial0[3] <= 0){			// ���� ������� ����������� ���������� ������ "0"
								LevelDigitalPort = 1;
							}
							else LevelDigitalPort = 0;
						
							DigitalPort(EEPROM.read(E_PortExecModule + InputFromSerial0[1]), LevelDigitalPort, 2);		
							break;
						case 5:											// ������������ �����
							break;
					}
					break;
			}
		}
	}
	
// 	for (byte i = 0; i < sizeof(InputFromSerial0) / sizeof(int); i ++){			// �������� ������ ����� ������
// 		InputFromSerial0[i] = 0;
// 	}
	CleanInputFromSerial0();
	recievedFlag_date = false;
}


//map(InputFromSerial0[3], 0, 100, 0, EEPROM.read(E_MaxLimitRotation + InputFromSerial0[1]));
/*

x = 100 * (4,2 - InputFromSerial0[3]) / (4,2 - 2,5)


x = 100 * (4,2 - y) / (4,2 - 2,5)

��� x - ������� ��������, � y - ����� � ��������� �� 2,5 �� 4,2.

	void RunStepperMotor(byte Module, int Step, byte Mode);
	*/