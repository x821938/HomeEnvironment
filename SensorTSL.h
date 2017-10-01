#ifndef _SENSORTSL_h
#define _SENSORTSL_h

#include "SensorGeneric.h"

#define GAIN_INCREASE_LIMIT 1000 // If either broadband or ir raw reading is below this, the sampletime or gain is increased.
const float sampleTimeMs[] = { 13.7, 101 ,402 }; // Possible sampling times in ms available on the tsl2541 chip



enum TslState {
	MS13_GAIN_1X,
	MS101_GAIN_1X,
	MS402_GAIN_1X,
	MS402_GAIN_16X
};



class SensorTSL : public SensorGeneric {
	protected:
		bool isSetup = false;

		bool gain = 0; // Start with low gain (0=1x, 1=16x)
		char sampleTime = 0; // Start with fast sampling (0=13.7ms, 1=101ms, 2=402ms)
		unsigned int measuredSampledTime;

		TslState tslState;
		unsigned long nextReadAt;

		void connect();
		virtual void readValue();
		bool findGain( uint16_t broadbandRaw, uint16_t irRaw );

	public:
		void setup();
};


#endif