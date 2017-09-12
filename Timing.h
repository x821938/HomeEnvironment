#ifndef _TIMING_h
#define _TIMING_h

#include <Arduino.h>

#define MIN_TIME_BETWEEN_GLOBAL_TRIGGERS 300 // minimum time in ms between all triggers. makes sure that things are not sent out in bulk but spread a little

class Timing {
protected:
	long _triggerFreq;
	long lastTriggered;

public:
	static long globalLastTriggered;

	void setup( long triggerFreq );
	bool triggered();
};


#endif

