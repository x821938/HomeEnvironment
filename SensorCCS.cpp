#include "SensorCCS.h"
#include "Logging.h"
#include "Adafruit_CCS811.h"
#include <Homie.h>

extern HomieNode EnvironmentNode;
Adafruit_CCS811 ccs;


void SensorCCSClass::connect( uint16_t meassureFreq, uint16_t reportFreq ) {
	this->reportFreq = reportFreq;
	this->meassureFreq = meassureFreq;

	sensorErrors = 0;
	lastCalibrated = lastMeassured = lastReported = millis();
	co2Acc = co2Measurements = tvocAcc = tvocMeasurements = 0;

	if ( ccs.begin() ) {
		LOG_NOTICE( "CSS", "Sensor Connected" );
	} else {
		LOG_ERROR( "CCS", "Sensor connection failed" );
		sensorErrors++;
	}
}

void SensorCCSClass::handle( float calibTemp, float calibHumidity) {
	this->calibTemp = calibTemp;
	this->calibHumidity = calibHumidity;

	// Is the sensor warmed up?
	if ( millis() < CCS_WARMUP_TIME * 1000 ) {
		sensorWarmedUp = false;
	} else {
		// Sensor is warmed up, now we can do measurements and reporting
		sensorWarmedUp = true;
		if ( millis() - lastMeassured >= meassureFreq * 1000 ) {
			readSensor();
			lastMeassured = millis();
		}
		if ( millis() - lastReported >= reportFreq * 1000 ) {
			sendCo2();
			sendTvoc();
			sensorErrors = 0;
			lastReported = millis();
		}
		if ( millis() - lastCalibrated >= CCS_CALIB_FREQ * 1000 ) {
			calibrate();
			lastCalibrated = millis();
		}
	}
}


void SensorCCSClass::readSensor() {
	if ( sensorErrors > 0 ) connect( meassureFreq, reportFreq );
	if ( sensorErrors == 0 ) {
		if ( ccs.available() ) {
			if ( !ccs.readData() ) {
				if ( ccs.geteCO2() >= 400 && ccs.geteCO2() <= 8192 ) {
					LOG_INFO( "CCS", "CO2 concentration = " << ccs.geteCO2() << " ppm" );
					co2Acc += ccs.geteCO2();
					co2Measurements++;
				} else {
					LOG_ERROR( "CCS", "Could not get sane CO2 data" );
					sensorWarmedUp = false;
					sensorErrors++;
				}
				if ( ccs.getTVOC() >= 0 && ccs.getTVOC() <= 1187 ) {
					LOG_INFO( "CCS", "TVOC concentration = " << ccs.getTVOC() << " ppb" );
					tvocAcc += ccs.getTVOC();
					tvocMeasurements++;
				} else {
					LOG_ERROR( "CCS", "Could not get sane TVOC data" );
					sensorWarmedUp = false;
					sensorErrors++;
				}
			} else {
				LOG_ERROR( "CCS", "Sensor could not read data" );
				sensorErrors++;
			}
		} else {
			LOG_ERROR( "CCS","Sensor not available" );
			sensorErrors++;
		}
	} else {
		LOG_ERROR( "CCS", "Sensor not connected" );
		sensorErrors++;
	}
}


float SensorCCSClass::getAvgCo2() {
	if ( co2Measurements > 0 ) {
		float carbonDioxideAverage = co2Acc / co2Measurements;
		return carbonDioxideAverage;
	} else {
		return 0;
	}
}


float SensorCCSClass::getAvgTvoc() {
	if ( tvocMeasurements > 0 ) {
		float TVOCAverage = tvocAcc / tvocMeasurements;
		return TVOCAverage;
	} else {
		return 0;
	}
}


void SensorCCSClass::sendCo2() {
	if ( co2Measurements > 0 ) {
		uint16_t carbonDioxide = getAvgCo2();
		LOG_NOTICE( "MQTT", "Sending CCS average CO2 of " << carbonDioxide << " ppm" );
		EnvironmentNode.setProperty( "co2" ).send( String( carbonDioxide ) );
		co2Acc = co2Measurements = 0;
	} else {
		LOG_WARNING( "MQTT", "No CCS CO2 data to send");
	}
	true;
}


void SensorCCSClass::sendTvoc() {
	if ( tvocMeasurements > 0 ) {
		uint16_t TVOC = getAvgTvoc();
		LOG_NOTICE( "MQTT", "Sending CCS average TVOC of " << TVOC << " ppb" );
		EnvironmentNode.setProperty( "tvoc" ).send( String( TVOC ) );
		tvocAcc = tvocMeasurements = 0;
	} else {
		LOG_WARNING( "MQTT", "No CCS TVOC data to send" );
	}
}


void SensorCCSClass::calibrate() {
	if ( calibTemp != 0 && calibHumidity != 0 ) {
		LOG_NOTICE( "CCS", "Calibrating sensor with temp = " << calibTemp << " C and humidity = " << calibHumidity << " %" );
		ccs.setEnvironmentalData( calibHumidity, calibTemp );
	} else {
		LOG_WARNING( "CCS", "Could not calibrate sensor because we don't have good temp/humidity data" );
	}
}


bool SensorCCSClass::isSensorAlive() {
	bool alive = ( sensorErrors == 0 && sensorWarmedUp );
	sensorErrors = 0;
	return alive;
}