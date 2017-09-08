#include "Logging.h"
#include "SensorPPD.h"
#include "SensorPIR.h"
#include "SensorTSL.h"
#include "SensorCCS.h"
#include "SensorBMP.h"
#include "SensorDHT.h"
#include "SensorMIC.h"
#include <Homie.h>
#include <Wire.h>

#define AVAIL_REPORT_FREQ 30

SensorPPDClass sensorPPD;
SensorPIRClass sensorPIR;
SensorTSLClass sensorTSL;
SensorCCSClass sensorCCS;
SensorBMPClass sensorBMP;
SensorDHTClass sensorDHT;
SensorMICClass sensorMIC;


HomieNode EnvironmentNode( "indoor", "indoor" );
uint16_t sensorStatus;

void setup() {
	Serial.begin( 115200 );

	// I2C settings
	Wire.begin( SDA, SCL );
	Wire.setClock( 100000L ); // Slow down I2C bus for stability
	Wire.setClockStretchLimit( 40000 ); // for slave arduino

	// Homie setup
	Homie_setFirmware( "homeenvironment", "1.0.0" );
	Homie.setLoopFunction( homieHandlerLoop );
	Homie.disableResetTrigger();
	Homie.disableLedFeedback();
	Homie.setup();

	// Sensors startup
	sensorCCS.connect( 5, 30 );
	sensorTSL.connect( 5, 30 );
	sensorBMP.connect( 5, 30 );
	sensorDHT.connect( 5, 30 );
	sensorMIC.connect( 30, 30 );
	sensorPPD.connect( 300, 300 );
	sensorPIR.connect( 30 );
}


void loop() {
	Homie.loop();
}


void homieHandlerLoop() {
	static long availLastReported = 0;

	if ( Homie.isConnected() ) {
		sensorDHT.handle();
		sensorBMP.handle();
		sensorCCS.handle( sensorBMP.getAvgTemp(), sensorDHT.getAvgHumidity() );
		sensorTSL.handle();
		sensorPPD.handle();
		sensorMIC.handle();
		sensorPIR.handle();

		if ( millis() - availLastReported >= AVAIL_REPORT_FREQ * 1000 ) {
			reportSensorAvailability();
			availLastReported = millis();
		}
	}
}


void reportSensorAvailability() {
	uint8_t status						= 0b11000000;
	status += sensorBMP.isSensorAlive() ? 0b00000001 : 0;
	status += sensorDHT.isSensorAlive() ? 0b00000010 : 0;
	status += sensorTSL.isSensorAlive() ? 0b00000100 : 0;
	status += sensorCCS.isSensorAlive() ? 0b00001000 : 0;
	status += sensorMIC.isSensorAlive() ? 0b00010000 : 0;
	status += sensorPPD.isSensorAlive() ? 0b00100000 : 0;
	LOG_NOTICE( "MQTT", "Sending status of all sensors: " << status );
	EnvironmentNode.setProperty( "sensorstatus" ).send( String( status ) );
}
