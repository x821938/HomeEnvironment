#ifndef _SPI_h
#define _SPI_h

#include <Arduino.h>
#include <functional>

typedef std::function<void( void* data )> handleIncomingSPI;
static handleIncomingSPI incomingCallback;


class SPI {
protected:
	static void spiReceivedHandler( uint8_t * data, size_t len );
	static void debug( const void * var, const uint8_t varSize, const char * dataType );

public:
	SPI();

};

#endif

