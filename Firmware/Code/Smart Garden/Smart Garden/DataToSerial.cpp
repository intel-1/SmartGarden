
#include <EEPROM.h>

#include "DataToSerial.h"
#include "Sensors.h"
#include "EEPROM_ADR.h"
#include "Warning.h"
#include "main.h"


void InformateDataFromSerial(){
	if(EEPROM_int_read(E_LOGING_TO_SERIAL) == 4){
		Serial.println(F(""));
	}
	Serial.print(time.gettime("d-m-Y, H:i:s, D")); // выводим дату и время
	Serial.print(F("\tRealTempLeftUp:\t"));
	Serial.print(F("\tVCC: ")); Serial.print(VCC); Serial.print(F("v"));
	Serial.print(F("\tHumidityAir: "));
	if(HumidityAirOffline == true){
		Serial.print(F("Off"));
	}/*
	else if(EEPROM.read(E_QuantityErrorReadHumidityAir) <= EEPROM.read(E_MaxQuantityErrorForSwitchError)){
		Serial.print(F("Error"));
	}  */
	else{
		//Serial.print(HumidityAir);
		Serial.print(F("%"));
	}
	//Serial.print(F("\tHumidityEarthLeft: "));  Serial.print(HumidityEarthLeft); Serial.print(F("%"));
	//Serial.print(F("\tHumidityEarthRight: ")); Serial.print(HumidityEarthRight); Serial.print(F("%"));
	Serial.print(F("\t\t\t"));
	Serial.print(F("RealTempLeftDown: "));
	/*if(RealTempLeftDown == 255){
		Serial.println(F("Error"));
	} 
	else{
		Serial.print(RealTempLeftDown); Serial.print((char)176); Serial.println(F("C"));
	}*/
}



