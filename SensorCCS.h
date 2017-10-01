#ifndef _SENSORCCS_h
#define _SENSORCCS_h

#include <Arduino.h>
#include "Timing.h"
#include "SensorGeneric.h"

#define CCS_I2C_ADDR 0x5A
#define CCS_CALIB_FREQ  1000 //TODO: calibration



class SensorCCS
{
	protected:
		 bool isSetup = false;
		 bool isConnected = false;
		 bool needsReset;

		 SensorGeneric sensorCCSco2;
		 SensorGeneric sensorCCStvoc;

		 void connect();
		 void readSensor();

	public:
		void setup();
		void handle();
		bool isValueSent();
		void calibrate( float humidity, float temperature );
};


#endif