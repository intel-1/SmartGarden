
#ifndef GPRS_H
#define GPRS_H


#include <stdint.h>
#include <Arduino.h>
#include "Configuration.h"


void InitializingGPRS();

bool ConnectionGPRS();						// Проверка регистрации GPRS

void SendGETGPRS(String Text);				// Отправка GET пакетов в БД по GPRS


#endif