#ifndef _SENSORPIR_h
#define _SENSORPIR_h

#include <Arduino.h>
#include "Timing.h"

#define PIR_PIN	D0


class SensorPIRClass
{
 protected:
	 bool isSetup = false;

	 Timing reportTimer;

	 bool lastPirValue = 0;
	 long motionTime = 0;
	 long motionStartedAt = 0;

	 void sendPIR();
	 void sendPIRMotionTime();
 public:
	 void connect( uint16_t reportFreq );
	 void handle();
	 
};

#endif

