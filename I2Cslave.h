// I2C.h
#ifndef _SLAVEI2C_h
#define _SLAVEI2C_h

#include <Arduino.h>

#define I2C_SLAVE_ADDR 8
#define I2C_STRETCH 80000 // Crazy high, but otherwise it doesn't work....
#define I2C_GOT_DATA_PIN 12

struct I2Cframe {
	float data;
	char dataType;
};

class I2C {

protected:
	void fetchData();
	void handleData( const char dataType, const float data );
	void hexDebug( const void * var, const uint8_t varSize, const char * msg, const uint8_t addr );
public:
	void setup();
	void handle();

};

#endif