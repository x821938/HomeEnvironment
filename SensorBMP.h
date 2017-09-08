#ifndef _SENSORBMP_h
#define _SENSORBMP_h

#include <Arduino.h>

class SensorBMPClass
{
 protected:
	 float tempAcc = 0;
	 uint16_t tempMeasurements = 0;

	 float pressureAcc = 0;
	 uint16_t pressureMeasurements = 0;

	 uint16_t sensorErrors = 0;

	 uint16_t meassureFreq; // How often the sensor should be read from device. In seconds
	 uint16_t reportFreq; // How often readings should be reported via mqtt. In seconds
	 long lastReported;
	 long lastMeassured;

	 void readTemp();
	 void readPressure();
	 void sendTemperature();
	 void sendPressure();
 public:
	void connect( uint16_t meassureFreq, uint16_t reportFreq );
	void handle();
	float getAvgTemp();
	float getAvgPressure();
	bool isSensorAlive();
};

#endif

