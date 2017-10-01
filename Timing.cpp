#include "Timing.h"

unsigned long Timing::globalLastTriggered = millis();



/* Setup a timer to trigger with a provided trigger frequency */
void Timing::setup( unsigned long triggerFreq ) {
	_triggerFreq = triggerFreq;
	lastTriggered = millis();
}



/* This should be called frequently. It returns true if time of frequency has passed.
   For everything not to trigger at almost the same time, it spaces the triggering out with the value of 
   MIN_TIME_BETWEEN_GLOBAL_TRIGGERS. */
bool Timing::triggered() {
	if ( millis() - lastTriggered >= _triggerFreq ) {
		if ( millis() - globalLastTriggered > MIN_TIME_BETWEEN_GLOBAL_TRIGGERS ) {
			lastTriggered = globalLastTriggered = millis();
			return true;
		}
	}
	return false;
}