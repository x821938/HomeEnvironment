#ifndef _SENSORCCS_h
#define _SENSORCCS_h

#include <Arduino.h>
#include "Timing.h"

#define CCS_ADDR		0x5A
#define CCS_WARMUP_TIME	1200 // 20 min according to datasheet
#define CCS_CALIB_FREQ  1000 


class SensorCCSClass
{
 protected:
	 bool isSetup = false;

	 Timing reportTimer;
	 Timing meassureTimer;
	 Timing warmupTimer;
	 Timing calibrateTimer;

	 long co2Acc = 0;
	 uint16_t co2Measurements = 0;

	 long tvocAcc = 0;
	 uint16_t tvocMeasurements = 0;

	 uint16_t sensorErrors = 0;
	 bool sensorWarmedUp = false;

	 float calibTemp;
	 float calibHumidity;

	 void reconnect();
	 void readSensor();
	 float getAvgCo2();
	 float getAvgTvoc();
	 void sendCo2();
	 void sendTvoc();
	 void doCalibrate();

 public:
	void connect( uint16_t meassureFreq, uint16_t reportFreq );
	void handle( float calibTemp, float calibHumidity );
	bool isSensorAlive();
};


#endif

