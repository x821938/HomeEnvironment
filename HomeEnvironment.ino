#define AVAIL_REPORT_FREQ 30

#include "Logging.h"
#include "SPI.h"
#include "Timing.h"
#include <Homie.h>
#include <Wire.h>

#include "GenericSensor.h"
#include "SensorPIR.h"
#include "SensorTSL.h"
#include "SensorCCS.h"
#include "SensorBMP.h"


SPI spi;
Timing availReportTimer;
HomieNode EnvironmentNode( "indoor", "indoor" );

SensorPIRClass sensorPIR; // Motion
SensorTSLClass sensorTSL; // Light
SensorCCSClass sensorCCS; // Co2 and TVOC
SensorBMPClass sensorBMP; // Pressure and temperature
GenericSensorClass sensorDHTtemp;
GenericSensorClass sensorDHThumidity;
GenericSensorClass sensorPPDdust;
GenericSensorClass sensorMHZco2;
GenericSensorClass sensorMICvol;
GenericSensorClass sensorMICrms;
GenericSensorClass sensorMICmax;



void setup() {
	Serial.begin( 115200 );

	// I2C settings ( remember to change twi_setClockStretchLimit from 230 to 460 in core_esp8266_si2c.c... Workaround for CCS881 Sensor )
	Wire.begin( SDA, SCL );
	Wire.setClock( 100000L ); 

	// Homie setup
	Homie_setFirmware( "homeenvironment", "1.0.0" );
	Homie.setLoopFunction( homieHandlerLoop );
	Homie.disableResetTrigger();
	Homie.disableLedFeedback();
	Homie.setup();
	
	// Remote SPI sensors connected to "Arduino Pro Mini"
	sensorDHTtemp.connect ( "DHT", "temperatureDHT", "temperature", "C", AVERAGE, 30, 10 );
	sensorDHThumidity.connect ( "DHT", "humidity", "humidity", "%", AVERAGE, 30, 10 );
	sensorPPDdust.connect( "PPD", "dust", "dust level", "pcs/l", AVERAGE, 300, 600 );
	sensorMHZco2.connect( "MHZ", "mhzco2", "Co2 concentration", "ppm", AVERAGE, 30, 20 );
	sensorMICvol.connect( "MIC", "avgvolume", "Avg volume", "%", AVERAGE, 30, 60 );
	sensorMICrms.connect( "MIC", "rmsvolume", "RMS volume", "%", AVERAGE, 30, 60 );
	sensorMICmax.connect( "MIC", "maxvolume", "Max volume", "%", MAX, 30, 60 );

	// Local Sensors connected to ESP
	sensorCCS.connect( 5, 30, 15 );	
	sensorTSL.connect( 5, 30, 15 );
	sensorBMP.connect( 5, 30, 15 );	
	sensorPIR.connect( 30 );

	// Reporting of sensor availability
	availReportTimer.setup( (long) AVAIL_REPORT_FREQ * 1000 );

	// Disable software watchdog. Now Relying on the hardware watchdog to reset if things goes wrong.
	ESP.wdtDisable(); 
}


void loop() {
	Homie.loop();
	ESP.wdtFeed(); // Feed the watchdog.
}


void homieHandlerLoop() {
	if ( Homie.isConnected() ) {
		sensorDHTtemp.handle();
		sensorDHThumidity.handle();
		sensorPPDdust.handle();
		sensorMHZco2.handle();
		sensorMICvol.handle();
		sensorMICrms.handle();
		sensorMICmax.handle();
		sensorBMP.handle();
		sensorCCS.handle( sensorDHTtemp.getAvgValue(), sensorDHThumidity.getAvgValue() );
		sensorTSL.handle();
		sensorPIR.handle();

		if ( availReportTimer.triggered() ) reportSensorAvailability();
	}
}


// Send the status of all sensors in one byte. Each bit represents a sensor
void reportSensorAvailability() {
	bool microphoneAlive = sensorMICvol.isSensorAlive() && sensorMICrms.isSensorAlive() && sensorMICmax.isSensorAlive();
	uint8_t status						=			0b00000000;
	status += sensorDHThumidity.isSensorAlive() ?	0b00000001 : 0;
	status += sensorDHTtemp.isSensorAlive() ?		0b00000010 : 0;
	status += sensorTSL.isSensorAlive() ?			0b00000100 : 0;
	status += sensorCCS.isSensorAlive() ?			0b00001000 : 0;
	status += microphoneAlive ?						0b00010000 : 0;
	status += sensorPPDdust.isSensorAlive() ?		0b00100000 : 0;
	status += sensorBMP.isSensorAlive() ?			0b01000000 : 0;
	status += sensorMHZco2.isSensorAlive() ?		0b10000000 : 0;
	LOG_NOTICE( "MQTT", "Sending status of all sensors: " << status );
	EnvironmentNode.setProperty( "sensorstatus" ).send( String( status ) );
}
