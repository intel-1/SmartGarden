#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <Wire.h>


#include "Configuration.h"


#include "SensorsFuncs\Work_BMP280.h"
#include "SensorsFuncs\Work_BME280.h"
#include "SensorsFuncs\Work_INA219.h"
#include "SensorsFuncs\Work_HTU21D.h"
#include "SensorsFuncs\Work_AM2302.h"
#include "SensorsFuncs\Work_DS18B20.h"
#include "SensorsFuncs\Work_TSL2561.h"

#include "SensorsFuncs\Work_BH1750.h"
#include "SensorsFuncs\Work_INA3221.h"
#include "SensorsFuncs\Work_LM75.h"
#include "SensorsFuncs\Work_MAX44009.h"



extern OneWire oneWire;


extern int LoopTimeRunCalculateSensor[QuantitySensors + 1];			// Временные интервалы измерения сенсоров

extern float CfCalcDC;										// Поправочные коэфициенты для вычисления VCC
extern float VCC;											// Текущее напряжение питания
extern float Ti;											// Температура встроенного температурного датчика (LM75A) 
extern boolean DebugRepet_1;								// Повторять ли вывод в консоль или нет
extern int RealValueADC[QuantitySensors + 1];				// Текущие значения аналоговых портов
extern float RealValueSensors[QuantitySensors + 1][3];		// Текущие значения датчиков
extern float OldValueSensors[QuantitySensors + 1][3];		// Старые значения датчиков (нужны для запуска мониторинга групп, сравнивается с текущими и если различаются, запускается мониторинг)
extern byte SensorsError[QuantitySensors + 1][3];			// Ошибки датчиков
extern float Buffer_Value_Sensors[QuantitySensors + 1][3];	// "Сырые" значения датчиков

extern struct StructQuantityCalcSensors{
	float SumValue[QuantitySensors + 1][3];					// Суммы измеренных значений датчиков ()
	byte QuantityCalc[QuantitySensors + 1];					// Счетчик количество измеренных значений датчиков
	//bool AllowSummData[QuantitySensors];					// Флаг что данные успешно суммированы и нет никаких ошибок измерений
} QuantityCalcSensors;




void ControllPort(byte NumberSensor, byte Controll);
//boolean UpDownControllPort(byte NumberSensor, boolean Controll);
void ViewValueAllSensors();
void CalculateSensors();
void i2c_scaner(boolean LogView);

#endif
