#include <Arduino.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include "ExecModules.h"
#include "DigitalPorts.h"
#include "lib\Servo.h"
#include "lib\AccelStepper.h"
#include "Warning.h"
#include "EEPROM_ADR.h"
#include "LCDdisplay.h"



Servo Servo;
AccelStepper Stepper(1, STEPPER_STEP, STEPPER_DIR);      // step � dir, 1 - ����� "external driver" (A4988)


int PassableSteperMotor[16];
boolean StepperMotorRun = false;
boolean AllowRunMotor = true;		// ��������� ����������. ���� false, �� ����� ������� �� ������

byte Stepper_Space = 0;				// ���������� ��� ������ ��������� �� ������ LCD �� ����� �������� �����������


boolean InitializingExecModule(byte _ExecModule){								// �-��� ������������� �������������� �������
	byte SwitchPort = EEPROM.read(E_LowSwitchPortModule + _ExecModule);			// ���� Down ���������
	byte MaxQuantityStep;														// ������������ �������� �������� ������
	byte Step = 0;
	byte PortExecModule = EEPROM.read(E_PortExecModule + _ExecModule);			// ���� ������
	bool _Error = false;
	switch(EEPROM.read(E_TypeExecModule + _ExecModule)){						// ���������� ��� ������
		case 1:																	// ������� �����			
			PassableSteperMotor[_ExecModule] = 0;								// �������� ������ ���������� ����� ������			
			MaxQuantityStep = EEPROM.read(E_MaxLimitRotation + _ExecModule);	// ������������ ���-�� �� ��� �������� ������			
			if(!ReadInputDigitalPorts(SwitchPort)){								// ��������� �� ������� �� ��������				
				while(!ReadInputDigitalPorts(SwitchPort)){
				//while(!StatusSwitchPorts[SwitchPort - 40]){						// ���� �� ��������� �������� DOWN
					if(Step <= MaxQuantityStep){
						Step ++;
						RunStepperMotor(_ExecModule, -1, 1);
					}
					else{ 
						_Error = true;											// ���� �� �������� �������� ��� �������� �� ���������
						goto ExitWhile;
					}
				}
			}
			ExitWhile:
			Stepper_Space = 0;
			StatusSwitchPorts[SwitchPort - 30] = false;					// �������� ��������� ���������
			PassableSteperMotor[_ExecModule] = 0;						// �������� ���������� ���-�� �����
			return _Error;
			break;
		case 2:															// PWM
			RunPWMonPort(PortExecModule, 0);
			return true;
			break;
		case 3:															// ����� �����
			RunServoMotor(PortExecModule, 0);
			return true;
			break;
		case 4:															// Digital port
			DigitalPort(PortExecModule, DIGITAL_PORT_OFF, DIGITAL_PORT_SWITCH_PORT, NO_LOG_TO_UART);	// ��������� ����
			return true;
			break;
		case 5:															// ����������� �����
			if(EEPROM.read(E_TypeHighSwitchModule + _ExecModule) == 2){	
				
			}
			return true;
			break;
	}
	return _Error;
}


void RunPWMonPort(byte Port, int LevelPWM){
	if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
		Serial.print(F("\t\t\t\t...Run PWM on port ")); DigitalPort(Port, DIGITAL_PORT_OFF, DIGITAL_PORT_RETURN_NAME_PORT, NO_LOG_TO_UART); Serial.print(F(". Signal Level: ")); Serial.print(LevelPWM); Serial.println(F("*"));
	}
	digitalWrite(DigitalPort(Port, DIGITAL_PORT_OFF, DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT, NO_LOG_TO_UART), LevelPWM);
}


