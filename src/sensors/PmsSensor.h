#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_PMS
// Plantower PMSx003 particulate matter sensor (UART). Ported from fetchSensorPMS().
// Provides PMS_P0 (PM1), PMS_P1 (PM10), PMS_P2 (PM2.5).

#include "Sensor.h"
#include "../Board.h"

class PmsSensor : public ISensor {
public:
	PmsSensor(SensorSerial &serial, int8_t rx, int8_t tx)
		: _serial(serial), _rx(rx), _tx(tx) {}

	const char *name() const override { return "PMSx003"; }
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

	// Frame parser state (preserved across update() calls)
	int  _len = 0, _frame_len = 24;
	int  _pm1 = 0, _pm10 = 0, _pm25 = 0;
	int  _checksum_is = 0, _checksum_should = 0;

	// Accumulators over the reading window
	long _pm1_sum = 0, _pm10_sum = 0, _pm25_sum = 0;
	long _pm1_min = 20000, _pm10_min = 20000, _pm25_min = 20000;
	long _pm1_max = 0, _pm10_max = 0, _pm25_max = 0;
	int  _val_count = 0;
};

#endif // FEATURE_SENSOR_PMS