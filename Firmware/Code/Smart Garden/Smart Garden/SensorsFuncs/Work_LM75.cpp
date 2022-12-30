#include <Arduino.h>

#include "Work_LM75.h"
#include "../Configuration.h"
#include "../Sensors.h"
#include "../ShareFunction.h"





word float2regdata (float temp){
	// First multiply by 8 and coerce to integer to get +/- whole numbers
	// Then coerce to word and bitshift 5 to fill out MSB
	return (word)((int)(temp * 8) << 5);
}

float LM75_regdata2float (word regdata){
	return ((float)(int)regdata / 32) / 8;
}


float LM75_temp (byte Address){
	return LM75_regdata2float(I2C_read_register16_bit(LM75_TEMP_REGISTER, Address));
}


// ============================== OS port ==============================
byte LM75_Read_Conf_OS (byte Address){
	return I2C_read_register8_bit(LM75_CONF_REGISTER, Address);
}

void LM75_Write_Conf_OS (byte Value, byte Address){
	I2C_write_register8_bit(LM75_CONF_REGISTER, Value, Address);
}


// ================================ TOS ================================
float LM75_Read_tos_byte (byte Address){
	return LM75_regdata2float(I2C_read_register16_bit(LM75_TOS_REGISTER, Address));
}

void LM75_Write_tos_byte (float Value, byte Address){
	I2C_write_register16_bit(LM75_TOS_REGISTER, float2regdata(Value), Address);
}


// =============================== THYST ===============================
float LM75_Read_thyst_byte (byte Address){
	return LM75_regdata2float(I2C_read_register16_bit(LM75_THYST_REGISTER, Address));
}

void LM75_Write_thyst_byte (float Value, byte Address){
	I2C_write_register16_bit(LM75_THYST_REGISTER, float2regdata(Value), Address);
}


/*
boolean LM75_shutdown (byte Address){
	return LM75_conf() & 0x01;
}

void LM75_shutdown (boolean val, byte Address){
	LM75_conf(val << LM75_CONF_SHUTDOWN);
}*/