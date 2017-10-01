#include "SensorSlave.h"
#include "SlaveI2C.h"
#include <Homie.h>
#include "Logging.h"



/* Sets up a generic sensor instance for each of the sensor values on the ATmega slave device 
   It also pulls the requestpin low to tell the slave that we want data after warmup period */
void SensorSlave::setup() {
	sensorPPDdust.setup( SLAVE_WAIT_TIME, "PPD", "dust level", "dust", "pcs/l" );
	sensorMICmax.setup( SLAVE_WAIT_TIME, "MIC", "sound max", "maxvolume", "%" );
	sensorMICavg.setup( SLAVE_WAIT_TIME, "MIC", "average sound", "avgvolume", "%" );
	sensorMICrms.setup( SLAVE_WAIT_TIME, "MIC", "sound rms", "rmsvolume", "%" );
	sensorDHTtemp.setup( SLAVE_WAIT_TIME, "DHT", "temperature", "temperatureDHT", "C" );
	sensorDHThum.setup( SLAVE_WAIT_TIME, "DHT", "humidity", "humidity", "%" );
	sensorPIRtime.setup( SLAVE_WAIT_TIME, "PIR", "motion time", "PIRmotiontime", "%" );
	slaveUptime.setup( SLAVE_WAIT_TIME, "SLV", "slave uptime", "slaveuptime", "min" );

	LOG_DEBUG( "SLV", "Pulling pin " << REQUEST_DATA_PIN << " low to tell slave that we want data" );
	pinMode( REQUEST_DATA_PIN, OUTPUT ); digitalWrite( REQUEST_DATA_PIN, LOW ); 

	dataPolled = false;
}



/* If warmup time has passed, we poll the slave for all the sensorvalues 
   And then we release the slave so that it can continue polling data */
void SensorSlave::handle() {
	sensorPPDdust.handle();
	sensorMICmax.handle();
	sensorMICavg.handle();
	sensorMICrms.handle();
	sensorDHTtemp.handle();
	sensorDHThum.handle();
	sensorPIRtime.handle();
	slaveUptime.handle();

	if ( dataPolled == false ) { // We only want to ask the slave once while we are awake.
		if ( sensorPPDdust.isWarmedUp() ) { // And only after the warmup time
			float readValue;
			if ( i2c.pollData( I2C_SLAVE_ADDRESS, 'D', &readValue, sizeof( readValue ) ) ) sensorPPDdust.putValue( readValue );
			if ( i2c.pollData( I2C_SLAVE_ADDRESS, 'M', &readValue, sizeof( readValue ) ) ) sensorMICmax.putValue( readValue );
			if ( i2c.pollData( I2C_SLAVE_ADDRESS, 'A', &readValue, sizeof( readValue ) ) ) sensorMICavg.putValue( readValue );
			if ( i2c.pollData( I2C_SLAVE_ADDRESS, 'R', &readValue, sizeof( readValue ) ) ) sensorMICrms.putValue( readValue );
			if ( i2c.pollData( I2C_SLAVE_ADDRESS, 'S', &readValue, sizeof( readValue ) ) ) sensorPIRtime.putValue( readValue );
			if ( i2c.pollData( I2C_SLAVE_ADDRESS, 'U', &readValue, sizeof( readValue ) ) ) slaveUptime.putValue( readValue );
			if ( i2c.pollData( I2C_SLAVE_ADDRESS, 'H', &humidity, sizeof( humidity ) ) ) sensorDHThum.putValue( humidity );
			if ( i2c.pollData( I2C_SLAVE_ADDRESS, 'T', &temperature, sizeof( temperature ) ) ) sensorDHTtemp.putValue( temperature );
			
			dataPolled = true; 

			LOG_DEBUG( "SLV", "Releasing the slave by setting pin " << REQUEST_DATA_PIN << " High" );
			digitalWrite( REQUEST_DATA_PIN, HIGH );
		}
	}
}



/* Returns true if all the MQTT values from slave sensors has been sent */
bool SensorSlave::isValueSent() {
	return sensorPPDdust.isValueSent() && sensorMICmax.isValueSent() && sensorMICavg.isValueSent() && \
		sensorMICrms.isValueSent() && sensorDHTtemp.isValueSent() && sensorDHThum.isValueSent();
}



float SensorSlave::getHumidity() {
	return humidity;
}



float SensorSlave::getTemperature() {
	return temperature;
}