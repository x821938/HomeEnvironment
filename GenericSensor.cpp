#include "GenericSensor.h"
#include "Logging.h"
#include "Homie.h"

extern HomieNode EnvironmentNode;


void GenericSensorClass::connect(const char *sensorName, const char *mqttName, const char *dataDesc, const char *dataUnit, SensorType sensorType, uint16_t reportFreq, uint16_t deviceLostAfter ) {
	isSetup = true;

	_sensorName = new char[strlen( sensorName ) + 1];
	strcpy( _sensorName, sensorName );
	_mqttName = new char[strlen( mqttName ) + 1];
	strcpy( _mqttName, mqttName );
	_dataDesc = new char[strlen( dataDesc ) + 1];
	strcpy( _dataDesc, dataDesc );
	_dataUnit = new char[strlen( dataUnit ) + 1];
	strcpy( _dataUnit, dataUnit );
	_deviceLostAfter = (long) deviceLostAfter * 1000;

	reportTimer.setup( reportFreq * 1000 );
	lastDataReceivedAt = millis();
	dataAcc = dataMeasurements = dataMax = 0;

	LOG_NOTICE( _sensorName, "Sensor for meassuring " << _dataDesc << " is setup" );
}


void GenericSensorClass::handle() {
	if ( isSetup && reportTimer.triggered() ) sendData();
}


void GenericSensorClass::addIncomingData( float value ) {
	if ( isSetup ) {
		lastDataReceivedAt = millis();
		LOG_INFO( _sensorName, _dataDesc << " = " << value << " " << _dataUnit );
		dataAcc += value;
		dataMax = _max( dataMax, value );
		dataMeasurements++;
	}
}


float GenericSensorClass::getAvgValue() {
	if ( dataMeasurements > 0 ) {
		float dataAverage = dataAcc / dataMeasurements;
		return dataAverage;
	} else {
		return 0;
	}
}


void GenericSensorClass::sendData() {
	if ( dataMeasurements > 0 ) {
		if ( _sensorType == AVERAGE ) {
			float avgValue = getAvgValue();
			LOG_NOTICE( "MQTT", "Sending " << _sensorName << " " << _dataDesc << " of " << avgValue << " " << _dataUnit );
			EnvironmentNode.setProperty( _mqttName ).send( String( avgValue ) );
		} else if ( _sensorType == MAX ) {
			LOG_NOTICE( "MQTT", "Sending " << _sensorName << " " << _dataDesc << " of " << dataMax << " " << _dataUnit );
			EnvironmentNode.setProperty( _mqttName ).send( String( dataMax ) );
		}
	} else {
		LOG_WARNING( "MQTT", "No " << _sensorName << " " << _dataDesc << " data to send." );
	}
	dataAcc = dataMeasurements = 0;
}


bool GenericSensorClass::isSensorAlive() {
	bool alive = isSetup && ( millis() - lastDataReceivedAt < _deviceLostAfter );
	return alive;
}