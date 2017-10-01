#ifndef _SENSORSLAVE_h
#define _SENSORSLAVE_h

#include "SensorGeneric.h"
#include "SlaveI2C.h"

#define I2C_SLAVE_ADDRESS 8
#define REQUEST_DATA_PIN 12 // Pin to pull low to tell slave we want data
#define SLAVE_WAIT_TIME 100UL // Time to let slave finish it's readings.



class SensorSlave {
	protected:
		SlaveI2C i2c;
		bool dataPolled = false;

		SensorGeneric sensorPPDdust;
		SensorGeneric sensorMICmax;
		SensorGeneric sensorMICavg;
		SensorGeneric sensorMICrms;
		SensorGeneric sensorDHTtemp;
		SensorGeneric sensorDHThum;
		SensorGeneric sensorPIRtime;
		SensorGeneric slaveUptime;

		float temperature;
		float humidity;

	public:
		void setup();
		void handle();
		bool isValueSent();
		float getHumidity();
		float getTemperature();
};


#endif