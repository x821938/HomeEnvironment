#include "SensorBMP.h"
#include "Logging.h"
#include <Homie.h>
#include <Adafruit_BMP085.h>

extern HomieNode EnvironmentNode;
Adafruit_BMP085 bmp;


void SensorBMPClass::connect( uint16_t meassureFreq, uint16_t reportFreq ) {
	this->reportFreq = reportFreq;
	this->meassureFreq = meassureFreq;

	sensorErrors = 0;
	lastReported = lastMeassured = millis();
	pressureAcc = pressureMeasurements = tempAcc = tempMeasurements = 0;

	if ( bmp.begin() ) {
		LOG_NOTICE( "BMP", "Sensor connected" );
	} else {
		LOG_ERROR( "BMP", "Could not connect to sensor" );
		sensorErrors++;
	}
}


void SensorBMPClass::handle() {
	if ( millis() - lastMeassured >= meassureFreq * 1000 ) {
		readTemp();
		readPressure();
		lastMeassured = millis();
	}
	if ( millis() - lastReported >= reportFreq * 1000 ) {
		sendTemperature();
		sendPressure();
		lastReported = millis();
	}
}


void SensorBMPClass::readTemp() {
	float temperature = bmp.readTemperature();
	if ( temperature < 60 && temperature > -30 ) {
		LOG_INFO( "BMP", "Temperature = " << temperature << " C" );
		tempAcc += temperature;
		tempMeasurements++;
	} else {
		LOG_ERROR( "BMP", "Could not get sane temperature" );
		sensorErrors++;
		connect( reportFreq, meassureFreq );
	}
}


void SensorBMPClass::readPressure() {
	float pressure = bmp.readPressure() / 100.0;
	if ( pressure > 800 && pressure < 1100 ) {
		LOG_INFO( "BMP", "Pressure = " << pressure << " mbar" );
		pressureAcc += pressure;
		pressureMeasurements++;
	} else {
		LOG_ERROR( "BMP", "Could not get sane pressure" );
		sensorErrors++;
		connect( reportFreq, meassureFreq );
	}
}


float SensorBMPClass::getAvgTemp() {
	if ( tempMeasurements > 0 ) {
		float temperatureAverage = tempAcc / tempMeasurements;
		return temperatureAverage;
	} else {
		return 0;
	}
}


float SensorBMPClass::getAvgPressure() {
	if ( pressureMeasurements > 0 ) {
		float pressureAverage = pressureAcc / pressureMeasurements;
		return pressureAverage;
	} else {
		return 0;
	}
}


void SensorBMPClass::sendTemperature() {
	if ( sensorErrors == 0 ) {
		float temperature = getAvgTemp();
		LOG_NOTICE( "MQTT", "Sending BMP average temperature of " << temperature << " C" );
		EnvironmentNode.setProperty( "temperatureBMP" ).send( String( temperature ) );
	} else {
		LOG_WARNING( "MQTT", "No BMP temperature data to send" );
	}
	tempAcc = tempMeasurements = 0;
}


void SensorBMPClass::sendPressure() {
	if ( sensorErrors == 0 ) {
		float pressure = getAvgPressure();
		LOG_NOTICE( "MQTT", "Sending BMP average pressure of " << pressure << " mbar" );
		EnvironmentNode.setProperty( "pressure" ).send( String( pressure ) );
	} else {
		LOG_WARNING( "MQTT", "No BMP pressure data to send" );
	}
	pressureAcc = pressureMeasurements = 0;
}


bool SensorBMPClass::isSensorAlive() {
	bool alive = ( sensorErrors == 0);
	sensorErrors = 0;
	return alive;
}
