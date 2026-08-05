#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_SPS30
// Sensirion SPS30 particulate matter sensor (I2C). Ported from fetchSensorSPS30() +
// initSPS30(). The sensor measures continuously; update() collects each measurement,
// collect() computes the average. Provides mass (P*) and count (N*) + TPS.

#include "Sensor.h"

class SpsSensor : public ISensor {
public:
	const char *name() const override { return "SPS30"; }
	int apiPin() const override { return 1; }
	bool begin() override;
	void update(unsigned long ms_since_start) override;
	void collect(std::vector<Reading> &out) override;

private:
	void resetAccumulators();

	bool  _ok = false;
	float _p0 = 0, _p1 = 0, _p2 = 0, _p4 = 0;
	float _n05 = 0, _n1 = 0, _n25 = 0, _n4 = 0, _n10 = 0;
	float _tps = 0;
	unsigned long _count = 0;
};

#endif // FEATURE_SENSOR_SPS30