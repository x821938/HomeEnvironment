#include "Logging.h"
#include <Homie.h>
#include "SlaveI2C.h"
#include <Wire.h>


bool SlaveI2CClass::pollData( const uint8_t i2cAddr, const char cmd, void* receiveVar, const uint8_t varSize ) {
	Wire.setClockStretchLimit( I2C_STRETCH ); // On esp8266 this is needed in order to talk to atmega328p - don't ask why.
	Wire.beginTransmission( i2cAddr );
	Wire.write( cmd ); // Send the command that tells the slave what data it should send.
	Wire.endTransmission();
	debug( &cmd, sizeof(cmd), "Sent", i2cAddr );

	Wire.requestFrom( i2cAddr, varSize ); // request a pull of x bytes from slave

	uint8_t index = 0;
	while ( Wire.available() ) { // get all i2c incomming data and put it into the var, byte by byte.
		*( (byte*) receiveVar + index++ ) = Wire.read();
	}
	debug( receiveVar, varSize, "Received", i2cAddr );

	if ( index != varSize ) { // i2c slave didn't respond nicely
		LOG_ERROR( "I2C", "Unexpected amount of data received" );
		return false;
	} else { 
		return true;
	}
}

void SlaveI2CClass::debug(const void* var, const uint8_t varSize, const char* msg, const uint8_t addr) {
	String hexString;
	for ( uint8_t i = 0; i < varSize; i++ ) { // traverse the var byte by byte
		char* p = (char *) var;

		uint8_t currentByte = *( p + i ); // get byte number i
		char currentByteHex[3]; 
		sprintf( currentByteHex, "%02X", currentByte ); // convert it to hex
		hexString = hexString + currentByteHex; // and concatenate it into a printable string of all bytes

		if ( i != varSize - 1 ) hexString = hexString + ",";
	}
	LOG_DEBUG( "I2C", msg << " " << varSize << " bytes (i2c slave addr " << addr << ") in hex: " << hexString );
}