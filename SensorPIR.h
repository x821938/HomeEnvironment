#ifndef _SENSORPIR_h
#define _SENSORPIR_h

#include <Arduino.h>
#define PIR_PIN	D5

class SensorPIRClass
{
 protected:
	 bool lastPirValue = 0;
	 long motionTime = 0;
	 long motionStarted = 0;

	 uint16_t reportFreq; // How often readings should be reported via mqtt. In seconds
	 long lastReported;

	 void sendPIR();
	 void sendPIRMotionTime();
 public:
	 void connect( uint16_t reportFreq );
	 void handle();
	 
};


#endif

