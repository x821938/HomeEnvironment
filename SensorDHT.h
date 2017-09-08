#ifndef _SENSORDHT_h
#define _SENSORDHT_h

#include "SlaveI2C.h"
#include <Arduino.h>

#define DHT_I2C_ADDR 8

struct DHTSensorData {
	float temperature;
	float humidity;
	bool temperatureOk;
	bool humidityOk;
};

class SensorDHTClass
{
 protected:
	 SlaveI2CClass slaveI2C;
	 DHTSensorData dhtSensorData;

	 float tempAccu = 0;
	 uint16_t tempMeasurements = 0;

	 float humidityAcc = 0;
	 uint16_t humidityMeasurements = 0;

	 uint16_t sensorErrors = 0;

	 uint16_t meassureFreq; // How often the sensor should be read from device. In seconds
	 uint16_t reportFreq; // How often readings should be reported via mqtt. In seconds
	 long lastReported;
	 long lastMeassured;

	 void readSensor();
	 void sendTemp();
	 void sendHumidity();

 public:
	 void connect( uint16_t meassureFreq, uint16_t reportFreq );
	 void handle();
	 float getAvgTemp();
	 float getAvgHumidity();
	 bool isSensorAlive();
};

#endif

