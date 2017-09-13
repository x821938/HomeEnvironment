#ifndef _SENSORCCS_h
#define _SENSORCCS_h

#include <Arduino.h>
#include "Timing.h"
#include "GenericSensor.h"

#define CCS_ADDR		0x5A
#define CCS_WARMUP_TIME	1200 // 20 min according to datasheet
#define CCS_CALIB_FREQ  1000 
#define CCS_DEVICE_LOST_AFTER 20


class SensorCCSClass
{
 protected:
	 bool isSetup = false;
	 GenericSensorClass sensorCCSco2;
	 GenericSensorClass sensorCCStvoc;

	 Timing reportTimer;
	 Timing meassureTimer;
	 Timing warmupTimer;
	 Timing calibrateTimer;

	 bool sensorWarmedUp = false;

	 float calibTemp;
	 float calibHumidity;

	 void reconnect();
	 void readSensor();
	 void doCalibrate();

 public:
	void connect( uint16_t meassureFreq, uint16_t reportFreq, uint16_t deviceLostAfter );
	void handle( float calibTemp, float calibHumidity );
	bool isSensorAlive();
};


#endif

