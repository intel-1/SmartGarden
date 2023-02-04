#include <./Arduino.h>
#include <./EEPROM.h>

#include "Write_to_ext_UART.h"
#include "../Configuration.h"
#include "../ParsingDataOnSerial.h"




void Write_To_Ext_UART(){
	
	Write_String_To_EEPROM(E_EXT_LINK_LOG_WEB_SERVER, F("http://net.uniscan.biz/LogsController.php/?Type=1"));
	Write_String_To_EEPROM(E_EXT_LINK_LOG_DATA_WEB_SERVER, F("http://net.uniscan.biz/ValueSensors.php/?Type=1"));
	Write_String_To_EEPROM(E_EXT_GPRS_APN_NAME, F("internet.beeline.ru"));
	Write_String_To_EEPROM(E_EXT_GPRS_APN_USER, F("beeline"));
	Write_String_To_EEPROM(E_EXT_GPRS_APN_PASSWORD, F("beeline"));
 	Write_String_To_EEPROM(E_EXT_GSM_CODE_BALANCE, F("#102#"));
	

	Serial.println(F("Save value to external EEPROM"));
	CleanInputFromSerial0();
	recievedFlag_write_extern_UART = false;
}