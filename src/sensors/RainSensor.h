#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_RAIN
// LM393 rain sensor (rain board + comparator module). Two outputs:
//   AO - analog, the wetter the plate, the LOWER the voltage
//   DO - digital threshold, set with the potentiometer on the module
//        (LOW = wet, the module pulls it down)
//
// Not part of sensor.community (no API pin) -> apiPin() = 0. The values go to
// MQTT/InfluxDB/custom API/CSV like every other reading.

#include "Sensor.h"

class RainSensor : public ISensor {
public:
	RainSensor(uint8_t pinAo, uint8_t pinDo) : _pinAo(pinAo), _pinDo(pinDo) {}

	const char *name() const override { return "RAIN"; }
	bool canReadInstant() const override { return true; }
	bool begin() override;
	void collect(std::vector<Reading> &out) override;

private:
	uint8_t _pinAo;
	uint8_t _pinDo;
};

#endif // FEATURE_SENSOR_RAIN
