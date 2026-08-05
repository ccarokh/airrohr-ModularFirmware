#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_HTU21D
// HTU21D temperature/humidity sensor. Port of fetchSensorHTU21D().

#include "Sensor.h"
#include <Adafruit_HTU21DF.h>

class Htu21dSensor : public ISensor {
public:
	const char *name() const override { return "HTU21D"; }
	bool canReadInstant() const override { return true; }
	int apiPin() const override { return 7; }
	bool begin() override;
	void collect(std::vector<Reading> &out) override;

private:
	Adafruit_HTU21DF _htu;
	bool _ok = false;
};

#endif // FEATURE_SENSOR_HTU21D