void RunServoMotor(byte Module, byte AngleRotation){
/*
	Port - ����� �����
	AngleRotation - ���� �������� �����
*/	
	byte Port = EEPROM.read(E_PortExecModule + Module);
	byte ArduinoPort = DigitalPort(Port, DIGITAL_PORT_OFF, DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT, NO_LOG_TO_UART);	// �������� ����� ����������� ����� � Arduino �������
	if(ArduinoPort != 255){																			// ���� ���� ��������������� (�������� �������� �� 255)
		if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
			Serial.print(F("\t\t\t\t\t\t\t...Run Servo motor on "));
			DigitalPort(Port, 0, 1, NO_LOG_TO_UART); Serial.print(F(" (")); Serial.print(DigitalPort(Port, DIGITAL_PORT_OFF, DIGITAL_PORT_RETURN_ARDUINO_NAME_PORT, NO_LOG_TO_UART)); Serial.print(F(")"));
			Serial.print(F(". Angle Rotation: ")); Serial.print(AngleRotation); Serial.println(F("*"));
		}
		if(EEPROM.read(E_ManualModeModule + Module) == 1){		// ���� ������� ������ ����� ��������� �����
			int MinImp = EEPROM.read(E_Servo_MinImp + Module);	// ���������� �������� �������� ��� �������� �� 0*
			int MaxImp = EEPROM.read(E_Servo_MaxImp + Module);	// --//--//--//-- ��� �������� �� 180*
			//Servo.attach(ArduinoPort, MinImp, MaxImp);		// ���������� ���� ��� ������� ����������
		}
		else Servo.attach(ArduinoPort);							// �������������� ����. ����� ����� �������� � Arduino �������
		if(Servo.attached()){									// ���� ���� ���������
			if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
				Serial.println(F("\t\t\t\t\t\t\t\t...port is initializing"));
			}
			//Servo.write(AngleRotation);					// ��������� �����������
			if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
				Serial.println(F("\t\t\t\t\t\t\t\t...movement is started"));
			}
			if(Servo.read() == AngleRotation){				// � ���� ���������� ��������� ����� AngleRotation
				if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
					Serial.println(F("\t\t\t\t\t\t\t\t...movement is done!!!"));
				}
			}
			else{
				if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
					Serial.println(F("\t\t\t\t\t\t\t\t...movement is error!!!"));
				}
			}
			Servo.detach();			// ��������� ����
		}
	}
	else{
		if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
			Serial.println(F("\t\t\t\t\t...The port isn't configured"));
		}
	}
}


void ViewStateSwitch(byte Level){
	switch(Level){
		case 0:
			Serial.println(F("off"));
			break;
		case 1:
			Serial.println(F("on"));
			break;
		default:
			Serial.println();
	}
}


