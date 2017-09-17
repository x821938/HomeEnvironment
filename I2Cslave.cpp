#include "Logging.h"
#include <Homie.h>
#include "I2Cslave.h"
#include "SensorGeneric.h"
#include <Wire.h>

extern SensorGeneric sensorDHTtemp;
extern SensorGeneric sensorDHThumidity;
extern SensorGeneric sensorPPDdust;
extern SensorGeneric sensorMHZco2;
extern SensorGeneric sensorMICvol;
extern SensorGeneric sensorMICrms;
extern SensorGeneric sensorMICmax;


void I2C::setup() {
	pinMode( I2C_GOT_DATA_PIN, INPUT );
}


void I2C::handle() {
	while ( digitalRead( I2C_GOT_DATA_PIN ) == HIGH ) {
		LOG_DEBUG( "I2C", "Slave has data for us. Reqesting it" );
		fetchData();
	}
}


void I2C::fetchData() {
	Wire.setClockStretchLimit( I2C_STRETCH ); // On esp8266 this is needed in order to talk to atmega328p - don't ask why.

	I2Cframe i2cFrame;
	const uint8_t i2cFrameSize = 5; // sizeof doesn't work on my struct in this compiler!

	if ( Wire.requestFrom( I2C_SLAVE_ADDR, i2cFrameSize ) ) {
		for ( uint8_t index = 0; index < i2cFrameSize; index++ ) { // get all i2c incomming data and put it into the var, byte by byte.
			*( (byte*) &i2cFrame + index ) = Wire.read();
		}
		hexDebug( &i2cFrame, i2cFrameSize, "Received", I2C_SLAVE_ADDR );
		handleData( i2cFrame.dataType, i2cFrame.data );
	} else {
		LOG_ERROR( "I2C", "Error getting data from slave" );
	}
}


void I2C::handleData( const char dataType, const float data ) {
	switch ( dataType ) {
		case 'V':
			sensorMICvol.addIncomingData( data );
			break;
		case 'M':
			sensorMICmax.addIncomingData( data );
			break;
		case 'R':
			sensorMICrms.addIncomingData( data );
			break;
		case 'D':
			sensorPPDdust.addIncomingData( data );
			break;
		case 'H':
			sensorDHThumidity.addIncomingData( data );
			break;
		case 'T':
			sensorDHTtemp.addIncomingData( data );
			break;
		case 'Q':
			sensorMHZco2.addIncomingData( data );
			break;
		default:
			LOG_ERROR( "I2C", "Got unknown data type" );
	}
}


void I2C::hexDebug( const void* var, const uint8_t varSize, const char* msg, const uint8_t addr ) {
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