#ifndef _SENSORTSL_h
#define _SENSORTSL_h

#include <Arduino.h>

#define SAMPLE_TIME 2 // 402ms most precise from TSL sensor
#define HIGH_GAIN_LIMIT 500 // at xxx lux we turn down or up the gain.

class SensorTSLClass
{
 protected:
	 double lightAcc;
	 uint16_t lightMeasurements;

	 long rawLightBroadbandAcc = 0;
	 long rawLightInfraredAcc = 0;
	 uint16_t rawLightMeasurements;

	 uint16_t sensorErrors = 0;

	 uint16_t meassureFreq; // How often the sensor should be read from device. In seconds
	 uint16_t reportFreq; // How often readings should be reported via mqtt. In seconds
	 long lastReported;
	 long lastMeassured;

	 bool gain = 1; // Start with high gain (x16)
	 unsigned int sampledTime;

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

