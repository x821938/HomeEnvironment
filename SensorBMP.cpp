#include "SensorBMP.h"
#include "Logging.h"
#include <Homie.h>
#include <Adafruit_BMP085.h>

extern HomieNode EnvironmentNode;
Adafruit_BMP085 bmp;



/* Setup two generic sensors one for temperature and one for humidity */
void SensorBMP::setup() {
	connect();

	sensorBMPpressure.setup( 100, "BMP", "pressure", "pressure", "mbar" );
	sensorBMPtemp.setup( 100, "BMP", "temperature", "temperatureBMP", "C" );

	isSetup = true;
}



/* Initializes the physical pressure & temperature sensor */
void SensorBMP::connect() {
	if ( bmp.begin() ) {
		LOG_INFO( "BMP", "Sensor connected" );
	} else {
		LOG_ERROR( "BMP", "Sensor connection failed" );
	}
}



/* Should be called from main loop. Handles each temperature and pressure values and reads the sensor if they passed the warm up time */
void SensorBMP::handle() {
	if ( isSetup ) {
		sensorBMPpressure.handle();
		sensorBMPtemp.handle();

		if ( sensorBMPtemp.isWarmedUp() ) readTemp();
		if ( sensorBMPpressure.isWarmedUp() ) readPressure();
	}
}



/* Reads temperature from physical sensor */
void SensorBMP::readTemp() {
	float temperature = bmp.readTemperature();
	if ( temperature < 60 && temperature > -30 ) {
		sensorBMPtemp.putValue( temperature );
	} else {
		LOG_ERROR( "BMP", "Could not get sane temperature" );
		connect();
	}
}



/* Reads pressure from physical sensor */
void SensorBMP::readPressure() {
	float pressure = bmp.readPressure() / 100.0;
	if ( pressure > 800 && pressure < 1100 ) {
		sensorBMPpressure.putValue( pressure );
	} else {
		LOG_ERROR( "BMP", "Could not get sane pressure" );
		connect();
	}
}

/* Returns true of the sensor values has been sent via MQTT */
bool SensorBMP::isValueSent() {
	if ( !isSetup ) return true;
	return sensorBMPpressure.isValueSent() && sensorBMPtemp.isValueSent();
}