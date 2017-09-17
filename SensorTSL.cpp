#include "SensorTSL.h"
#include "Logging.h"
#include <Homie.h>
#include <SparkFunTSL2561.h>

extern HomieNode EnvironmentNode;
SFE_TSL2561 light;


void SensorTSL::connect( uint16_t meassureFreq, uint16_t reportFreq, uint16_t deviceLostAfter ) {
	isSetup = true;

	sensorTSLlight.connect( "TSL", "light", "light", "lux", AVERAGE, reportFreq, deviceLostAfter );
	sensorTSLirPct.connect( "TSL", "IRpercentage", "IR percentage", "%", AVERAGE, reportFreq, deviceLostAfter );

	meassureTimer.setup( meassureFreq * 1000 );
	reconnect();
}


void SensorTSL::handle() {
	if ( isSetup ) {
		if ( meassureTimer.triggered() ) readSensor();
		sensorTSLlight.handle();
		sensorTSLirPct.handle();
	}
}


void SensorTSL::reconnect() {
	light.begin();
	light.setTiming( gain, SAMPLE_TIME, sampledTime );
	light.setPowerUp();
	LOG_NOTICE( "TSL", "Sensor setup completed" );
}

void SensorTSL::readSensor() {
	unsigned int broadbandRaw, irRaw;
	double lux;

	if ( light.getData( broadbandRaw, irRaw ) ) {
		if ( light.getLux( gain, sampledTime, broadbandRaw, irRaw, lux ) ) {
			sensorTSLlight.addIncomingData( lux );
			LOG_DEBUG( "TSL", "Raw Broadband Light = " << broadbandRaw );
			LOG_DEBUG( "TSL", "Raw Infrared Light = " << irRaw );
			sensorTSLirPct.addIncomingData( getIrPercentage( broadbandRaw, irRaw ) );
			if ( lux < HIGH_GAIN_LIMIT && gain == 0 ) {
				LOG_WARNING( "TSL", "Light is less than " << HIGH_GAIN_LIMIT << ". Raising gain to 16X" );
				gain = 1;
				light.setTiming( gain, SAMPLE_TIME, sampledTime );
			}
		} else {
			LOG_WARNING( "TSL", "Sensor too satuated. Lowering Gain to 1X" );
			gain = 0; // 1x gain
			light.setTiming( gain, SAMPLE_TIME, sampledTime );
		}
	} else {
		byte error = light.getError();
		LOG_ERROR( "TSL", "Error code = " << error );
		reconnect();
	}
}


float SensorTSL::getIrPercentage( unsigned int broadband, unsigned int ir ) {
	if ( broadband > 0 && ir > 0 ) {
		float irPtc = (float) ir / ( broadband + ir ) * 100.0;
		return irPtc;
	} else {
		return 0;
	}
}


bool SensorTSL::isSensorAlive() {
	bool alive = isSetup && sensorTSLlight.isSensorAlive() && sensorTSLirPct.isSensorAlive();
	return alive;
}