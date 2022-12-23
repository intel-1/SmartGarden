#include "MAX44009.h"
#include <math.h>

MAX44009::MAX44009() {}

bool MAX44009::begin(uint8_t addr){
	Wire.beginTransmission(addr);
	Wire.write(0x02);
	//Wire.write(0x40);
	Wire.write(0x43);				// Задержка 100ms (для тусклого света)
	if(Wire.endTransmission() == 0){
		return true;
	}
	else return false;
}


float MAX44009::get_lux(uint8_t addr){
	unsigned int data[2];
	
	Wire.beginTransmission(addr);
	Wire.write(0x03);
	Wire.endTransmission();
	
	// Request 2 bytes of data
	Wire.requestFrom(addr, 2);
	
	// Read 2 bytes of data luminance msb, luminance lsb
	if (Wire.available() == 2)
	{
		data[0] = Wire.read();
		data[1] = Wire.read();
	}
	
	// Convert the data to lux
	int exponent = (data[0] & 0xF0) >> 4;
	int mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F);
	
	//float luminance = pow(2, exponent) * mantissa * 0.045;
	float luminance = (float)(((0x00000001 << exponent) * (float)mantissa) * 0.045);
	
	return luminance;
}