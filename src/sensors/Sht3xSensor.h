#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_SHT3X
// SHT3x temperature/humidity sensor. Port of fetchSensorSHT3x().

#include "Sensor.h"
#include <Adafruit_SHT31.h>

class Sht3xSensor : public ISensor {
public:
	const char *name() const override { return "SHT3X"; }
	bool canReadInstant() const override { return true; }
	int apiPin() const override { return 7; }
	bool begin() override;
	void collect(std::vector<Reading> &out) override;

private:
	Adafruit_SHT31 _sht;
	bool _ok = false;
};

#endif // FEATURE_SENSOR_SHT3X