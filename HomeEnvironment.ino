#define AVAIL_REPORT_FREQ 30

#include <user_interface.h>
#include "Logging.h"
#include "I2Cslave.h"
#include "Timing.h"
#include <Homie.h>
#include <Wire.h>

#include "SensorGeneric.h"
#include "SensorPIR.h"
#include "SensorTSL.h"
#include "SensorCCS.h"
#include "SensorBMP.h"


I2C i2c;
Timing availReportTimer;
HomieNode EnvironmentNode( "indoor", "indoor" );

SensorPIR sensorPIR; // Motion
SensorTSL sensorTSL; // Light
SensorCCS sensorCCS; // Co2 and TVOC
SensorBMP sensorBMP; // Pressure and temperature
SensorGeneric sensorDHTtemp;
SensorGeneric sensorDHThumidity;
SensorGeneric sensorPPDdust;
SensorGeneric sensorMHZco2;
SensorGeneric sensorMICvol;
SensorGeneric sensorMICrms;
SensorGeneric sensorMICmax;



void setup() {
	Serial.begin( 115200 );


	// I2C settings ( remember to change twi_setClockStretchLimit from 230 to 460 in core_esp8266_si2c.c... Workaround for CCS881 Sensor )
	Wire.begin( SDA, SCL );
	Wire.setClock( 100000L ); 


	// Homie setup
	Homie_setFirmware( "homeenvironment", "1.0.0" );
	Homie.setSetupFunction( homieHandlerSetup ).setLoopFunction( homieHandlerLoop );
	Homie.disableResetTrigger();
	Homie.disableLedFeedback();
	Homie.setup();
}


void loop() {
	Homie.loop();
}

void homieHandlerSetup() {
	i2c.setup();

	// Remote SPI sensors connected to "Arduino Pro Mini"
	sensorDHTtemp.connect( "DHT", "temperatureDHT", "temperature", "C", AVERAGE, 30, 30 );
	sensorDHThumidity.connect( "DHT", "humidity", "humidity", "%", AVERAGE, 30, 30 );
	sensorPPDdust.connect( "PPD", "dust", "dust level", "pcs/l", AVERAGE, 300, 600 );
	sensorMHZco2.connect( "MHZ", "mhzco2", "Co2 concentration", "ppm", AVERAGE, 30, 30 );
	sensorMICvol.connect( "MIC", "avgvolume", "Avg volume", "%", AVERAGE, 30, 30 );
	sensorMICrms.connect( "MIC", "rmsvolume", "RMS volume", "%", AVERAGE, 30, 30 );
	sensorMICmax.connect( "MIC", "maxvolume", "Max volume", "%", MAX, 30, 30 );

	// Local Sensors connected to ESP
	sensorCCS.connect( 5, 30, 30, 1200 );
	sensorTSL.connect( 5, 30, 30 ); 
	sensorBMP.connect( 5, 30, 30 ); 
	sensorPIR.connect( 30 );

	// Reporting of sensor availability
	availReportTimer.setup( (unsigned long) AVAIL_REPORT_FREQ * 1000 );
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
		i2c.handle();
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