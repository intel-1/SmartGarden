#ifndef EXECMODULE_H
#define EXECMODULE_H

#include <Arduino.h>

extern boolean AllowRunMotor;			// Аварийная переменная. Если false, то снято питания на моторе
extern int PassableSteperMotor[];
extern boolean StepperMotorRun;

boolean InitializingExecModule(byte NumberModule);
void GSM_module();
void RunServoMotor(byte Module, byte AngleRotation);
void RunPWMonPort(byte Port, int LevelPWM);
void RunStepperMotor(byte Motor, int Step, byte Mode);

#endif
