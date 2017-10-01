#ifndef _SENSORGENERIC_h
#define _SENSORGENERIC_h

#include <Arduino.h>


enum SensorState {
	SensorWarmup,
	SensorRead,
	SensorWaitMqtt,
	SensorSend,
	SensorSentMqtt
};



class SensorGeneric {
	protected:
		SensorState sensorState;
		unsigned long warmupStartetAt;

		double sensorValue = 0;
		unsigned long warmupTime = 0;
		String shortName;	// Used for debug logging
		String mqttName;	// The name of the sensor presented over MQTT
		String friendlyName; // Used for debug logging
		String unit; // Used for debug logging

		void sendValue();
		virtual void readValue();

	public:
		void setup();
		void setup( unsigned long warmupTime, String shortName, String friendlyName, String mqttName, String unit );
		void handle();
		void putValue( double value );
		bool isValueSent();
		bool isWarmedUp();
};


#endif