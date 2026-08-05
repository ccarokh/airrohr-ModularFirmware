#pragma once
#include "../Features.h"
#if FEATURE_SENDER_MQTT
// Internal MQTT sender. Publishes one topic per measured value
// (<topic>/<node-id>/<sensor>/<measurement> = value) and optionally
// Home Assistant MQTT discovery configuration topics.
//
// This is the feature that was deliberately left out of the original firmware
// (issue #33: memory/inconsistent formats) - implemented here for ESP32.

#include "DataSender.h"

class MqttSender : public IDataSender {
public:
	const char *name() const override { return "MQTT"; }
	bool isMqtt() const override { return true; }
	bool enabled() const override;
	bool send(const std::vector<SensorReadings> &groups, const String &json) override;

private:
	// Derive the unit_of_measurement for the HA-Discovery from the value_type.
	static const char *unitForValueType(const String &vt);
};

#endif // FEATURE_SENDER_MQTT