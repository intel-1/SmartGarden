#ifndef WARNING_H
#define WARNING_H

#include <Arduino.h>
#include <stdint.h>

#include "Configuration.h"

extern boolean GlobalError;             // Глобальная ошибка, работать дальше не возможно!!!!

extern boolean ErrorCloseWindow_1;      // Ошибка закрытия первого окна
extern boolean ErrorCloseWindow_2;      // Ошибка закрытия второго окна
extern boolean ErrorCloseWindow_3;      // Ошибка закрытия третьего окна
extern byte NumberStepForCloseWindow;	// Число мм для закрытия окна когда температура меньше минимальной (3 см)

extern boolean ErrorSDcard;             // Ошибка работы с SD картой

extern boolean HumidityAirOffline;      // Датчик влажность Offline 

extern boolean ErrorTempLeft;      // Ни одна температура левой теплицы не известна
extern boolean ErrorTempRight;     // Ни одна температура правой теплицы не известна
extern boolean ErrorTempLeftUp;
extern boolean ErrorTempLeftDown;
extern boolean ErrorTempBox;

//---------- Ошибки напряжения питания ---------------
extern boolean Low_Input_VCC;             // Напряжение ниже минимального, отключается управление окнами

//----------- Warning рабочих температур --------------  
extern boolean WarningMaxTempLeft;           // Превышена максимальная температура левой теплицы
extern boolean WarningMaxTempRight;          // Превышена максимальная температура правой теплицы
extern boolean WarningMinTempLeft;
extern boolean WarningMinTempRight;

extern unsigned long LoopFrequencyLight5;    
extern unsigned long LoopFrequencyLight6;    

extern byte PostCode;

void StatusLED(byte Warning);
//void Warning();

#endif
