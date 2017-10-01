#include "SensorCCS.h"
#include "Logging.h"
#include <Homie.h>
#include "CustomCCS811.h"


extern uint8_t bootReason;
extern HomieNode EnvironmentNode;
Adafruit_CCS811 ccs;



/* Setup two generic sensors one for co2 and one for tvoc */
void SensorCCS::setup() {
	needsReset = bootReason != 5; // Reset sensor if we didn't wake up from a deep-sleep (like normal power up or wdt)

	isConnected = false;
	connect();

	sensorCCSco2.setup( 3000, "CCS", "CO2 concentration", "co2", "ppm" ); // make sure power/transmissions have settled before reading - avoid reading noise.
	sensorCCStvoc.setup( 3000, "CCS", "TVOC concentration", "tvoc", "ppb");

	isSetup = true;
}



/* Should be called from main loop. Handles each co2 and tvoc values and reads the sensor if they passed the warm up time */
void SensorCCS::handle() {
	if ( isSetup ) {
		sensorCCSco2.handle();
		sensorCCStvoc.handle();
		
		if ( sensorCCSco2.isWarmedUp() && sensorCCStvoc.isWarmedUp() ) readSensor();
	}
}



/* Initializes the physical co2 sensor CCS811. It calls a modified adafruit CCS811 library
   If esp is powered on, it sends a reset to the CCS. Otherwise it just connects to it.
   This is to get around the 20min warm up time of the sensor */
void SensorCCS::connect() {
	if ( needsReset ) LOG_DEBUG( "CCS", "Just powered up, sensor needs a reset" );
	if ( ccs.begin(CCS_I2C_ADDR, needsReset) ) {
		LOG_INFO( "CSS", "Sensor connected" );
		isConnected = true;
	} else {
		LOG_ERROR( "CCS", "Sensor connection failed" );
	}
}



/* Reads co2 and tvoc values from the sensor */
void SensorCCS::readSensor() {
	if ( isConnected ) {
		if ( !ccs.readData() ) {
			float co2 = ccs.geteCO2();
			float tvoc = ccs.getTVOC();

			if ( ccs.geteCO2() >= 400 && ccs.geteCO2() <= 8192 ) {
				sensorCCSco2.putValue( co2 );
			} else {
				LOG_ERROR( "CCS", "Could not get sane CO2 data. Got " << co2 );
			}
			if ( ccs.getTVOC() >= 0 && ccs.getTVOC() <= 1187 ) {
				sensorCCStvoc.putValue( tvoc );
			} else {
				LOG_ERROR( "CCS", "Could not get sane TVOC data. Got " << tvoc );
			}
		} else {
			LOG_ERROR( "CCS", "Sensor could not read data" );
			isConnected = false;
		}
	} else {
		isConnected = false;
	}
}



void SensorCCS::calibrate( float humidity, float temperature ) {
	if ( temperature != 0 && humidity != 0 ) {
		LOG_NOTICE( "CCS", "Calibrating sensor with temp = " << temperature << " C and humidity = " << humidity << " %" );
		ccs.setEnvironmentalData( humidity, temperature );
	} else {
		LOG_WARNING( "CCS", "Could not calibrate sensor because we don't have good temp/humidity data" );
	}
}



/* Returns true of the sensor values has been sent via MQTT */
bool SensorCCS::isValueSent() {
	if ( !isSetup ) return true;
	return sensorCCSco2.isValueSent() && sensorCCStvoc.isValueSent();
}