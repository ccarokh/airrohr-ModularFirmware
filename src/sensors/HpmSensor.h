#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_HPM
// Honeywell HPM particulate matter sensor (UART). Port of fetchSensorHPM().
// Provides HPM_P1 (PM10) and HPM_P2 (PM2.5) - value_type mapping as in the original.

#include "Sensor.h"
#include "../Board.h"

class HpmSensor : public ISensor {
public:
	HpmSensor(SensorSerial &serial, int8_t rx, int8_t tx)
		: _serial(serial), _rx(rx), _tx(tx) {}

	const char *name() const override { return "HPM"; }
	int apiPin() const override { return 1; }
	bool begin() override;
	void update(unsigned long ms_since_start) override;
	void collect(std::vector<Reading> &out) override;

private:
	enum class Cmd { Start, Stop, ContinuousMode };
	bool cmd(Cmd c);
	void resetFrame();
	void resetAccumulators();

	SensorSerial &_serial;
	int8_t _rx, _tx;
	bool   _running = true;

	int  _len = 0;
	int  _pm10 = 0, _pm25 = 0;
	int  _checksum_is = 0, _checksum_should = 0;

	long _pm10_sum = 0, _pm25_sum = 0;
	long _pm10_min = 20000, _pm25_min = 20000;
	long _pm10_max = 0, _pm25_max = 0;
	int  _val_count = 0;
};

#endif // FEATURE_SENSOR_HPM