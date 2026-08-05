#include "../Features.h"
#if FEATURE_SENSOR_SDS
#include "SdsSensor.h"
#include "../Debug.h"
#include "../Config.h"

namespace {
constexpr unsigned long WARMUPTIME_MS = 15000; // lead time for "warming up"
constexpr unsigned long READINGTIME_MS = 5000; // duration of the reading window
}

// Send a 19-byte command frame to the SDS.
void SdsSensor::rawcmd(uint8_t h1, uint8_t h2, uint8_t h3)
{
	uint8_t buf[19];
	buf[0] = 0xAA;
	buf[1] = 0xB4;
	buf[2] = h1;
	buf[3] = h2;
	buf[4] = h3;
	for (unsigned i = 5; i < 15; ++i) buf[i] = 0x00;
	buf[15] = 0xFF;
	buf[16] = 0xFF;
	buf[17] = h1 + h2 + h3 - 2;
	buf[18] = 0xAB;
	_serial.write(buf, sizeof(buf));
}

bool SdsSensor::cmd(Cmd c)
{
	switch (c) {
	case Cmd::Start:          rawcmd(0x06, 0x01, 0x01); break;
	case Cmd::Stop:           rawcmd(0x06, 0x01, 0x00); break;
	case Cmd::ContinuousMode: rawcmd(0x08, 0x01, 0x00); rawcmd(0x02, 0x01, 0x00); break;
	}
	return c != Cmd::Stop;
}

bool SdsSensor::checksumValid(const uint8_t (&data)[8])
{
	uint8_t sum = 0;
	for (unsigned i = 0; i < 6; ++i) sum += data[i];
	return (data[7] == 0xAB && sum == data[6]);
}

void SdsSensor::resetAccumulators()
{
	_pm10_sum = _pm25_sum = 0;
	_val_count = 0;
	_pm10_max = _pm25_max = 0;
	_pm10_min = _pm25_min = 20000;
}

bool SdsSensor::begin()
{
	_serial.begin(9600, SENSOR_SERIAL_8N1, _rx, _tx);
	cmd(Cmd::ContinuousMode);
	_running = cmd(Cmd::Stop);
	return true;
}

void SdsSensor::update(unsigned long ms_since_start)
{
	const unsigned long interval = cfg.sending_intervall_ms;

	if (interval > (WARMUPTIME_MS + READINGTIME_MS) &&
		ms_since_start < (interval - (WARMUPTIME_MS + READINGTIME_MS))) {
		// Outside the warmup/reading window: turn the sensor off.
		if (_running) {
			_running = cmd(Cmd::Stop);
		}
		return;
	}

	// Inside the window: turn the sensor on and read frames.
	if (!_running) {
		_running = cmd(Cmd::Start);
		_waiting_for = REPLY_HDR;
	}

	while (_serial.available() >= _waiting_for) {
		static constexpr uint8_t hdr_measurement[2] = {0xAA, 0xC0};
		uint8_t data[8];

		switch (_waiting_for) {
		case REPLY_HDR:
			if (_serial.find(hdr_measurement, sizeof(hdr_measurement)))
				_waiting_for = REPLY_BODY;
			break;
		case REPLY_BODY:
			if (_serial.readBytes(data, sizeof(data)) == sizeof(data) && checksumValid(data)) {
				const uint32_t pm25 = data[0] | (data[1] << 8);
				const uint32_t pm10 = data[2] | (data[3] << 8);
				if (ms_since_start > (interval - READINGTIME_MS)) {
					_pm10_sum += pm10;
					_pm25_sum += pm25;
					if (pm10 < _pm10_min) _pm10_min = pm10;
					if (pm10 > _pm10_max) _pm10_max = pm10;
					if (pm25 < _pm25_min) _pm25_min = pm25;
					if (pm25 > _pm25_max) _pm25_max = pm25;
					_val_count++;
				}
			}
			_waiting_for = REPLY_HDR;
			break;
		}
	}
}

void SdsSensor::collect(std::vector<Reading> &out)
{
	uint32_t pm10_sum = _pm10_sum;
	uint32_t pm25_sum = _pm25_sum;
	uint16_t count = _val_count;

	if (count > 2) {
		// Discard the extreme values (min/max).
		pm10_sum = pm10_sum - _pm10_min - _pm10_max;
		pm25_sum = pm25_sum - _pm25_min - _pm25_max;
		count = count - 2;
	}

	if (count > 0) {
		const float p1 = float(pm10_sum) / (count * 10.0f);
		const float p2 = float(pm25_sum) / (count * 10.0f);
		out.emplace_back(F("SDS_P1"), p1, 2);
		out.emplace_back(F("SDS_P2"), p2, 2);
		if (_val_count < 3) _error_count++;
	} else {
		_error_count++;
		LOG_ERROR(F("SDS011: keine gültigen Werte im Zyklus"));
	}

	resetAccumulators();

	if (cfg.sending_intervall_ms > (WARMUPTIME_MS + READINGTIME_MS) && _running) {
		_running = cmd(Cmd::Stop);
	}
}
#endif // FEATURE_SENSOR_SDS
