#ifndef _SENSORPIR_h
#define _SENSORPIR_h

#include <Arduino.h>
#include "Timing.h"

#define PIR_PIN	16


class SensorPIR
{
 protected:
	 bool isSetup = false;

	 Timing reportTimer;

	 bool lastPirValue = 0;
	 unsigned long motionTime = 0;
	 unsigned long motionStartedAt = 0;

	 void sendPIR();
	 void sendPIRMotionTime();
 public:
	 void connect( uint16_t reportFreq );
	 void handle();
	 
};

#endif

