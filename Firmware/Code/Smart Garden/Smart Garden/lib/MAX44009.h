#ifndef MAX44009_h
#define MAX44009_h
#include <Arduino.h>
#include <Wire.h>


class MAX44009 {
	public:
	MAX44009();
	
	bool begin(uint8_t addr);
	
	float get_lux(uint8_t addr);
};



#endif