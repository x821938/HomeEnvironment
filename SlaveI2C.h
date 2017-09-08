#ifndef _SLAVEI2C_h
#define _SLAVEI2C_h

#include <Arduino.h>

#define I2C_STRETCH 80000 // Crazy high, but otherwise it doesn't work....

class SlaveI2CClass
{
 protected:
	void debug( const void * var, const uint8_t varSize, const char * msg, const uint8_t addr );
 public:
	bool pollData( const uint8_t i2cAddr, const char cmd, void * receiveVar, const uint8_t varSize );
};

#endif

