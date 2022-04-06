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



Servo Servo;
AccelStepper Stepper(1, STEPPER_STEP, STEPPER_DIR);      // step и dir, 1 - режим "external driver" (A4988)


int PassableSteperMotor[16];
boolean StepperMotorRun = false;
boolean AllowRunMotor = true;		// Аварийная переменная. Если false, то снято питания на моторе

boolean InitializingExecModule(byte Module){							// Ф-ция инициализации исполнительных модулей
	byte SwitchPort = EEPROM.read(E_LowSwitchPortModule + Module);		// Порт Down концевика
	byte MaxQuantityStep;												// Максимальная величина открытия модуля
	byte Step = 0;
	byte PortExecModule = EEPROM.read(E_PortExecModule + Module);		// Порт модуля
	switch(EEPROM.read(E_TypeExecModule + Module)){						// Определяем тип модуля
		case 1:															// Шаговый мотор
			PassableSteperMotor[Module] = 0;							// Обнуляем массив пройденных шагов мотора			
			MaxQuantityStep = EEPROM.read(E_MaxLimitRotation + Module);	// Максимальное кол-во см для закрытия модуля			
			if(!ReadInputDigitalPorts(SwitchPort)){						// Проверяем не закрыта ли форточка
				while(!StatusSwitchPorts[SwitchPort - 30]){				// Пока не сработает концевик DOWN
					if(Step <= MaxQuantityStep){
						Step ++;
						RunStepperMotor(Module, -1, 1);
					}
					else break;
				}
			}
			StatusSwitchPorts[SwitchPort - 30] = false;					// Обнуляем состояние концевика
			PassableSteperMotor[Module] = 0;							// Обнуляем пройденное кол-во шагов
			return true;
			break;
		case 2:															// PWM
			RunPWMonPort(PortExecModule, 0);
			return true;
			break;
		case 3:															// Серво мотор
			RunServoMotor(PortExecModule, 0);
			return true;
			break;
		case 4:															// Digital port
			DigitalPort(PortExecModule, 0, 2);							// Выключаем порт
			return true;
			break;
		case 5:															// Колекторный мотор
			if(EEPROM.read(E_TypeHighSwitchModule + Module) == 2){	
				
			}
			return true;
			break;
	}
}


void GSM_module(){
	
}

void RunPWMonPort(byte Port, int LevelPWM){
	if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
		Serial.print(F("\t\t\t\t...Run PWM on port ")); DigitalPort(Port, 0, 1); Serial.print(F(". Signal Level: ")); Serial.print(LevelPWM); Serial.println(F("*"));
	}
	digitalWrite(DigitalPort(Port, 0, 3), LevelPWM);
}


void RunServoMotor(byte Module, byte AngleRotation){
/*
	Port - номер порта
	AngleRotation - угол поворота сервы
*/	
	byte Port = EEPROM.read(E_PortExecModule + Module);
	byte ArduinoPort = DigitalPort(Port, 0, 3);					// Получаем номер управлящего порта в Arduino формате
	if(ArduinoPort != 255){										// Если порт сконфигурирован (значение отличное от 255)
		if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
			Serial.print(F("\t\t\t\t\t\t\t...Run Servo motor on "));
			DigitalPort(Port, 0, 1); Serial.print(F(" (")); Serial.print(DigitalPort(Port, 0, 3)); Serial.print(F(")"));
			Serial.print(F(". Angle Rotation: ")); Serial.print(AngleRotation); Serial.println(F("*"));
		}
		if(EEPROM.read(E_ManualModeModule + Module) == 1){		// Если включен ручной режим настройки сервы
			int MinImp = EEPROM.read(E_Servo_MinImp + Module);	// Выставляем величину импульза для поворота на 0*
			int MaxImp = EEPROM.read(E_Servo_MaxImp + Module);	// --//--//--//-- для поворона на 180*
			//Servo.attach(ArduinoPort, MinImp, MaxImp);		// Инициируем порт для ручного управления
		}
		else Servo.attach(ArduinoPort);							// Инициализируем порт. Номер порта получаем в Arduino формате
		if(Servo.attached()){									// Если порт подключен
			if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
				Serial.println(F("\t\t\t\t\t\t\t\t...port is initializing"));
			}
			//Servo.write(AngleRotation);					// Запускаем перемещение
			if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
				Serial.println(F("\t\t\t\t\t\t\t\t...movement is started"));
			}
			if(Servo.read() == AngleRotation){				// и если пройденное растояние равно AngleRotation
				if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
					Serial.println(F("\t\t\t\t\t\t\t\t...movement is done!!!"));
				}
			}
			else{
				if(OUTPUT_LEVEL_UART_MODULE_BESIDES_SETUP){
					Serial.println(F("\t\t\t\t\t\t\t\t...movement is error!!!"));
				}
			}
			Servo.detach();			// Отключаем порт
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
			Serial.println(F(""));
	}
}


