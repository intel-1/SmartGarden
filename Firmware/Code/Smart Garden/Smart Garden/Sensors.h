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
#include "SensorsFuncs\Work_AHT25.h"




extern OneWire oneWire;

extern int LoopTimeRunCalculateSensor[QUANTITY_SENSORS + 1];	// Временные интервалы измерения сенсоров

extern float VCC;												// Текущее напряжение питания
extern float Ti;												// Температура встроенного температурного датчика (LM75A) 
extern boolean DebugRepet_1;									// Повторять ли вывод в консоль или нет


extern struct StructSensors{
	int PresentValueADC[QUANTITY_SENSORS + 1];			// Текущие значения аналоговых портов
	float PresentValue[QUANTITY_SENSORS + 1][3];		// Текущие значения датчиков
	float OldValue[QUANTITY_SENSORS + 1][3];			// Прошлые значения датчиков (нужны для запуска мониторинга групп, сравнивается с текущими и если различаются, запускается мониторинг)
	float BufferSumValue[QUANTITY_SENSORS + 1][3];		// Суммы измеренных значений датчиков (сырые)
	byte MeasurementCounter[QUANTITY_SENSORS + 1];		// Счетчик количество измеренных показаний датчиков
	bool Error_Value[QUANTITY_SENSORS + 1][3];			// Ошибки показаний датчиков
} Sensors;



extern struct StructBuferValueSensors{ 
	float Value[3];
	bool Allow[3];
} BuferValueSensors;



void ControllPort(byte NumberSensor, byte Controll);
//boolean UpDownControllPort(byte NumberSensor, boolean Controll);
void ViewValueAllSensors();
void CalculateSensors();
void Recording_Sensor_Readings(byte NumberSensor);		// Актуализация измеренных показаний в зависимости от количества измерений
void i2c_scaner(boolean LogView);

void React_to_Error_Calculate_Value(byte NumberSensor,byte TypeMeasurement, byte Value);

#endif
