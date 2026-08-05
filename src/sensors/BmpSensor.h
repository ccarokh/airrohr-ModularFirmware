#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_BMP
// BMP180 barometric pressure/temperature sensor. Ported from fetchSensorBMP().

#include "Sensor.h"
#include <Adafruit_BMP085.h>

class BmpSensor : public ISensor {
public:
	const char *name() const override { return "BMP180"; }
	bool canReadInstant() const override { return true; }
	int apiPin() const override { return 3; }
	bool begin() override;
	void collect(std::vector<Reading> &out) override;

private:
	Adafruit_BMP085 _bmp;
	bool _ok = false;
};

#endif // FEATURE_SENSOR_BMP