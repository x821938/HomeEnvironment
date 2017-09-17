#include "SensorPIR.h"
#include "Logging.h"
#include <Homie.h>

extern HomieNode EnvironmentNode;


void SensorPIR::connect( uint16_t reportFreq ) {
	isSetup = true;

	reportTimer.setup( reportFreq * 1000 );
	pinMode( PIR_PIN, INPUT ); // PIR input setup
	LOG_NOTICE( "PIR", "Sensor connected" );
}


void SensorPIR::handle() {
	if ( isSetup ) {
		bool pirValue = digitalRead( PIR_PIN ) == HIGH;
		if ( pirValue != lastPirValue ) {
			lastPirValue = pirValue;

			LOG_INFO( "PIR", "IO changed" );
			sendPIR();

			if ( pirValue ) {
				motionStartedAt = millis();
			} else {
				motionTime += ( millis() - motionStartedAt ) / 1000;
				LOG_NOTICE( "MQTT", "Sending PIR Motion Time = " << motionTime << " s" );
				EnvironmentNode.setProperty( "PIRmotiontime" ).send( String( motionTime ) );
			}
		}
		if ( reportTimer.triggered() ) sendPIRMotionTime();
	}
}


void SensorPIR::sendPIR() {
	bool pirValue = digitalRead( PIR_PIN ) == HIGH;
	EnvironmentNode.setProperty( "PIR" ).send( pirValue ? "true" : "false" );
	LOG_NOTICE( "MQTT", "Sending PIR Pin Status = " << ( pirValue ? "true" : "false" ) );
}


void SensorPIR::sendPIRMotionTime() {
	LOG_NOTICE( "MQTT", "Sending PIR motion time of " << motionTime << " s" );
	EnvironmentNode.setProperty( "PIRmotiontime" ).send( String( motionTime ) );
}