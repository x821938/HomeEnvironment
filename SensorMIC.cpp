#include "SensorMIC.h"
#include "Logging.h"
#include "SlaveI2C.h"
#include <Homie.h>
#include <Wire.h>

extern HomieNode EnvironmentNode;


void SensorMICClass::connect( uint16_t meassureFreq, uint16_t reportFreq ) {
	this->reportFreq = reportFreq;
	this->meassureFreq = meassureFreq;

	sensorErrors = 0;
	lastMeassured = lastReported = millis();
	volMeasurements = volAvgAcc = volMax = volRmsAcc = 0;

	LOG_NOTICE( "MIC", "Sensor connected" );
}


void SensorMICClass::handle() {
	if ( millis() - lastMeassured >= meassureFreq * 1000 ) {
		readSensor();
		lastMeassured = millis();
	}
	if ( millis() - lastReported >= reportFreq * 1000 ) {
		sendSound();
		lastReported = millis();
	}
}


void SensorMICClass::readSensor() {
	MicData micData;

	bool sensorAvailable = slaveI2C.pollData( MIC_I2C_ADDR, 'M', &micData, sizeof(micData) );
	if ( sensorAvailable ) {
		LOG_INFO( "MIC", "Avg volume = " << micData.volAvgPtc << " %" );
		LOG_INFO( "MIC", "Max volume = " << micData.volMaxPtc << " %" );
		LOG_INFO( "MIC", "RMS volume = " << micData.volRmsPtc << " %" );
		volAvgAcc += micData.volAvgPtc;
		volRmsAcc += micData.volRmsPtc;
		volMax = _max( micData.volMaxPtc, volMax );
		volMeasurements++;
	} else {
		LOG_ERROR( "MIC", "Error talking to I2C slave" );
		sensorErrors++;
	}
}

float SensorMICClass::getAvgVol() {
	if ( volMeasurements > 0 ) {
		float volAverage = volAvgAcc / volMeasurements;
		return volAverage;
	} else {
		return 0;
	}
}

float SensorMICClass::getAvgRmsVol() {
	if ( volMeasurements > 0 ) {
		float volRmsAverage = volRmsAcc / volMeasurements;
		return volRmsAverage;
	} else {
		return 0;
	}
}

void SensorMICClass::sendSound() {
	if ( volMeasurements > 0 && sensorErrors == 0 ) {
		LOG_NOTICE( "MQTT", "Sending MIC average volume of " << getAvgVol() << " %" );
		EnvironmentNode.setProperty( "avgvolume" ).send( String( getAvgVol() ) );

		LOG_NOTICE( "MQTT", "Sending MIC max volume of " << volMax << " %" );
		EnvironmentNode.setProperty( "maxvolume" ).send( String( volMax ) );

		LOG_NOTICE( "MQTT", "Sending MIC RMS volume of " << getAvgRmsVol() << " %" );
		EnvironmentNode.setProperty( "rmsvolume" ).send( String( getAvgRmsVol() ) );
	} else {
		LOG_WARNING( "MQTT", "No MIC sound data to send" );
	}

	volMeasurements = volAvgAcc = volMax = volRmsAcc = 0;
}


bool SensorMICClass::isSensorAlive() {
	bool alive = ( sensorErrors == 0);
	sensorErrors = 0;
	return alive;
}