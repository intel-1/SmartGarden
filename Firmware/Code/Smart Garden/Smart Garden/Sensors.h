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



// ========================================================================================
// ================================= Наименования датчиков ================================
// ========================================================================================
#define S_DS18B20 1
#define S_AM2302 2
#define S_SI7013 3
#define S_SI7020 3
#define S_SI7021 3
#define S_HTU21D 3
#define S_SHT21 3
#define S_BME280 4
#define S_BMP280 5
#define S_INA219 6
#define S_ANALOG_SENSOR 7
#define S_TSL2561 8
#define S_BH1750 9
#define S_MAX44009 10
#define S_LM75 11
#define S_INA3221 12

// ========================================================================================
// ================================ Типы измеряемых данных ================================
// ========================================================================================
#define TEMP_AIR_VALUE 1			// Температура воздуха
#define HUMM_AIR_VALUE 2			// Влажности воздуха
#define HUMM_SOIL 3					// Влажности почвы
#define LIGHT 4						// Освещение
#define ATMOSPHERIC_PRESSURE 5		// Атмосферное давление
#define TENSION 6					// Напряжение
#define CURRENT 7					// Ток
#define ANALOG_VALUE 8				// Аналоговое значение
#define HUMM_AND_TEMP_VALUE 12		// Температура и влажность воздуха
#define TEMP_AIR_AND_PRESS	15		// Температура и атмосферное давление
#define TEMP_PRESS_AND_HUMM_AIR 125	// Температура, влажность и атмосферное давление
#define TENS_And_CURR 67			// Напряжение и ток




extern OneWire oneWire;

extern int LoopTimeRunCalculateSensor[QUANTITY_SENSORS + 1];	// Временные интервалы измерения сенсоров

extern float CfCalcDC;											// Поправочные коэфициенты для вычисления VCC
extern float VCC;												// Текущее напряжение питания
extern float Ti;												// Температура встроенного температурного датчика (LM75A) 
extern boolean DebugRepet_1;									// Повторять ли вывод в консоль или нет

extern int RealValueADC[QUANTITY_SENSORS + 1];					// Текущие значения аналоговых портов
extern float RealValueSensors[QUANTITY_SENSORS + 1][3];			// Текущие значения датчиков
extern float OldValueSensors[QUANTITY_SENSORS + 1][3];			// Старые значения датчиков (нужны для запуска мониторинга групп, сравнивается с текущими и если различаются, запускается мониторинг)
extern byte SensorsError[QUANTITY_SENSORS + 1][3];				// Ошибки датчиков


extern struct StructQuantityCalcSensors{
	float SumValue[QUANTITY_SENSORS + 1][3];					// Суммы измеренных значений датчиков (сырые)
	byte QuantityCalc[QUANTITY_SENSORS + 1];					// Счетчик количество измеренных значений датчиков
	//bool AllowSummData[QuantitySensors];						// Флаг что данные успешно суммированы и нет никаких ошибок измерений
} QuantityCalcSensors;




void ControllPort(byte NumberSensor, byte Controll);
//boolean UpDownControllPort(byte NumberSensor, boolean Controll);
void ViewValueAllSensors();
void CalculateSensors();
void i2c_scaner(boolean LogView);

void React_to_Error_Calculate_Value(byte NumberSensor,byte TypeMeasurement, byte TypeValue);

#endif
