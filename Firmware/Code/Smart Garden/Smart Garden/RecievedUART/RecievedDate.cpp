#include <./Arduino.h>
#include <./EEPROM.h>

#include "RecievedDate.h"
#include "../ParsingDataOnSerial.h"
#include "../EEPROM_ADR.h"
#include "../main.h"

// ==============================================================================
// =========================== Дата и время =====================================
// ==============================================================================
void RecievedDate(){
	boolean flag;
	for (byte i=0; i<sizeof(InputFromSerial0)/sizeof(int); i++){
		if (InputFromSerial0[i] == 0){
			flag = false;
		}
		else{
			flag = true;
			break;
		}
	}
	
	if (flag){
		if(NumberUARTPort == 0){						// Если данные прилетели из нулевого UART
			Serial.print(F("Configuration Date and Time:   "));
			Serial.print(InputFromSerial0[2]); Serial.print(F(":")); Serial.print(InputFromSerial0[1]); Serial.print(F(":")); Serial.print(InputFromSerial0[0]); Serial.print(F(" "));
			Serial.print(InputFromSerial0[3]); Serial.print(F(".")); Serial.print(InputFromSerial0[4]); Serial.print(F(".")); Serial.print(InputFromSerial0[5]); Serial.print(F(" "));
			switch(InputFromSerial0[6]){
				case 1:
					Serial.println(F("Monday"));
					break;
				case 2:
					Serial.println(F("Tuesday"));
					break;
				case 3:
					Serial.println(F("Wednesday"));
					break;
				case 4:
					Serial.println(F("Thursday"));
					break;
				case 5:
					Serial.println(F("Friday"));
					break;
				case 6:
					Serial.println(F("Saturday"));
					break;
				case 7:
					Serial.println(F("Sunday"));
					break;
			}
		}
		time.settime(InputFromSerial0[0],InputFromSerial0[1],InputFromSerial0[2],InputFromSerial0[3],InputFromSerial0[4],InputFromSerial0[5],InputFromSerial0[6]);
	}
	else{
		switch(NumberUARTPort){
			case 0:
				Serial.print(time.gettime("H")); Serial.print(F(":")); Serial.print(time.gettime("i")); Serial.print(F(":")); Serial.print(time.gettime("s")); Serial.print(F(" "));
				Serial.print(time.gettime("d")); Serial.print(F(".")); Serial.print(time.gettime("m")); Serial.print(F(".")); Serial.print(time.gettime("Y")); Serial.print(F(" ")); Serial.println(time.gettime("M"));
				break;
			case 2:
				Serial2.print("t ");
				Serial2.print(time.gettime("H")); Serial2.print(F(" "));
				Serial2.print(time.gettime("i")); Serial2.print(F(" "));
				Serial2.print(time.gettime("s")); Serial2.print(F(" "));
				Serial2.print(time.gettime("d")); Serial2.print(F(" "));
				Serial2.print(time.gettime("m")); Serial2.print(F(" "));
				Serial2.print(time.gettime("Y")); Serial2.print(F(" "));
				Serial2.println(time.gettime("M"));
				break;
		}
	}

	CleanInputFromSerial0();	
// 	for (byte i=0; i<sizeof(InputFromSerial0)/sizeof(int); i++){			// Затираем массив после работы
// 		InputFromSerial0[i] = 0;
// 	}
	recievedFlag_date = false;
	flag = false;
}