void RunStepperMotor(byte Module, int Step, byte Mode){
/*
	Module - Номер исполнительного модуля
	Step - Количество шагов для перемещения
	Mode - штатная работа или "закрытия" модуля
*/	
	Stepper.setCurrentPosition(0);
	int setAcceleration = EEPROM_int_read(E_StepperMotor_setAcceleration + Module * 2);
	int setMaxSpeed = EEPROM_int_read(E_StepperMotor_setMaxSpeed + Module * 2);
 	if(ControllerSetup){						// Если контроллер в стадии Setup 
 		Serial.print(F("."));
 	}
	if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
		byte StateSwitch;
		byte SwitchPort;
		if(EEPROM.read(E_LowSwitchPortModule + Module) != 0){			// Если добавлен DOWN концевик
			Serial.print(F("\t\t\t\t\t\t\t...DOWN switch (")); 
			SwitchPort = EEPROM.read(E_LowSwitchPortModule + Module);	
			ViewNameInputDigitalPorts(SwitchPort);						// Выводим имя порта концевика
			//ReadInputDigitalPorts(SwitchPort); 						// Выводим имя порта концевика
			Serial.print(F("): ")); 
			StateSwitch = ReadInputDigitalPorts(SwitchPort);			// Считываем состояние концевика
			ViewStateSwitch(StateSwitch);								// Выводим состояние концевика в Serial
		}
		if(EEPROM.read(E_HighSwitchPortModule + Module) != 0){			// Если добавлен UP концевик
			Serial.print(F("\t\t\t\t\t\t\t...UP switch("));				// Если нет, то выводим его как в первый раз
			SwitchPort = EEPROM.read(E_HighSwitchPortModule + Module);
			ViewNameInputDigitalPorts(SwitchPort);						// Выводим имя порта концевика
			//ReadInputDigitalPorts(SwitchPort);						// Выводим имя порта концевика
			Serial.print(F("): "));										
			StateSwitch = ReadInputDigitalPorts(SwitchPort);			// Считываем состояние концевика
			ViewStateSwitch(StateSwitch);								// Выводим состояние в Serial
		}
		Serial.print(F("\t\t\t\t\t\t\t...driver stepper motor on port "));
		DigitalPort(EEPROM.read(E_PortExecModule + Module), 0, 1);		// Выводим имя порта
	}
	// =============================================================
	// Включаем драйвер и питание если кол-во шагов отлично от нуля
	// =============================================================
	if(Step != 0){		
		STEPPER_VCC_on();												// Подаем питание												
		DigitalPort(EEPROM.read(E_PortExecModule + Module), 0, 2);		// Запускаем драйвер шаговика низким уровнем на порту
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
	// Устанавливаем скорость и ускорение работы (берем default если они выходят за границы разрешенных)
	// =================================================================================================
	if(setAcceleration > 0 && setAcceleration <= 10000){											// Если настройка setAcceleration в разрешенном диапазоне
		Stepper.setAcceleration(setAcceleration * EEPROM.read(E_StepperMotor_DriverStep + Module));	// Принимаем ее в качестве рабочей умноженой на делитель
	}
	else{ 
		if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
			Serial.println(F("\t\t\t\t\t\t\t...Use default setAcceleration = 1000"));
		}
		Stepper.setAcceleration(1000);														// Иначе применяем default настройки
	}
	if(setMaxSpeed > 0 && setMaxSpeed <= 10000){											// Если настройка setMaxSpeed в разрешенном диапазоне
		Stepper.setMaxSpeed(setMaxSpeed * EEPROM.read(E_StepperMotor_DriverStep + Module));	// Принимаем ее в качестве рабочей умноженой на делитель
	}
	else{ 
		if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
			Serial.println(F("\t\t\t\t\t\t\t...Use default setMaxSpeed = 500"));
		}
		Stepper.setMaxSpeed(500);								// Иначе применяем default настройки
	}	
	// ============================================================================
	// ============= Основные процедуры запуска мотора ============================
	// ============================================================================
	long StepperMotor_Step = EEPROM_int_read(E_StepperMotor_Step + Module * 2) * EEPROM.read(E_StepperMotor_DriverStep + Module);
	long MotorStep = StepperMotor_Step * ((long)Step);				// Количество шагов для перемещения
	if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
		Serial.print(F("\t\t\t\t\t\t\t...Quantity of steps of the motor = ")); Serial.println(MotorStep);
	}
	if(Step != 0){	
		StepperMotorRun = true;					// Мотор запущен 
		Stepper.move(MotorStep);				// Устанавливает следующее перемещение
		//Stepper.runToPosition();				// Запуск шагового двигателя (Блокирующая ф-ция, работает пока не остановился двигатель)
		long Steps;
		while(Stepper.run()){					// Пока не прошли все перемещение или не сработала аварийная защита
 			wdt_reset();						// Сбрасываем собаку
			Steps ++;							// Счетчик пройденных шагов
			if (Steps > MotorStep){				// ЗАЩИТА!!!!! Если число пройденных шагов оказалось больше нужного
				
			}
		}
		AllowRunMotor = true;
		PassableSteperMotor[Module] += Step;	// Сохраняем пройденное кол-во шагов (нужно для отображения в Serial)
		StepperMotorRun = false;				// Мотор запущен 
	}
	// ============================================================================
	if(Mode != 1){								// Если работаем не в режиме инициализации (закрытия) модуля
		if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
			Serial.print(F("\t\t\t\t\t\t\t...Passable quantity of steps = ")); Serial.println(PassableSteperMotor[Module]);
		}
	}
	if(Step != 0){														// Если запускали мотор
		if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
			Serial.print(F("\t\t\t\t\t\t\t...Hardware quantity of the passable steps = ")); Serial.println(Stepper.currentPosition() / MotorStep);
		}
	}
	// -------------------------------------------------------------------------------------
	if(Step != 0){														// Если не запускали мотор то и драйвер выключать не нужно
		if(OUTPUT_LEVEL_UART_MODULE_AND_SETUP){
			Serial.println(F("\t\t\t\t\t\t\t...driver stepper motor is stop"));
		}
		STEPPER_VCC_off();												// Выключаем питание	
		DigitalPort(EEPROM.read(E_PortExecModule + Module), 1, 2);		// Останавливаем драйвер шаговика высоким уровнем на порту
	}
}