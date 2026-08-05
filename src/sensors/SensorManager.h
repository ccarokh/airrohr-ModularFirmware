#pragma once
// Manages the sensors that are active depending on the configuration:
// initialize, update in loop(), collect readings at the end of the cycle.

#include "Sensor.h"
#include <memory>
#include <vector>

class SensorManager {
public:
	// Instantiate the active sensors based on the config and call begin().
	void begin();

	// Call in every loop() iteration (serves sampling UART sensors).
	void update();

	// true when the sending interval has elapsed.
	bool shouldSend() const;

	// Live availability of a sensor: -1 = not active, 0 = not found, 1 = ok.
	// (I2C sensors detect "not found" already at begin(), without a measurement cycle.)
	int availability(const char *name) const;

	// Instantaneous values of the instant-capable sensors (I2C climate) - for /values live.
	// Particulate matter sensors are NOT included (they need the measurement window).
	std::vector<SensorReadings> instantReadings();

	// Collect readings per sensor and restart the cycle.
	std::vector<SensorReadings> collect();

	size_t count() const { return _sensors.size(); }

private:
	std::vector<std::unique_ptr<ISensor>> _sensors;
	unsigned long _cycle_start = 0;
};

// Builds the sensor.community-compatible payload JSON from the readings:
// {"software_version":"...","sensordatavalues":[{"value_type":..,"value":..},..]}
String buildPayloadJson(const std::vector<Reading> &readings);
