#include <Arduino.h>
#include <EEPROM.h>
#include <Print.h>
#include <HardwareSerial.h>
#include <stdint.h>
#include <avr/wdt.h>

#include "lib\I2C_eeprom.h"

#include "Configuration.h"
#include "EEPROM_ADR.h"
#include "Sensors.h"


I2C_eeprom EXT_EEPROM(0x50, I2C_DEVICESIZE_24LC64);




void Write_String_To_EEPROM(int EEPROM_address, String Data){
	//Serial.println("1");
	byte len = Data.length();
	//Serial.println("1.1");
	//Serial.println("2");
	EXT_EEPROM.writeByte(EEPROM_address, len);
	Serial.println("3");
	for(byte Symbol = 1; Symbol <= len; Symbol ++){
		//Serial.println(Symbol++);
		EXT_EEPROM.writeByte(EEPROM_address + Symbol, Data[Symbol-1]);
	}
}


String Read_String_From_EEPROM(int EEPROM_address){
	String Out_Link;
	for(byte Symbol = 1; Symbol <= EXT_EEPROM.readByte(EEPROM_address); Symbol ++){
		Out_Link += char(EXT_EEPROM.readByte(EEPROM_address + Symbol));
	}
	return Out_Link;
}


int EEPROM_int_read(int addr){ // чтение из EEPROM
	uint8_t  x[4];
	for(uint8_t i = 0; i < 4; i++){
		x[i] = EEPROM.read(i + addr);
	}
	int16_t *y = (int16_t *)&x;
	return y[0];
}


float EEPROM_float_read(int addr) {
	byte raw[4];
	for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr+i);
	float &num = (float&)raw;
	return num;
}
void EEPROM_float_write(int addr, float num) {
	byte raw[4];
	(float&)raw = num;
	for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
}


unsigned long EEPROM_ulong_read(int addr) {
	byte raw[4];
	for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr+i);
	unsigned long &num = (unsigned long&)raw;
	return num;
}
void EEPROM_ulong_write(int addr, unsigned long num) {
	byte raw[4];
	(unsigned long&)raw = num;
	for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
}


long EEPROM_long_read(int addr) {
	byte raw[4];
	for(byte i = 0; i < 4; i++) raw[i] = EEPROM.read(addr+i);
	long &num = (long&)raw;
	return num;
}
void EEPROM_long_write(int addr, long num) {
	byte raw[4];
	(long&)raw = num;
	for(byte i = 0; i < 4; i++) EEPROM.write(addr+i, raw[i]);
}


unsigned int EEPROM_uint_read(int addr) {
	byte raw[2];
	for(byte i = 0; i < 2; i++) raw[i] = EEPROM.read(addr+i);
	unsigned int &num = (unsigned int&)raw;
	return num;
}
void EEPROM_uint_write(int addr, unsigned int num) {
	byte raw[2];
	(unsigned int&)raw = num;
	for(byte i = 0; i < 2; i++) EEPROM.write(addr+i, raw[i]);
}


String EEPROM_str_read(int addr) {
	char raw[21];
	for (byte i = 0; i < 21; i++){
		while (EEPROM.read(addr + i) != 0){
			EEPROM.get(addr + i, raw[i]);
		}
	}
	return raw;
}
void EEPROM_str_write(int addr, byte size, String text) {
	for(byte i = 0; i < size; i++){
		EEPROM.put(addr + i, text[i]);
	}
}


void cleanEEPROM(byte ThatClean){
/*
	1 - Общий сброс. Очищает весь EEPROM
	2 - Очищаем все ошибки
	3 - 
	4 - 
*/
	switch(ThatClean){
		case 1:											// Очистка всех ошибок в EEPROM
			for(int i = 2000; i <= 4096; i ++){
				EEPROM.update(i, 0);
			}			
			Serial.println(F("Error`s on EEPROM is clean"));
			break;
		case 2:											// Очистка настроек датчиков
			for (int i = 1000; i < 2000; i++){
				EEPROM.update(i, 0);
			}
			Serial.println(F("Configs`s sensor`s on EEPROM is clean"));
			break;
		case 3:											// Очистка настроек групп
			for (int i = 700; i < 999; i++){
				EEPROM.update(i, 0);
			}
			Serial.println(F("Config`s channel`s on EEPROM is clean"));
			break;
		case 4:											// Очистка настрек исполнительных модулей
			for (int i = 100; i < 699; i++){
				EEPROM.update(i, 0);
			}
			Serial.println(F("Config`s exec modules on EEPROM is clean"));
			break;
		case 5:											// Очистка настроек контроллера
			for (int i = 1; i < 100; i++){
				EEPROM.update(i, 0);
			}
			Serial.println(F("Config`s controller on EEPROM is clean"));
			break;
		case 6:											// Полная очистка EEPROM	
			for (int i = 1; i < 4096; i++){
				EEPROM.update(i, 0);
			}
			Serial.println(F("EEPROM is clean"));
			break;	
	}
}


void View_DataEEPROM(){

/*
	Serial.print(F("WarningReadTempBox_SMS: ")); Serial.println(EEPROM.read(E_WarningReadTempBox_SMS));
	Serial.print(F("E_Low_Input_VCC_SMS: ")); Serial.println(EEPROM.read(E_Low_Input_VCC_SMS));
	Serial.print(F("WarningMaxTempLeft_SMS: "));  Serial.println(EEPROM.read(E_WarningMaxTempLeft_SMS));
	Serial.print(F("WarningMaxTempRight_SMS: ")); Serial.println(EEPROM.read(E_WarningMaxTempRight_SMS));
	Serial.print(F("WarningMinTempLeft_SMS: "));  Serial.println(EEPROM.read(E_WarningMinTempLeft_SMS));
	Serial.print(F("WarningMinTempRight_SMS: ")); Serial.println(EEPROM.read(E_WarningMinTempRight_SMS));
	Serial.print(F("WarningReadHumidityAir_SMS: ")); Serial.println(EEPROM.read(E_WarningReadHumidityAir_SMS));
	Serial.print(F("WarningReadWaterSensorLeft_SMS: "));  Serial.println(EEPROM.read(E_WarningReadWaterSensorLeft_SMS));
	Serial.print(F("WarningReadWaterSensorRight_SMS: ")); Serial.println(EEPROM.read(E_WarningReadWaterSensorRight_SMS));
	Serial.print(F("ErrorCloseWindow_1_SMS: "));  Serial.println(EEPROM.read(E_ErrorCloseWindow_1_SMS));
	Serial.print(F("ErrorCloseWindow_2_SMS: "));  Serial.println(EEPROM.read(E_ErrorCloseWindow_2_SMS));
	Serial.print(F("ErrorCloseWindow_3_SMS: "));  Serial.println(EEPROM.read(E_ErrorCloseWindow_3_SMS));
	Serial.print(F("ErrorReadTempLeft_SMS: "));   Serial.println(EEPROM.read(E_ErrorReadTempLeft_SMS));      // Ни одна температура левой теплицы не известна
	Serial.print(F("ErrorReadTempRight_SMS: "));  Serial.println(EEPROM.read(E_ErrorReadTempRight_SMS));    // Ни одна температура правой теплицы не известна// Флаги состояния отправки СМС (отправлялась или нет)
	Serial.print(F("GlobalError_SMS: ")); Serial.println(EEPROM.read(E_GlobalError_SMS));
	Serial.println(F(""));*/
}

