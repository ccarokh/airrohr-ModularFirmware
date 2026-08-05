#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_SCD30
// SCD30 CO2/temperature/humidity sensor. Port of fetchSensorSCD30().

#include "Sensor.h"
#include <SparkFun_SCD30_Arduino_Library.h>

class Scd30Sensor : public ISensor {
public:
	const char *name() const override { return "SCD30"; }
	bool canReadInstant() const override { return true; }
	int apiPin() const override { return 17; }
	bool begin() override;
	void collect(std::vector<Reading> &out) override;

private:
	SCD30 _scd;
	bool _ok = false;
};

#endif // FEATURE_SENSOR_SCD30