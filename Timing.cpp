#include "Timing.h"


unsigned long Timing::globalLastTriggered = millis();


void Timing::setup( unsigned long triggerFreq ) {
	_triggerFreq = triggerFreq;
	lastTriggered = millis();
}


bool Timing::triggered() {
	if ( millis() - lastTriggered >= _triggerFreq ) {
		if ( millis() - globalLastTriggered > MIN_TIME_BETWEEN_GLOBAL_TRIGGERS ) {
			lastTriggered = globalLastTriggered = millis();
			return true;
		}
	}
	return false;
}