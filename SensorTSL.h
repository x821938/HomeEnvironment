#ifndef _SENSORTSL_h
#define _SENSORTSL_h

#include <Arduino.h>
#include "Timing.h"

#define SAMPLE_TIME 2 // 2=402ms most precise from TSL sensor
#define HIGH_GAIN_LIMIT 500 // going under XXX lux we turn up the gain.


class SensorTSLClass
{
protected:
	bool isSetup = false;

	Timing reportTimer;
	Timing meassureTimer;

	 double lightAcc = 0;
	 uint16_t lightMeasurements = 0;

	 long rawLightBroadbandAcc = 0;
	 long rawLightInfraredAcc = 0;
	 uint16_t rawLightMeasurements = 0;

	 uint16_t sensorErrors = 0;

	 bool gain = 1; // Start with high gain (x16)
	 unsigned int sampledTime;

	 void reconnect();
	 void readSensor();
	 double getAvgLight();
	 float getAvgIrPercentage();
	 void sendLight();
	 void sendIrPercentage();

public:
	 void connect( uint16_t meassureFreq, uint16_t reportFreq );
	 void handle();
	 bool isSensorAlive();
};


#endif

