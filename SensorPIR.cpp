#include "SensorPIR.h"
#include "Logging.h"
#include <Homie.h>

extern HomieNode EnvironmentNode;


void SensorPIRClass::connect( uint16_t reportFreq ) {
	this->reportFreq = reportFreq;
	lastReported = millis();
	pinMode( PIR_PIN, INPUT ); // PIR input setup
	LOG_NOTICE( "PIR", "Sensor connected" );
}


void SensorPIRClass::handle() {
	bool pirValue = digitalRead( PIR_PIN ) == HIGH;
	if ( pirValue != lastPirValue ) {
		lastPirValue = pirValue;

		LOG_INFO( "PIR", "IO changed" );
		sendPIR();

		if ( pirValue ) {
			motionStarted = millis();
		} else {
			motionTime += ( millis() - motionStarted ) / 1000;
			LOG_NOTICE( "MQTT", "Sending PIR Motion Time = " << motionTime << " s" );
			EnvironmentNode.setProperty( "PIRmotiontime" ).send( String( motionTime ) );
		}
	}
	if ( millis() - lastReported >= reportFreq * 1000 ) {
		sendPIRMotionTime();
		lastReported = millis();
	}
}

void SensorPIRClass::sendPIR() {
	bool pirValue = digitalRead( PIR_PIN ) == HIGH;
	EnvironmentNode.setProperty( "PIR" ).send( pirValue ? "true" : "false" );
	LOG_NOTICE( "MQTT", "Sending PIR Pin Status = " << ( pirValue ? "true" : "false" ) );
}

void SensorPIRClass::sendPIRMotionTime() {
	LOG_NOTICE( "MQTT", "Sending PIR motion time of " << motionTime << " s" );
	EnvironmentNode.setProperty( "PIRmotiontime" ).send( String( motionTime ) );
}