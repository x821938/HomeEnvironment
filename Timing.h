#ifndef _TIMING_h
#define _TIMING_h

#include <Arduino.h>

#define MIN_TIME_BETWEEN_GLOBAL_TRIGGERS 300 // minimum time in ms between all triggers. makes sure that things are not sent out in bulk but spread a little



class Timing {
	protected:
		unsigned long _triggerFreq;
		unsigned long lastTriggered;

	public:
		static unsigned long globalLastTriggered;

		void setup( unsigned long triggerFreq );
		bool triggered();
};


#endif

