
#ifndef GPRS_H
#define GPRS_H


#include <stdint.h>
#include <Arduino.h>
#include "Configuration.h"


void InitializingGPRS();

bool ConnectionGPRS();						// �������� ����������� GPRS

void SendGETGPRS(String Text);				// �������� GET ������� � �� �� GPRS


#endif