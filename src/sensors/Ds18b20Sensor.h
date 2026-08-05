#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_DS18B20
// DS18B20 temperature sensor (OneWire). Port of fetchSensorDS18B20().

#include "Sensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class Ds18b20Sensor : public ISensor {
public:
	explicit Ds18b20Sensor(uint8_t pin) : _oneWire(pin), _ds(&_oneWire) {}

	const char *name() const override { return "DS18B20"; }
	bool canReadInstant() const override { return true; }
	int apiPin() const override { return 13; }
	bool begin() override;
	void collect(std::vector<Reading> &out) override;

private:
	OneWire           _oneWire;
	DallasTemperature _ds;
	bool              _ok = false;
};

#endif // FEATURE_SENSOR_DS18B20