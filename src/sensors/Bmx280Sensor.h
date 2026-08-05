#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_BMX280
// BMP280/BME280 sensor. Ported from fetchSensorBMX280() + initBMX280().
// Detects BME (with humidity) vs. BMP based on the sensor ID.

#include "Sensor.h"
#include <bmx280_i2c.h>

class Bmx280Sensor : public ISensor {
public:
	const char *name() const override { return "BMx280"; }
	bool canReadInstant() const override { return true; }
	int apiPin() const override { return _isBme ? 11 : 3; } // BME280=11, BMP280=3
	bool begin() override;
	void collect(std::vector<Reading> &out) override;

private:
	bool tryInit(uint8_t addr);

	BMX280 _bmx;
	bool _ok = false;
	bool _isBme = false;
};

#endif // FEATURE_SENSOR_BMX280