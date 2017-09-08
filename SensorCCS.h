#ifndef _SENSORCCS_h
#define _SENSORCCS_h

#include <Arduino.h>

#define CCS_ADDR		0x5A
#define CCS_WARMUP_TIME	1200 // Seconds
#define CCS_CALIB_FREQ  1000 // Seconds

class SensorCCSClass
{
 protected:
	 long co2Acc = 0;
	 uint16_t co2Measurements = 0;

	 long tvocAcc = 0;
	 uint16_t tvocMeasurements = 0;

	 uint16_t sensorErrors = 0;
	 bool sensorWarmedUp = false;

	 float calibTemp;
	 float calibHumidity;

	 uint16_t meassureFreq; // How often the sensor should be read from device. In seconds
	 uint16_t reportFreq; // How often readings should be reported via mqtt. In seconds
	 long lastReported;
	 long lastMeassured;
	 long lastCalibrated;

	 void readSensor();
	 float getAvgCo2();
	 float getAvgTvoc();
	 void sendCo2();
	 void sendTvoc();
	 void calibrate();

 public:
	void connect( uint16_t meassureFreq, uint16_t reportFreq );
	void handle( float calibTemp, float calibHumidity );
	bool isSensorAlive();
};


#endif

