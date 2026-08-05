#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_DHT
// DHT11/DHT22 temperature/humidity sensor. Port of fetchSensorDHT().

#include "Sensor.h"
#include <DHT.h>

class DhtSensor : public ISensor {
public:
	DhtSensor(uint8_t pin, uint8_t type) : _dht(pin, type) {}

	const char *name() const override { return "DHT"; }
	bool canReadInstant() const override { return true; }
	int apiPin() const override { return 7; }
	bool begin() override;
	void collect(std::vector<Reading> &out) override;

private:
	DHT   _dht;
	bool  _ok = false;
};

#endif // FEATURE_SENSOR_DHT