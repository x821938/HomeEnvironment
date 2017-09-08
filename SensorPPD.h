#ifndef _SENSORPPD_h
#define _SENSORPPD_h

#include "SlaveI2C.h""

#define PPD_I2C_ADDR 8

class SensorPPDClass
{
 protected:
	 SlaveI2CClass slaveI2C;

	 float dustAcc = 0;
	 uint16_t dustMeasurements = 0;

	 uint16_t sensorErrors = 0;

	 uint16_t meassureFreq; // How often the sensor should be read from device. In seconds
	 uint16_t reportFreq; // How often readings should be reported via mqtt. In seconds
	 long lastReported;
	 long lastMeassured;

	 float readSensor();
	 float getAvgDust();
	 void sendDust();

public:
	void connect( uint16_t meassureFreq, uint16_t reportFreq );
	void handle();
	bool isSensorAlive();
};


#endif

