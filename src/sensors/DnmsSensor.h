#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_DNMS
// DNMS – Digital Noise Measurement Sensor (I2C). Ported from fetchSensorDNMS()
// + initDNMS(). Provides LAeq, LA_min, LA_max.

#include "Sensor.h"

class DnmsSensor : public ISensor {
public:
	const char *name() const override { return "DNMS"; }
	bool canReadInstant() const override { return true; }
	int apiPin() const override { return 15; }
	bool begin() override;
	void collect(std::vector<Reading> &out) override;

private:
	bool _ok = false;
};

#endif // FEATURE_SENSOR_DNMS