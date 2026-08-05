#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_PPD
// Shinyei PPD42NS particulate matter sensor (pulse/LPO measurement). Ported from
// fetchSensorPPD(). Measures the "low pulse occupancy" on two GPIOs over a
// fixed sample window and computes the particle concentration from it.

#include "Sensor.h"

class PpdSensor : public ISensor {
public:
	PpdSensor(uint8_t pinP1, uint8_t pinP2) : _pinP1(pinP1), _pinP2(pinP2) {}

	const char *name() const override { return "PPD42NS"; }
	int apiPin() const override { return 5; }
	bool begin() override;
	void update(unsigned long ms_since_start) override;
	void collect(std::vector<Reading> &out) override;

private:
	uint8_t _pinP1, _pinP2;
	bool          _trigP1 = false, _trigP2 = false;
	unsigned long _trigOnP1 = 0, _trigOnP2 = 0;
	unsigned long _lpoP1 = 0, _lpoP2 = 0;
};

#endif // FEATURE_SENSOR_PPD