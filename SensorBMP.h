#ifndef _SENSORBMP_h
#define _SENSORBMP_h

#include <Arduino.h>
#include "Timing.h"
#include "SensorGeneric.h"



class SensorBMP
{
	protected:
		bool isSetup;

		SensorGeneric sensorBMPpressure;
		SensorGeneric sensorBMPtemp;

		void connect();
		void readTemp();
		void readPressure();

	public:
		void setup();
		void handle();
		bool isValueSent();
};


#endif

