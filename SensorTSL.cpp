#include "SensorTSL.h"
#include "SensorGeneric.h"
#include "Logging.h"
#include <Homie.h>
#include <SparkFunTSL2561.h>


SFE_TSL2561 light;



/* Setup basic information about sensor for use for GenericSensor class. */
void SensorTSL::setup() {
	warmupTime = 100;
	shortName = "TSL";
	friendlyName = "light";
	mqttName = "light";
	unit = "lux";

	connect();
	SensorGeneric::setup();
	isSetup = true;
}



/* Initializes the physical light sensor TSL2541 */
void SensorTSL::connect() {
	light.begin();
	light.setTiming( gain, sampleTime, measuredSampledTime );
	light.setPowerUp();
	LOG_INFO( shortName, "Sensor connected" );

	tslState = MS13_GAIN_1X; // Start the senser with the least sensitivity - We expect sun!
	nextReadAt = millis(); // The first reading we can do immediately
}



/* Reads the light value from the sensor. If unsuccessful it tries to reinitialize it */
void SensorTSL::readValue() {
	if ( isSetup ) {
		unsigned int broadbandRaw, irRaw;
		double lux;

		if ( millis() > nextReadAt ) { // Only do a reading when it has done it's full sampling.
			if ( light.getData( broadbandRaw, irRaw ) ) {
				if ( findGain( broadbandRaw, irRaw ) ) { // If we found a suitable gain, we can start using the data
					if ( light.getLux( gain, measuredSampledTime, broadbandRaw, irRaw, lux ) ) {
						sensorValue = lux;
						LOG_INFO( shortName, "Got a light reading of " << sensorValue << " lux" );
					} else { // If we end up here we have a solar nuclear explsion
						LOG_ERROR( shortName, "Sensor too satuated!" );
						sensorValue = NAN;
					}
					light.setTiming( 0, 0, measuredSampledTime ); // Reset gain & sampletime on sensor so it's ready for next wakeup
					sensorState = SensorWaitMqtt; // Queue value up for sending
				}
			} else {
				byte error = light.getError();
				LOG_ERROR( shortName, "HW Error code = " << error );
				connect();
			}
		}
	}
}



/* Takes the raw values from the sensor as input. If they are too low, the sampletime will be raised little by little.
   If that is not enough then the 16x gain is finally turned on.
   Returns false if the reading is insuffiscient and a new reading is needed. Returns true when the reading is fine and can be interpreted */
bool SensorTSL::findGain( uint16_t broadbandRaw, uint16_t irRaw ) {
	LOG_DEBUG( "TSL", "Sample time = " << sampleTimeMs[sampleTime] << " ms. Gain = " << gain << ". Broadband raw = " << broadbandRaw << ". IR raw = " << irRaw );
	if ( broadbandRaw < GAIN_INCREASE_LIMIT || irRaw < GAIN_INCREASE_LIMIT ) {
		switch ( tslState ) {
			case MS13_GAIN_1X:
				tslState = MS101_GAIN_1X;
				nextReadAt = millis() + 101 + 10; // Give time for sampling plus a little slack
				sampleTime = 1;
				LOG_DEBUG( "TSL", "Readings too low, increasing sample time to " << sampleTimeMs[sampleTime] << " ms" );
				break;
			case MS101_GAIN_1X:
				tslState = MS402_GAIN_1X;
				nextReadAt = millis() + 402 + 10; // Give time for sampling plus a little slack
				sampleTime = 2;
				LOG_DEBUG( "TSL", "Readings too low, increasing sample time to " << sampleTimeMs[sampleTime] << " ms" );
				break;
			case MS402_GAIN_1X:
				tslState = MS402_GAIN_16X;
				nextReadAt = millis() + 402 + 10; // Give time for sampling plus a little slack
				gain = 1;
				LOG_DEBUG( "TSL", "Readings too low, increasing sample time to " << sampleTimeMs[sampleTime] << " ms" );
				break;
			case MS402_GAIN_16X:
				return true;
		}
		light.setTiming( gain, sampleTime, measuredSampledTime );
		return false;
	} else {
		return true;
	}
}