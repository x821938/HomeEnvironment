#ifndef _SENSORTSL_h
#define _SENSORTSL_h

#include <Arduino.h>
#include "Timing.h"
#include "SensorGeneric.h"

#define SAMPLE_TIME 2 // 2=402ms most precise from TSL sensor
#define HIGH_GAIN_LIMIT 500 // going under XXX lux we turn up the gain.


class SensorTSL
{
protected:
	 bool isSetup = false;
	 SensorGeneric sensorTSLlight;
	 SensorGeneric sensorTSLirPct;

	 Timing meassureTimer;

	 bool gain = 1; // Start with high gain (x16)
	 unsigned int sampledTime;

	 void reconnect();
	 void readSensor();

	 float getIrPercentage( unsigned int broadband, unsigned int ir );

public:
	 void connect( uint16_t meassureFreq, uint16_t reportFreq, uint16_t deviceLostAfter );
	 void handle();
	 bool isSensorAlive();
};


#endif

