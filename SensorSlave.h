#ifndef _SENSORSLAVE_h
#define _SENSORSLAVE_h

#include "SensorGeneric.h"


#define I2C_SLAVE_ADDRESS 8
#define SLAVE_WAIT_TIME 100UL



class SensorSlave {
	protected:
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