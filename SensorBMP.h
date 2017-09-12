#ifndef _SENSORBMP_h
#define _SENSORBMP_h

#include <Arduino.h>
#include "Timing.h"


class SensorBMPClass
{
 protected:
	 bool isSetup = false;

	 Timing reportTimer; 
	 Timing meassureTimer;

	 float tempAcc = 0;
	 uint16_t tempMeasurements = 0;

	 float pressureAcc = 0;
	 uint16_t pressureMeasurements = 0;

	 uint16_t sensorErrors = 0;

	 void reconnect();
	 void readTemp();
	 void readPressure();
	 void sendTemp();
	 void sendPressure();

 public:
	void connect( uint16_t meassureFreq, uint16_t reportFreq );
	void handle();
	float getAvgTemp();
	float getAvgPressure();
	bool isSensorAlive();
};

#endif

