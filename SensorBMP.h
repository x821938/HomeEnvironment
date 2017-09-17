#ifndef _SENSORBMP_h
#define _SENSORBMP_h

#include <Arduino.h>
#include "Timing.h"
#include "SensorGeneric.h"


class SensorBMP
{
 protected:
	 bool isSetup = false;
	 SensorGeneric sensorBMPpressure;
	 SensorGeneric sensorBMPtemp;

	 Timing meassureTimer;

	 void reconnect();
	 void readTemp();
	 void readPressure();

 public:
	void connect( uint16_t meassureFreq, uint16_t reportFreq, uint16_t lostAfter );
	void handle();
	bool isSensorAlive();
};

#endif

