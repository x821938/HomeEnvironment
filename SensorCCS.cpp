#include "SensorCCS.h"
#include "Logging.h"
#include <Homie.h>
#include "Adafruit_CCS811.h"

extern HomieNode EnvironmentNode;
Adafruit_CCS811 ccs;


void SensorCCSClass::connect( uint16_t meassureFreq, uint16_t reportFreq, uint16_t deviceLostAfter, uint16_t warmedUpAfter ) {
	isSetup = true;
	isConnected = false;
	sensorWarmedUp = false;

	sensorCCSco2.connect( "CCS", "co2", "CO2 concentration", "ppm", AVERAGE, reportFreq, deviceLostAfter );
	sensorCCStvoc.connect( "CCS", "tvoc", "TVOC concentration", "ppb", AVERAGE, reportFreq, deviceLostAfter );

	connectionTimer.setup( CCS_RECONNECTION_TIME * 1000 );
	meassureTimer.setup( meassureFreq * 1000 );
	calibrateTimer.setup( CCS_CALIB_FREQ * 1000 );
	warmupTimer.setup( warmedUpAfter * 1000 );

	LOG_NOTICE( "CCS", "Sensor setup completed" );
}


void SensorCCSClass::handle( float calibTemp, float calibHumidity) {
	if ( isSetup ) {
		this->calibTemp = calibTemp;
		this->calibHumidity = calibHumidity;

		if ( isConnected == false && connectionTimer.triggered() ) reconnect();

		if ( sensorWarmedUp == false && warmupTimer.triggered() ) {
			LOG_NOTICE( "CCS", "Sensor now warmed up" );
			sensorWarmedUp = true;
		}
		if ( sensorWarmedUp ) {
			if ( meassureTimer.triggered() ) readSensor();
			if ( calibrateTimer.triggered() ) doCalibrate();
			sensorCCSco2.handle();
			sensorCCStvoc.handle();
		} 
	}
}


void SensorCCSClass::reconnect() {
	if ( ccs.begin() ) {
		LOG_NOTICE( "CSS", "Sensor is connected" );
		isConnected = true;
	} else {
		LOG_ERROR( "CCS", "Sensor connection failed" );
	}
}


void SensorCCSClass::readSensor() {
	if ( isConnected ) {
		if ( !ccs.readData() ) {
			if ( ccs.geteCO2() >= 400 && ccs.geteCO2() <= 8192 ) {
				float co2 = ccs.geteCO2();
				sensorCCSco2.addIncomingData( co2 );
			} else {
				LOG_ERROR( "CCS", "Could not get sane CO2 data" );
				isConnected = false;
			}
			if ( ccs.getTVOC() >= 0 && ccs.getTVOC() <= 1187 ) {
				float tvoc = ccs.getTVOC();
				sensorCCStvoc.addIncomingData( tvoc );
			} else {
				LOG_ERROR( "CCS", "Could not get sane TVOC data" );
				isConnected = false;
			}
		} else {
			LOG_ERROR( "CCS", "Sensor could not read data" );
			isConnected = false;
		}
	} else {
		isConnected = false;
	}
}


void SensorCCSClass::doCalibrate() {
	if ( calibTemp != 0 && calibHumidity != 0 ) {
		LOG_NOTICE( "CCS", "Calibrating sensor with temp = " << calibTemp << " C and humidity = " << calibHumidity << " %" );
		ccs.setEnvironmentalData( calibHumidity, calibTemp );
	} else {
		LOG_WARNING( "CCS", "Could not calibrate sensor because we don't have good temp/humidity data" );
	}
}


bool SensorCCSClass::isSensorAlive() {
	bool alive = ( isSetup && sensorWarmedUp && sensorCCSco2.isSensorAlive() && sensorCCStvoc.isSensorAlive() );
	return alive;
}