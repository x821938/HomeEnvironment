#ifndef _SENSORMIC_h
#define _SENSORMIC_h

#include "SlaveI2C.h"
#include <Arduino.h>

#define MIC_I2C_ADDR 8

struct MicData {
	uint32_t samples; // How many samples were taken since last poll of i2c microphone
	uint32_t sampleTime; // How long time the sampling has been going on
	float volAvgPtc; // Average volume in percent
	float volMaxPtc; // Max volume detected in sampling period in percent
	float volRmsPtc; // RMS meassured in sampling period in percent
};

class SensorMICClass
{
 protected:
	 SlaveI2CClass slaveI2C;

	 float volAvgAcc;
	 float volRmsAcc;
	 float volMax;
	 uint16_t volMeasurements;

	 uint16_t sensorErrors = 0;

	 uint16_t meassureFreq; // How often the sensor should be read from device. In seconds
	 uint16_t reportFreq; // How often readings should be reported via mqtt. In seconds
	 long lastReported;
	 long lastMeassured;

	 void readSensor();
	 float getAvgVol();
	 float getAvgRmsVol();
	 void sendSound();

 public:
	void connect( uint16_t meassureFreq, uint16_t reportFreq );
	void handle();
	bool isSensorAlive();
};

#endif

