#include <user_interface.h>
#include "Logging.h"
#include "Timing.h"
#include <Homie.h>
#include <Wire.h>

#include "SensorGeneric.h"
#include "SensorTSL.h"
#include "SensorBMP.h"
#include "SensorCCS.h"
#include "SensorSlave.h"


#define SLEEP_TIME 120	// Time between each wakeup of device. Keep as long as possible to save power. ESP is hungry
#define PANIC_TIME 10	// After this time without sending all data, just go to sleep. Keep power down if a sensor is broken.		


HomieNode EnvironmentNode( "indoor", "indoor" );
SensorTSL sensorTSL;		// Light
SensorBMP sensorBMP;		// Pressure and temperature
SensorCCS sensorCCS;		// CO2 and TVOC
SensorSlave sensorSlave;	// All slave sensors on atmega via i2c


volatile bool mqttConnected = false; // This is set by ISR when homie is ready to send mqtt messages
uint8_t bootReason; // Global readable bootreason - for now it's for SensorCCS class.



void setup() {
	Serial.begin( 115200 ); Serial.println();
	getBootReason();

	// I2C settings ( remember to change twi_setClockStretchLimit from 230 to 460 in core_esp8266_si2c.c... Workaround for CCS881 Sensor )
	Wire.begin( SDA, SCL );
	Wire.setClock( 100000L ); 

	// Homie setup
	Homie_setFirmware( "homeenvironment", "1.0.0" );
	Homie.disableResetTrigger();
	Homie.disableLedFeedback();
	Homie.disableLogging();
	Homie.onEvent( onHomieEvent );
	Homie.setup();

	// Sensors
	sensorTSL.setup();
	sensorBMP.setup();
	sensorCCS.setup();
	sensorSlave.setup();
}



/* Handles all sensors. If all sensors has sent their data over MQTT a deep sleep is requested 
   If all sensors havn't sent all their data before PANIC_TIME, we deep sleep anyway */
void loop() {
	static bool sleepOrdered = false;

	Homie.loop();
	if ( !sleepOrdered ) {
		sensorTSL.handle();
		sensorBMP.handle();
		sensorCCS.handle();
		sensorSlave.handle();

		if ( sensorTSL.isValueSent() && sensorBMP.isValueSent() && sensorCCS.isValueSent() && sensorSlave.isValueSent() ) {
			sensorCCS.calibrate( sensorSlave.getHumidity(), sensorSlave.getTemperature() ); // Calibrate the CCS sensor before sleeping
			Homie.prepareToSleep(); // If all sensors have delivered their data, we can now request a deep sleep
			sleepOrdered = true; // We only want to instruct homie once to do a deep sleep
		}
		if ( millis() > PANIC_TIME * 1000UL ) {
			LOG_CRITICAL( "HOM", "We havn't got data from all sensors - some hardware is broken." );
			Homie.prepareToSleep();
			sleepOrdered = true;
		}
	}
}



/* All events from homie is going to this handler */
void onHomieEvent( const HomieEvent& event ) {
	switch ( event.type ) {
		case HomieEventType::MQTT_CONNECTED:
			LOG_NOTICE( "HOM", "MQTT connected" );
			mqttConnected = true;
			break;
		case HomieEventType::READY_TO_SLEEP:
			LOG_NOTICE( "HOM", "Going to sleep now for " << SLEEP_TIME << " seconds" );
			ESP.deepSleep( 1000000UL * SLEEP_TIME );
			break;
	}
}



/* Checks how device is booted. Bootcode is used in order to avoid unnecessary CCS811 sensor reset.
   We are interested in code5 which means ESP has woken up from a deep sleep */
void getBootReason() {
	rst_info *resetInfo;
	resetInfo = ESP.getResetInfoPtr();
	bootReason = ( *resetInfo ).reason;
	LOG_NOTICE( "ESP", "Booted with reason code: " << bootReason );
}