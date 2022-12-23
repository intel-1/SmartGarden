#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "lib\iarduino_RTC.h"


extern iarduino_RTC time;

extern int analogValue;

extern unsigned int T_second;
extern byte TimerTsecond;
extern byte T_min;							
extern byte T_hour;						
extern unsigned int T_day;							

extern int UpStepValue[];

extern unsigned int LoopOffBluetoothTime;			// Выключение Bluetooth модуля
extern unsigned int LoopCheckRegistrationGSM;		// Интервал проверки регистрации GSM
extern boolean RegistrationGSM;						// Флаг что отправлена команда для проверки регистрации GSM
extern int QuantityPacketGPRS;						// Счетчик количества отправок по GPRS
extern int QuantityPacketGPRSundoError;				// Счетчик количества отправок по GPRS до зависания

extern unsigned int LoopCheckRegistrationGSM;		// Интервал проверки регистрации GSM

extern unsigned int LoopReadInternalTemp;			// Интервал измерения встроеный LM75

extern unsigned int LoopCheckIntervalGPRS;			// Интервал проверки регистрации GPRS


void TimeIntervals();
void ManagementVCC();
void MainFunc();
void TermostatFunc(byte _NumberChannel);
void TimerFunc();
void Turning_Backlight_LCD();
void CleanTimeIntervals();
boolean ArchiveRestoreSaveWordData(byte _Type);

#endif