#ifndef PARSING_DATA_ON_SD_H
#define PARSING_DATA_ON_SD_H
#include <stdint.h>
#include <Arduino.h>


/*
  Пример пакета: $110 25 600 920;  будет раскидан в массив по порядку слева направо
*/


//void ParsingDataFromSerial();

void ProcessingDataFromSerial();
//void Read_E_Type_A_Sensor_register(byte Sensor);
//void Read_E_Type_B_Sensor_register(byte Sensor);
void CleaningDuplicatedPorts(int Value);


extern int InputFromSerial0[50];

extern boolean recievedFlag_date;
extern boolean transmitFlag_date;
extern boolean recievedFlag_intervals;
extern boolean recievedFlag_config_controller;
extern boolean recievedFlag_config_module;
extern boolean recievedFlag_debug;
extern boolean recievedFlag_channel;
extern boolean recievedFlag_sensors;
extern boolean getStarted;

extern byte NumberUARTPort;			// Номер Serial порта чтобы отличать откуда пришла комманда
extern boolean RecievedDataFromUART;

//void Reboot();					// Сброс контроллера
void jmp_bootloader();
void CleanInputFromSerial0();

#endif
