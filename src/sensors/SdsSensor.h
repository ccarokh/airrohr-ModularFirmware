#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_SDS
// SDS011 particulate matter sensor (UART). Ported from fetchSensorSDS() + SDS_cmd().
//
// Flow over the measurement cycle (identical to the original firmware):
//  - update(): turns the sensor on in time before the reading window (warmup),
//    reads frames and averages the values within the reading window (last READINGTIME_MS).
//  - collect(): computes the trimmed mean (without min/max) and provides
//    SDS_P1 (PM10) and SDS_P2 (PM2.5).

#include "Sensor.h"
#include "../Board.h"

class SdsSensor : public ISensor {
public:
	SdsSensor(SensorSerial &serial, int8_t rx, int8_t tx)
		: _serial(serial), _rx(rx), _tx(tx) {}

	const char *name() const override { return "SDS011"; }
	int apiPin() const override { return 1; }
	bool begin() override;
	void update(unsigned long ms_since_start) override;
	void collect(std::vector<Reading> &out) override;

private:
	enum WaitFor : uint8_t { REPLY_HDR = 10, REPLY_BODY = 8 };
	enum class Cmd { Start, Stop, ContinuousMode };

	void    rawcmd(uint8_t h1, uint8_t h2, uint8_t h3);
	bool    cmd(Cmd c);
	static bool checksumValid(const uint8_t (&data)[8]);
	void    resetAccumulators();

	SensorSerial &_serial;
	int8_t _rx, _tx;

	bool     _running = true;
	WaitFor  _waiting_for = REPLY_HDR;
	uint32_t _pm10_sum = 0, _pm25_sum = 0;
	uint32_t _pm10_min = 20000, _pm25_min = 20000;
	uint32_t _pm10_max = 0, _pm25_max = 0;
	uint16_t _val_count = 0;
	uint16_t _error_count = 0;
};

#endif // FEATURE_SENSOR_SDS