#include "SensorDHT.h"
#include "Logging.h"
#include <Homie.h>
#include "DHT.h"

extern HomieNode EnvironmentNode;


void SensorDHTClass::connect( uint16_t meassureFreq, uint16_t reportFreq ) {
	this->reportFreq = reportFreq;
	this->meassureFreq = meassureFreq;

	sensorErrors = 0;
	tempAccu = tempMeasurements = humidityAcc = humidityMeasurements = 0;
	lastMeassured = lastReported = millis();

	LOG_NOTICE( "DHT", "Sensor connected" );
}


void SensorDHTClass::handle() {
	if ( millis() - lastMeassured >= meassureFreq * 1000 ) {
		readSensor();
		lastMeassured = millis();
	}
	if ( millis() - lastReported >= reportFreq * 1000 ) {
		sendTemp();
		sendHumidity();
		lastReported = millis();
	}
}


void SensorDHTClass::readSensor() {
	bool pollSuccess = slaveI2C.pollData( DHT_I2C_ADDR, 'H', &dhtSensorData, 10 ); // compiler fucks up sizeof (dhtSensorData)... hardcoded the size!

	if ( pollSuccess ) {
		if ( dhtSensorData.temperatureOk ) {
			LOG_INFO( "DHT", "Temperature = " << dhtSensorData.temperature << " C" );
			tempAccu += dhtSensorData.temperature;
			tempMeasurements++;
		} else {
			LOG_ERROR( "DHT", "Error reading temperature" );
			sensorErrors++;
		}

		if ( dhtSensorData.humidityOk ) {
			LOG_INFO( "DHT", "Humidity = " << dhtSensorData.humidity << " %" );
			humidityAcc += dhtSensorData.humidity;
			humidityMeasurements++;
		} else {
			LOG_ERROR( "DHT", "Error reading humidity" );
			sensorErrors++;
		}
	} else {
		LOG_ERROR( "DHT", "Error talking to I2C slave" );
	}
}


float SensorDHTClass::getAvgTemp() {
	if ( tempMeasurements > 0 ) {
		float temperatureAverage = tempAccu / tempMeasurements;
		return temperatureAverage;
	} else {
		return 0;
	}
}


float SensorDHTClass::getAvgHumidity() {
	if ( humidityMeasurements > 0 ) {
		float humidityAverage = humidityAcc / humidityMeasurements;
		return humidityAverage;
	} else {
		return 0;
	}
}


void SensorDHTClass::sendTemp() {
	if ( tempMeasurements > 0 && sensorErrors == 0) {
		float temperature = getAvgTemp();
		LOG_NOTICE( "MQTT", "Sending DHT average temperature of " << temperature << " C" );
		EnvironmentNode.setProperty( "temperatureDHT" ).send( String( temperature ) );
	} else {
		LOG_ERROR( "MQTT", "No DHT temperature data to send." );
	}
	tempAccu = tempMeasurements = 0;
}


void SensorDHTClass::sendHumidity() {
	if ( humidityMeasurements > 0 && sensorErrors == 0) {
		float humidity = getAvgHumidity();
		LOG_NOTICE( "MQTT", "Sending DHT average humidity of " << humidity << "%" );
		EnvironmentNode.setProperty( "humidity" ).send( String( humidity ) );
	} else {
		LOG_ERROR( "MQTT", "No DHT humidity data to send" );
	}
	humidityAcc = humidityMeasurements = 0;
}


bool SensorDHTClass::isSensorAlive() {
	bool alive = ( sensorErrors == 0 );
	sensorErrors = 0;
	return alive;
}