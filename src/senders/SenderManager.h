#pragma once
// Holds all senders and distributes a measurement record to the active targets.

#include "DataSender.h"
#include <memory>
#include <vector>

class SenderManager {
public:
	// Register all known senders.
	void begin();

	// Distribute a record (grouped per sensor) to all active senders.
	// HTTP/CSV senders run first, MQTT last and publishes along the way the
	// send status of the others (status/<backend> = ok/error).
	void dispatch(const std::vector<SensorReadings> &groups, const String &json);

	// Serve only the MQTT sender (for its own, shorter MQTT interval -
	// frequent live values without burdening the cloud backends). true if
	// something was sent.
	bool publishMqtt(const std::vector<SensorReadings> &groups, const String &json);

	// true if an MQTT sender is active/configured.
	bool mqttEnabled() const;

private:
	std::vector<std::unique_ptr<IDataSender>> _senders;
};
