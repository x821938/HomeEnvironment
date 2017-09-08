#include "SensorPPD.h"
#include "Logging.h"
#include <Wire.h>
#include <Arduino.h>
#include <Homie.h>

extern HomieNode EnvironmentNode;


void SensorPPDClass::connect( uint16_t meassureFreq, uint16_t reportFreq ) {
	this->reportFreq = reportFreq;
	this->meassureFreq = meassureFreq;

	sensorErrors = 0;
	lastMeassured = lastReported = millis();
	dustMeasurements = dustAcc = 0;

	LOG_NOTICE( "PPD", "Sensor connected" );
}


void SensorPPDClass::handle() {
	if ( millis() - lastMeassured >= meassureFreq * 1000 ) {
		readSensor();
		lastMeassured = millis();
	}
	if ( millis() - lastReported >= reportFreq * 1000 ) {
		sendDust();
		lastReported = millis();
	}
}


float SensorPPDClass::readSensor() {
	float dustConcentration;
	bool sensorAvailable = slaveI2C.pollData( PPD_I2C_ADDR, 'D', &dustConcentration, sizeof( dustConcentration ) );
	if ( sensorAvailable ) {
		LOG_INFO( "PPD", "Dust concentration = " << dustConcentration << " pcs/l");
		dustAcc += dustConcentration;
		dustMeasurements++;
	} else {
		LOG_ERROR( "PPD", "Error talking to I2C slave");
		sensorErrors++;
	}
}


float SensorPPDClass::getAvgDust() {
	if ( dustMeasurements > 0 ) {
		float dustAverage = dustAcc / dustMeasurements;
		return dustAverage;
	} else {
		return 0;
	}
}


void SensorPPDClass::sendDust() {
	if ( dustMeasurements>0 ) {
		float dustConcentration = getAvgDust();
		LOG_NOTICE( "MQTT", "Sending PPD Dust concentration of " << dustConcentration << " pcs/l" );
		EnvironmentNode.setProperty( "dust" ).send( String( dustConcentration ) );
	} else {
		LOG_WARNING( "MQTT", "No PPD dust data to send." );
	}
	dustMeasurements = dustAcc = 0;
}


bool SensorPPDClass::isSensorAlive() {
	bool alive = ( sensorErrors == 0);
	sensorErrors = 0;
	return alive;
}