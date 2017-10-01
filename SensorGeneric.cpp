#include "SensorGeneric.h"
#include "Logging.h"
#include <Homie.h>

extern volatile bool mqttConnected;
extern HomieNode EnvironmentNode;



/* Resets the state machine by setting the sensor in warmup state */
void SensorGeneric::setup() {
	sensorState = SensorWarmup;
	warmupStartetAt = millis();
	LOG_INFO( shortName, "Sensor setup completed (" << friendlyName << ")" );
}



/* Setting up using this class if the sensor does not use inheritance. This would be for a physical sensor returning
   more than one value */
void SensorGeneric::setup( unsigned long warmupTime, String shortName, String friendlyName, String mqttName, String unit ) {
	this->warmupTime = warmupTime;
	this->shortName = shortName;
	this->friendlyName = friendlyName;
	this->mqttName = mqttName;
	this->unit = unit;
	setup();
}



/* Should be called continously from main loop. It handles the statemachine of the sensor */
void SensorGeneric::handle() {
	switch ( sensorState ) {
		case SensorWarmup:
			if ( millis() - warmupStartetAt > warmupTime ) {
				sensorState = SensorRead;
				LOG_DEBUG( shortName, "Sensor warmed up after " << warmupTime << " ms (" << friendlyName << ")" );
			}
			break;
		case SensorRead:
			readValue();
			break;
		case SensorWaitMqtt:
			if ( mqttConnected ) sensorState = SensorSend;
			break;
		case SensorSend:
			sendValue();
			break;
		case SensorSentMqtt:
			break;
	}
}



/* This should be overloaded by the child sensor class.
   In this method the child should read the physical sensor and store it's value in our "sensorValue" variable */
void SensorGeneric::readValue() {
}



/* Instead of using overloaded readValue method - a value can be provided here */
void SensorGeneric::putValue(double value) {
	if ( sensorState == SensorRead ) {
		this->sensorValue = value;
		sensorState = SensorWaitMqtt;
		LOG_INFO( shortName, "Got a " << friendlyName << " reading of " << value << " " << unit );
	}
}



/* Sends the value that was previously read via MQTT */
void SensorGeneric::sendValue() {
	LOG_NOTICE( shortName, "Sendig MQTT " << mqttName << " = " << sensorValue << " " << unit );
	EnvironmentNode.setProperty( mqttName ).send( String( sensorValue ) );
	sensorState = SensorSentMqtt;
}



/* Returns true of the sensor has been read and the value has been sent via MQTT */
bool SensorGeneric::isValueSent() {
	return sensorState == SensorSentMqtt;
}



/* Returns true if the sensor has passed the warmup time */
bool SensorGeneric::isWarmedUp() {
	return sensorState != SensorWarmup;
}