#include "SPI.h"
#include "External\SPISlave.h"
#include "Logging.h"
#include "Homie.h"
#include "SensorSPI.h"


extern SensorSPIClass sensorDHTtemp;
extern SensorSPIClass sensorDHThumidity;
extern SensorSPIClass sensorPPDdust;
extern SensorSPIClass sensorMHZco2;
extern SensorSPIClass sensorMICvol;
extern SensorSPIClass sensorMICrms;
extern SensorSPIClass sensorMICmax;


SPI::SPI() {
	delay( 1000 ); // starting SPI too fast fails the boot!
	SPISlave.onData( &spiReceivedHandler );
	SPISlave.begin();
}


void SPI::spiReceivedHandler( uint8_t * data, size_t len ) {
	char msgType = *( (char*) data );			// First byte of SPI telegram is the type of data
	uint8_t msgLen = *( (uint8_t*) data + 1 );	// Second byte is the length of data

	float dataValue;
	memcpy( &dataValue, data + 2, sizeof( dataValue ) ); // At byte number two comes the date/struct

	switch ( msgType ) {
		case 'V':
			debug( &dataValue, msgLen, "mic volume" );
			sensorMICvol.addIncomingData( dataValue );
			break;
		case 'M':
			debug( &dataValue, msgLen, "mic max volume" );
			sensorMICmax.addIncomingData( dataValue );
			break;
		case 'R':
			debug( &dataValue, msgLen, "mic RMS" );
			sensorMICrms.addIncomingData( dataValue );
			break;
		case 'D':
			debug( &dataValue, msgLen, "dust" );
			sensorPPDdust.addIncomingData( dataValue );
			break;
		case 'H':
			debug( &dataValue, msgLen, "humidity" );
			sensorDHThumidity.addIncomingData( dataValue );
			break;
		case 'T':
			debug( &dataValue, msgLen, "temperature" );
			sensorDHTtemp.addIncomingData( dataValue );
			break;
		case 'Q':
			debug( &dataValue, msgLen, "CO2" );
			sensorMHZco2.addIncomingData( dataValue );
			break;
		default:
			LOG_ERROR( "SPI", "Got unknown data" );
	}
}


// Prints out hex data of any var/struct for debugging purposes.
void SPI::debug( const void* var, const uint8_t varSize, const char* dataType ) {
	String hexString;
	for ( uint8_t i = 0; i < varSize; i++ ) { // traverse the var byte by byte
		char* p = (char *) var;

		uint8_t currentByte = *( p + i ); // get byte number i
		char currentByteHex[3];
		sprintf( currentByteHex, "%02X", currentByte ); // convert it to hex
		hexString = hexString + currentByteHex; // and concatenate it into a printable string of all bytes

		if ( i != varSize - 1 ) hexString = hexString + ",";
	}
	LOG_DEBUG( "SPI", "Received " << varSize << " bytes of " <<  dataType << " data: " << hexString );
}