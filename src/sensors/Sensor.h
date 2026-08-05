#pragma once
// Sensor abstraction. Each physical sensor implements ISensor and provides
// a list of readings (value_type -> value) per measurement cycle. Senders then
// format these readings (sensor.community JSON, InfluxDB line, MQTT topics ...).

#include <Arduino.h>
#include <vector>

// A single reading. value_type follows the well-known convention
// ("SDS_P1", "temperature", "BME280_pressure", ...), so that the sender formats
// (and thus the existing backends) remain unchanged.
struct Reading {
	String value_type;
	String value;

	Reading() = default;
	Reading(const String &t, const String &v) : value_type(t), value(v) {}
	Reading(const __FlashStringHelper *t, float v, uint8_t decimals = 2)
		: value_type(t), value(String(v, (unsigned int)decimals)) {}
};

// Readings of a single sensor (for per-sensor upload, e.g. sensor.community).
struct SensorReadings {
	const char *sensor;        // Sensor name
	int apiPin;                // sensor.community pin (0 = do not send)
	bool available = true;     // Sensor found/initialized at begin()?
	std::vector<Reading> readings;
};

// Flattens the groups into a single reading list (definition in SensorManager.cpp).
std::vector<Reading> flattenReadings(const std::vector<SensorReadings> &groups);

class ISensor {
public:
	virtual ~ISensor() {}

	// Found/initialized at begin()? Set by the SensorManager.
	bool available = false;

	// Display name (for logs).
	virtual const char *name() const = 0;

	// sensor.community "pin" of this sensor (identifies the measurement type on
	// upload). 0 = do not send to sensor.community.
	virtual int apiPin() const { return 0; }

	// true = collect() returns an immediate instantaneous value (I2C climate sensors)
	// and may be called at any time (e.g. for /values live). false =
	// needs the measurement/sending window (particulate matter, accumulating) -> only within the cycle.
	virtual bool canReadInstant() const { return false; }

	// Initialize hardware. false = sensor not available.
	virtual bool begin() = 0;

	// Optional: call continuously in loop() (e.g. UART sensors that
	// collect samples over the measurement window). ms_since_start = time since the start
	// of the current measurement cycle. Default: nothing to do.
	virtual void update(unsigned long ms_since_start) { (void)ms_since_start; }

	// At the end of the cycle: append the readings.
	virtual void collect(std::vector<Reading> &out) = 0;
};
