#include "SensorTSL.h"
#include "Logging.h"
#include <Homie.h>
#include <SparkFunTSL2561.h>

extern HomieNode EnvironmentNode;
SFE_TSL2561 light;


void SensorTSLClass::connect( uint16_t meassureFreq, uint16_t reportFreq ) {
	this->reportFreq = reportFreq;
	this->meassureFreq = meassureFreq;

	sensorErrors = 0;
	lightAcc = lightMeasurements = rawLightBroadbandAcc = rawLightInfraredAcc = rawLightMeasurements = 0;
	lastReported = lastMeassured = millis();

	light.begin();
	light.setTiming( gain, SAMPLE_TIME, sampledTime );
	light.setPowerUp();
	LOG_NOTICE( "TSL", "Sensor Connected" );
}


void SensorTSLClass::handle() {
	if ( millis() - lastMeassured >= meassureFreq * 1000 ) {
		readSensor();
		lastMeassured = millis();
	}
	if ( millis() - lastReported >= reportFreq * 1000 ) {
		sendLight();
		sendIrPercentage();
		lastReported = millis();
	}
}


void SensorTSLClass::readSensor() {
	unsigned int broadbandRaw, irRaw;
	double lux;

	if ( light.getData( broadbandRaw, irRaw ) ) {
		if ( light.getLux( gain, sampledTime, broadbandRaw, irRaw, lux ) ) {
			LOG_INFO( "TSL", "Light = " << lux << " lux. Gain = " << ( gain == 0 ? "1X." : "16X" ) );
			lightAcc += lux;
			LOG_INFO( "TSL", "Raw Broadband Light = " << broadbandRaw );
			rawLightBroadbandAcc += broadbandRaw;
			LOG_INFO( "TSL", "Raw Infrared Light = " << irRaw );
			rawLightInfraredAcc += irRaw;
			lightMeasurements++;
			rawLightMeasurements++;
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
		connect( meassureFreq, reportFreq );
		sensorErrors++;

	}
}


double SensorTSLClass::getAvgLight() {
	if ( lightMeasurements > 0 ) {
		double lightAverage = lightAcc / lightMeasurements;
		return lightAverage;
	} else {
		return 0;
	}
}


float SensorTSLClass::getAvgIrPercentage() {
	if ( rawLightMeasurements > 0 ) {
		float rawLightBroadbandAverage = rawLightBroadbandAcc / rawLightMeasurements;
		float rawLightInfraredAverage = rawLightInfraredAcc / rawLightMeasurements;
		return ( rawLightInfraredAverage / ( rawLightBroadbandAverage + rawLightInfraredAverage ) ) * 100.0;
	} else {
		return 0;
	}
}


void SensorTSLClass::sendLight() {
	if ( lightMeasurements > 0 && sensorErrors == 0 ) {
		double light = getAvgLight();
		LOG_NOTICE( "MQTT", "Sending TSL average light of " << light << " lux" );
		EnvironmentNode.setProperty( "light" ).send( String( light ) );
	} else {
		LOG_WARNING( "MQTT", "No TSL light data to send" );
	}
	lightAcc = lightMeasurements = 0;
}


void SensorTSLClass::sendIrPercentage() {
	if ( rawLightMeasurements > 0  && sensorErrors == 0) {
		float IRPercentage = getAvgIrPercentage();
		LOG_NOTICE( "MQTT", "Sending TSL average IR Percentage of " << IRPercentage << " %" );
		EnvironmentNode.setProperty( "IRpercentage" ).send( String( IRPercentage ) );
	} else {
		LOG_WARNING( "MQTT", "No TSL raw data to send" );
	}
	rawLightBroadbandAcc = rawLightInfraredAcc = rawLightMeasurements = 0;
}


bool SensorTSLClass::isSensorAlive() {
	bool alive = ( sensorErrors == 0);
	sensorErrors = 0;
	return alive;
}
