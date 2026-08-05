#pragma once
// Sender abstraction. Every sender receives the raw readings AND the finished
// sensordatavalues JSON and produces its target format from them:
//  - MqttSender:  one topic per reading (+ optional HA-Discovery)
//  - InfluxSender: InfluxDB line protocol from the readings
//  - HttpJsonSender/SensorCommunity: JSON POST
//
// This keeps the format logic encapsulated per target, and the sensors know
// nothing about output formats.

#include <Arduino.h>
#include <vector>
#include "../sensors/Sensor.h"

class IDataSender {
public:
	virtual ~IDataSender() {}

	// Display name (for logs).
	virtual const char *name() const = 0;

	// Is this sender active according to the configuration?
	virtual bool enabled() const = 0;

	// MQTT runs last and carries along the send status of the others.
	virtual bool isMqtt() const { return false; }

	// Send data. groups = readings per sensor (for per-sensor upload),
	// json = finished sensordatavalues payload. true on success.
	virtual bool send(const std::vector<SensorReadings> &groups, const String &json) = 0;
};