void RunStepperMotor(byte Module, int Step, byte Mode){
/*
	Module - ����� ��������������� ������
	Step - ���������� ����� ��� �����������
	Mode - ������� ������ ��� "��������" ������
*/	
	Stepper.setCurrentPosition(0);
	int setAcceleration = EEPROM_int_read(E_StepperMotor_setAcceleration + Module * 2);
	int setMaxSpeed = EEPROM_int_read(E_StepperMotor_setMaxSpeed + Module * 2);	 	
	if(ControllerSetup){						// ���� ���������� � ������ Setup 
		Serial.print(F("."));
		WriteToLCD(String(F("*")), LCD_LINE_4, LCD_START_SYMBOL_2 + Stepper_Space, LCD_NO_SCREEN_REFRESH_DELAY);
		Stepper_Space ++;
 	}
	if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
		
		byte StateSwitch;
		byte SwitchPort;
		if(EEPROM.read(E_LowSwitchPortModule + Module) != 0){			// ���� �������� DOWN ��������
			Serial.print(F("\t\t\t\t\t\t\t...DOWN switch (")); 
			SwitchPort = EEPROM.read(E_LowSwitchPortModule + Module);	
			ViewNameInputDigitalPorts(SwitchPort);						// ������� ��� ����� ���������
			//ReadInputDigitalPorts(SwitchPort); 						// ������� ��� ����� ���������
			Serial.print(F("): ")); 
			StateSwitch = ReadInputDigitalPorts(SwitchPort);			// ��������� ��������� ���������
			ViewStateSwitch(StateSwitch);								// ������� ��������� ��������� � Serial
		}
		if(EEPROM.read(E_HighSwitchPortModule + Module) != 0){			// ���� �������� UP ��������
			Serial.print(F("\t\t\t\t\t\t\t...UP switch("));				// ���� ���, �� ������� ��� ��� � ������ ���
			SwitchPort = EEPROM.read(E_HighSwitchPortModule + Module);
			ViewNameInputDigitalPorts(SwitchPort);						// ������� ��� ����� ���������
			//ReadInputDigitalPorts(SwitchPort);						// ������� ��� ����� ���������
			Serial.print(F("): "));										
			StateSwitch = ReadInputDigitalPorts(SwitchPort);			// ��������� ��������� ���������
			ViewStateSwitch(StateSwitch);								// ������� ��������� � Serial
		}
		Serial.print(F("\t\t\t\t\t\t\t...driver stepper motor on port "));
		DigitalPort(EEPROM.read(E_PortExecModule + Module), DIGITAL_PORT_OFF, DIGITAL_PORT_RETURN_NAME_PORT, NO_LOG_TO_UART);		// ������� ��� �����
	}
	// =============================================================
	// �������� ������� � ������� ���� ���-�� ����� ������� �� ����
	// =============================================================
	if(Step != 0){		
		STEPPER_VCC_on();																	// ������ �������												
		DigitalPort(EEPROM.read(E_PortExecModule + Module), DIGITAL_PORT_OFF, DIGITAL_PORT_SWITCH_PORT, NO_LOG_TO_UART);	// ��������� ������� �������� ������ ������� �� �����
		if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
			Serial.println(F(" it is started"));
		}
	}
	else{
		if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
			Serial.println(F(" it is stopped"));
		}
	}
	if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
		Serial.print(F("\t\t\t\t\t\t\t...Step = ")); Serial.print(Step); Serial.println(F("cm"));  
	}
	// =================================================================================================
	// ������������� �������� � ��������� ������ (����� default ���� ��� ������� �� ������� �����������)
	// =================================================================================================
	if(setAcceleration > 0 && setAcceleration <= 10000){												// ���� ��������� setAcceleration � ����������� ���������
		Stepper.setAcceleration(setAcceleration * EEPROM.read(E_StepperMotor_DividerStep + Module));	// ��������� �� � �������� ������� ��������� �� ��������
	}
	else{ 
		if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
			Serial.println(F("\t\t\t\t\t\t\t...Use default setAcceleration = 1000"));
		}
		Stepper.setAcceleration(1000);															// ����� ��������� default ���������
	}
	if(setMaxSpeed > 0 && setMaxSpeed <= 10000){												// ���� ��������� setMaxSpeed � ����������� ���������
		Stepper.setMaxSpeed(setMaxSpeed * EEPROM.read(E_StepperMotor_DividerStep + Module));	// ��������� �� � �������� ������� ��������� �� ��������
	}
	else{ 
		if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
			Serial.println(F("\t\t\t\t\t\t\t...Use default setMaxSpeed = 500"));
		}
		Stepper.setMaxSpeed(500);											// ����� ��������� default ���������
	}	
	// ============================================================================
	// ============= �������� ��������� ������� ������ ============================
	// ============================================================================
	long StepperMotor_Step = EEPROM_int_read(E_StepperMotor_Step + Module * 2) * EEPROM.read(E_StepperMotor_DividerStep + Module);	// ���������� ����� �� ���� ��������� ��������� �� �������� ����
	long MotorStep = StepperMotor_Step * ((long)Step);						// ���������� ����� ��� �����������
		
	if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
		Serial.print(F("\t\t\t\t\t\t\t...Quantity of steps of the motor = ")); Serial.println(MotorStep);
	}
	if(Step != 0){	
		StepperMotorRun = true;					// ����� ������� 
		Stepper.move(MotorStep);				// ������������� ��������� �����������
		//Stepper.runToPosition();				// ������ �������� ��������� (����������� �-���, �������� ���� �� ����������� ���������)
		long Steps;
		while(Stepper.run()){					// ���� �� ������ ��� ����������� ��� �� ��������� ��������� ������
			wdt_reset();						// ���������� ������
			Steps ++;							// ������� ���������� �����
			if (Steps > MotorStep){				// ������!!!!! ���� ����� ���������� ����� ��������� ������ �������
			}
		}
		AllowRunMotor = true;
		PassableSteperMotor[Module] += Step;	// ��������� ���������� ���-�� ����� (����� ��� ����������� � Serial)
		StepperMotorRun = false;				// ����� ������� 
	}
	// ============================================================================
	if(Mode != 1){								// ���� �������� �� � ������ ������������� (��������) ������
		if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
			Serial.print(F("\t\t\t\t\t\t\t...Passable quantity of steps = ")); Serial.println(PassableSteperMotor[Module]);
		}
	}
	if(Step != 0){														// ���� ��������� �����
		if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
			Serial.print(F("\t\t\t\t\t\t\t...Hardware quantity of the passable steps = ")); Serial.println(Stepper.currentPosition() / MotorStep);
		}
	}
	DigitalPort(EEPROM.read(E_PortExecModule + Module), DIGITAL_PORT_ON, DIGITAL_PORT_SWITCH_PORT, NO_LOG_TO_UART);	// ������������� ������� �������� ������� ������� �� �����
	// -------------------------------------------------------------------------------------
	if(Step != 0){														// ���� �� ��������� ����� �� � ������� ��������� �� �����
		if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
			Serial.println(F("\t\t\t\t\t\t\t...driver stepper motor is stop"));
		}
		STEPPER_VCC_off();																				// ��������� �������	
		DigitalPort(EEPROM.read(E_PortExecModule + Module), DIGITAL_PORT_ON, DIGITAL_PORT_SWITCH_PORT, NO_LOG_TO_UART);	// ������������� ������� �������� ������� ������� �� �����
	}
}