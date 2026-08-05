#include "../Features.h"
#if FEATURE_SENSOR_PMS
#include "PmsSensor.h"
#include "../Debug.h"
#include "../Config.h"

namespace {
constexpr unsigned long WARMUPTIME_MS = 15000;
constexpr unsigned long READINGTIME_MS = 5000;
}

bool PmsSensor::cmd(Cmd c)
{
	static const uint8_t start_cmd[]      = {0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74};
	static const uint8_t stop_cmd[]       = {0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73};
	static const uint8_t continuous_cmd[] = {0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71};
	switch (c) {
	case Cmd::Start:          _serial.write(start_cmd, sizeof(start_cmd)); break;
	case Cmd::Stop:           _serial.write(stop_cmd, sizeof(stop_cmd)); break;
	case Cmd::ContinuousMode: _serial.write(continuous_cmd, sizeof(continuous_cmd)); break;
	}
	return c != Cmd::Stop;
}

void PmsSensor::resetFrame()
{
	_len = 0;
	_pm1 = _pm10 = _pm25 = 0;
	_checksum_is = 0;
}

void PmsSensor::resetAccumulators()
{
	_pm1_sum = _pm10_sum = _pm25_sum = 0;
	_val_count = 0;
	_pm1_min = _pm10_min = _pm25_min = 20000;
	_pm1_max = _pm10_max = _pm25_max = 0;
}

bool PmsSensor::begin()
{
	_serial.begin(9600, SENSOR_SERIAL_8N1, _rx, _tx);
	_frame_len = 24;
	return true;
}

void PmsSensor::update(unsigned long ms_since_start)
{
	const unsigned long interval = cfg.sending_intervall_ms;

	if (ms_since_start < (interval - (WARMUPTIME_MS + READINGTIME_MS))) {
		if (_running) _running = cmd(Cmd::Stop);
		return;
	}
	if (!_running) _running = cmd(Cmd::Start);

	while (_serial.available() > 0) {
		const int value = _serial.read();
		switch (_len) {
		case 0:  if (value != 66) _len = -1; break;   // 'B'
		case 1:  if (value != 77) _len = -1; break;   // 'M'
		case 2:  _checksum_is = value; break;
		case 3:  _frame_len = value + 4; break;
		case 10: _pm1 += (value << 8); break;
		case 11: _pm1 += value; break;
		case 12: _pm25 = (value << 8); break;
		case 13: _pm25 += value; break;
		case 14: _pm10 = (value << 8); break;
		case 15: _pm10 += value; break;
		case 22: if (_frame_len == 24) _checksum_should = (value << 8); break;
		case 23: if (_frame_len == 24) _checksum_should += value; break;
		case 30: _checksum_should = (value << 8); break;
		case 31: _checksum_should += value; break;
		}
		if ((_len > 2) && (_len < (_frame_len - 2))) {
			_checksum_is += value;
		}
		_len++;
		if (_len == _frame_len) {
			bool checksum_ok = (_checksum_should == (_checksum_is + 143));
			if (!checksum_ok) {
				_len = 0;
			}
			if (checksum_ok && (ms_since_start > (interval - READINGTIME_MS))) {
				_pm1_sum += _pm1;
				_pm10_sum += _pm10;
				_pm25_sum += _pm25;
				if (_pm1 < _pm1_min) _pm1_min = _pm1;
				if (_pm1 > _pm1_max) _pm1_max = _pm1;
				if (_pm25 < _pm25_min) _pm25_min = _pm25;
				if (_pm25 > _pm25_max) _pm25_max = _pm25;
				if (_pm10 < _pm10_min) _pm10_min = _pm10;
				if (_pm10 > _pm10_max) _pm10_max = _pm10;
				_val_count++;
				resetFrame();
			}
		}
		yield();
	}
}

void PmsSensor::collect(std::vector<Reading> &out)
{
	long pm1 = _pm1_sum, pm10 = _pm10_sum, pm25 = _pm25_sum;
	int count = _val_count;
	if (count > 2) {
		pm1  = pm1  - _pm1_min  - _pm1_max;
		pm10 = pm10 - _pm10_min - _pm10_max;
		pm25 = pm25 - _pm25_min - _pm25_max;
		count -= 2;
	}
	if (count > 0) {
		out.emplace_back(F("PMS_P0"), float(pm1) / count, 2);
		out.emplace_back(F("PMS_P1"), float(pm10) / count, 2);
		out.emplace_back(F("PMS_P2"), float(pm25) / count, 2);
	} else {
		LOG_ERROR(F("PMSx003: keine gültigen Werte im Zyklus"));
	}
	resetAccumulators();
	if (cfg.sending_intervall_ms > (WARMUPTIME_MS + READINGTIME_MS)) {
		_running = cmd(Cmd::Stop);
	}
}
#endif // FEATURE_SENSOR_PMS
