#ifndef _SENSORCCS_h
#define _SENSORCCS_h

#include <Arduino.h>
#include "Timing.h"
#include "GenericSensor.h"

#define CCS_ADDR		0x5A
#define CCS_CALIB_FREQ  1000 
#define CCS_RECONNECTION_TIME 5

class SensorCCSClass
{
 protected:
	 bool isSetup = false;
	 bool isConnected = false;
	 GenericSensorClass sensorCCSco2;
	 GenericSensorClass sensorCCStvoc;

	 Timing connectionTimer;
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
	void connect( uint16_t meassureFreq, uint16_t reportFreq, uint16_t deviceLostAfter, uint16_t warmedUpAfter );
	void handle( float calibTemp, float calibHumidity );
	bool isSensorAlive();
};


#endif

