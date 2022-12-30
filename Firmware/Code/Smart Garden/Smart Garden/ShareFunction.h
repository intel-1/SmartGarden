#ifndef SHAREFUNCTION_H
#define SHAREFUNCTION_H







word I2C_read_register16_bit (byte reg, byte Address);

void I2C_write_register16_bit (byte reg, word regdata, byte Address);

void Write_To_i2c(uint8_t regID, uint8_t regDat, uint8_t Address);

byte Read_Byte_from_i2c(uint8_t regID, uint8_t Address);

word I2C_read_register8_bit (byte reg, byte Address);

void I2C_write_register8_bit (byte reg, byte Value, byte Address);


#endif