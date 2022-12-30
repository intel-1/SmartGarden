#include <Arduino.h>

#include "ShareFunction.h"
#include "Configuration.h"
#include "Sensors.h"




void Write_To_i2c(uint8_t regID, uint8_t regDat, uint8_t Address){
	Wire.beginTransmission(Address >> 1);
	Wire.write(regID & 0x00FF);
	Wire.write(regDat & 0x00FF);
	if(Wire.endTransmission()){
		Serial.print(F("error write reg "));
		Serial.println(regID);
	}
	delay(1);
}

byte Read_Byte_from_i2c(uint8_t regID, uint8_t Address){
	Wire.beginTransmission(Address >> 1);
	Wire.write(regID & 0x00FF);
	Wire.endTransmission();
	delay(1);
	Wire.requestFrom((Address >> 1),1);
	if(Wire.available())
	return Wire.read();
	Serial.print(F("error read reg "));
	Serial.println(regID);
	return 0;
}











word I2C_read_register16_bit (byte reg, byte Address){
	Wire.beginTransmission(Address);
	Wire.write(reg);
	Wire.endTransmission();
	
	Wire.requestFrom(Address, 2);
	word regdata = (Wire.read() << 8) | Wire.read();
	return regdata;
}

void I2C_write_register16_bit (byte reg, word regdata, byte Address){
	byte msb = (byte)(regdata >> 8);
	byte lsb = (byte)(regdata);
	
	Wire.beginTransmission(Address);
	Wire.write(reg);
	Wire.write(msb);
	Wire.write(lsb);
	Wire.endTransmission();
}
// =================================================

word I2C_read_register8_bit (byte reg, byte Address){
	Wire.beginTransmission(Address);
	Wire.write(reg);
	Wire.endTransmission();
	
	Wire.requestFrom(Address, 1);
	return Wire.read();
}

void I2C_write_register8_bit (byte reg, byte Value, byte Address){
	Wire.beginTransmission(Address);
	Wire.write(reg);
	Wire.write(Value);
	Wire.endTransmission();
}