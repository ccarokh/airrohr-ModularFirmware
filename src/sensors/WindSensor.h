#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_WIND
// Analog anemometer (voltage output, fed through the R1/R2 divider on the
// carrier board). Sampled continuously over the measurement cycle:
//   wind_speed - mean over the cycle
//   wind_gust  - maximum single sample of the cycle
// That is why the sensor is NOT instant-capable - like the particulate matter
// sensors it needs its measurement window (a snapshot of the wind says little).
//
// Not part of sensor.community (no API pin) -> apiPin() = 0.

#include "Sensor.h"

class WindSensor : public ISensor {
public:
	explicit WindSensor(uint8_t pinAo) : _pinAo(pinAo) {}

	const char *name() const override { return "WIND"; }
	bool begin() override;
	void update(unsigned long ms_since_start) override;
	void collect(std::vector<Reading> &out) override;

private:
	// Raw value -> m/s using the calibration from the config.
	float speedFromRaw(int raw) const;

	uint8_t       _pinAo;
	unsigned long _last_sample = 0;
	double        _sum_speed = 0.0;   // for the mean
	double        _sum_volts = 0.0;
	unsigned long _samples = 0;
	float         _gust = 0.0f;
};

#endif // FEATURE_SENSOR_WIND
