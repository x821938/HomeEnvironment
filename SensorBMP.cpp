#include "SensorBMP.h"
#include "Logging.h"
#include <Homie.h>
#include <Adafruit_BMP085.h>

extern HomieNode EnvironmentNode;
Adafruit_BMP085 bmp;


void SensorBMP::connect( uint16_t meassureFreq, uint16_t reportFreq, uint16_t deviceLostAfter ) {
	isSetup = true;

	sensorBMPpressure.connect( "BMP", "pressure", "pressure", "mbar", AVERAGE, reportFreq, deviceLostAfter );
	sensorBMPtemp.connect( "BMP", "temperatureBMP", "temperature", "C", AVERAGE, reportFreq, deviceLostAfter );

	meassureTimer.setup( meassureFreq * 1000 );
	reconnect();
}


void SensorBMP::reconnect() {
	if ( bmp.begin() ) {
		LOG_NOTICE( "BMP", "Sensor setup completed" );
	} else {
		LOG_ERROR( "BMP", "Sensor setup failed" );
	}
}


void SensorBMP::handle() {
	if ( isSetup ) {
		if ( meassureTimer.triggered() ) {
			readTemp();
			readPressure();
		}
		sensorBMPpressure.handle();
		sensorBMPtemp.handle();
	}
}


void SensorBMP::readTemp() {
	float temperature = bmp.readTemperature();
	if ( temperature < 60 && temperature > -30 ) {
		sensorBMPtemp.addIncomingData( temperature );
	} else {
		LOG_ERROR( "BMP", "Could not get sane temperature" );
		reconnect();
	}
}


void SensorBMP::readPressure() {
	float pressure = bmp.readPressure() / 100.0;
	if ( pressure > 800 && pressure < 1100 ) {
		sensorBMPpressure.addIncomingData( pressure );
	} else {
		LOG_ERROR( "BMP", "Could not get sane pressure" );
		reconnect();
	}
}



bool SensorBMP::isSensorAlive() {
	bool alive = ( isSetup && sensorBMPpressure.isSensorAlive() && sensorBMPtemp.isSensorAlive() );
	return alive;
}