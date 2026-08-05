#include "../Features.h"
#if FEATURE_SENSOR_NPM
#include "NpmSensor.h"
#include "../Debug.h"
#include "../Config.h"

namespace {
constexpr unsigned long WARMUPTIME_MS = 15000;
constexpr unsigned long READINGTIME_MS = 15000;
}

void NpmSensor::sendCmd(Cmd c)
{
	static const uint8_t state_cmd[]         = {0x81, 0x16, 0x69};
	static const uint8_t change_cmd[]        = {0x81, 0x15, 0x6A};
	static const uint8_t concentration_cmd[] = {0x81, 0x11, 0x6E};
	switch (c) {
	case Cmd::State:         _serial.write(state_cmd, sizeof(state_cmd)); break;
	case Cmd::Change:        _serial.write(change_cmd, sizeof(change_cmd)); break;
	case Cmd::Concentration: _serial.write(concentration_cmd, sizeof(concentration_cmd)); break;
	}
}

bool NpmSensor::checksumValid16(const uint8_t (&d)[16])
{
	uint8_t sum = 0;
	for (int i = 0; i < 16; ++i) sum += d[i];
	return (sum % 0x100) == 0;
}

bool NpmSensor::waitAvailable(uint8_t n, unsigned long timeout_ms)
{
	const unsigned long start = millis();
	while ((uint8_t)_serial.available() < n) {
		if (millis() - start > timeout_ms) return false;
		yield();
	}
	return true;
}

bool NpmSensor::startStop()
{
	// Send the toggle command and evaluate the reported state.
	sendCmd(Cmd::Change);
	if (!waitAvailable(4)) {
		LOG_WARN(F("NextPM: keine Antwort auf Change"));
		return _running;
	}
	const uint8_t header[2] = {0x81, 0x15};
	if (!_serial.find(header, sizeof(header))) return _running;

	uint8_t state[1];
	if (_serial.readBytes(state, 1) != 1) return _running;
	uint8_t checksum[1];
	_serial.readBytes(checksum, 1); // discard checksum

	if (bitRead(state[0], 0) == 0) return true;   // running
	return false;                                  // stopped
}

void NpmSensor::resetAccumulators()
{
	_pm1_sum = _pm10_sum = _pm25_sum = 0;
	_n1_sum = _n10_sum = _n25_sum = 0;
	_val_count = 0;
	_pm1_min = _pm10_min = _pm25_min = 20000;
	_pm1_max = _pm10_max = _pm25_max = 0;
	_n1_min = _n10_min = _n25_min = 60000;
	_n1_max = _n10_max = _n25_max = 0;
}

bool NpmSensor::begin()
{
	_serial.begin(115200, SENSOR_SERIAL_8N1, _rx, _tx);
	return true;
}

void NpmSensor::update(unsigned long ms_since_start)
{
	const unsigned long interval = cfg.sending_intervall_ms;

	if (interval > (WARMUPTIME_MS + READINGTIME_MS) &&
		ms_since_start < (interval - (WARMUPTIME_MS + READINGTIME_MS))) {
		if (_running && !cfg.npm_fulltime) {
			_running = startStop();
		}
		return;
	}

	if (!_running && !cfg.npm_fulltime) {
		_running = startStop();
		_waiting_for = HEADER_16;
	}
	if (_running && cfg.npm_fulltime) {
		_waiting_for = HEADER_16;
	}

	if (ms_since_start <= (interval - READINGTIME_MS)) {
		return;
	}

	sendCmd(Cmd::Concentration);
	if (!waitAvailable(HEADER_16)) {
		return;
	}

	while ((uint8_t)_serial.available() >= _waiting_for) {
		const uint8_t header[2] = {0x81, 0x11};
		static uint8_t state[1];
		static uint8_t data[12];
		uint8_t checksum[1];
		uint8_t test[16];

		switch (_waiting_for) {
		case HEADER_16:
			if (_serial.find(header, sizeof(header))) _waiting_for = STATE_16;
			break;
		case STATE_16:
			_serial.readBytes(state, sizeof(state));
			_waiting_for = BODY_16;
			break;
		case BODY_16:
			_serial.readBytes(data, sizeof(data));
			_waiting_for = CHECKSUM_16;
			break;
		case CHECKSUM_16: {
			_serial.readBytes(checksum, sizeof(checksum));
			memcpy(test, header, 2);
			memcpy(&test[2], state, 1);
			memcpy(&test[3], data, 12);
			memcpy(&test[15], checksum, 1);
			if (checksumValid16(test)) {
				const long n1  = word(data[0], data[1]);
				const long n25 = word(data[2], data[3]);
				const long n10 = word(data[4], data[5]);
				const long pm1  = word(data[6], data[7]);
				const long pm25 = word(data[8], data[9]);
				const long pm10 = word(data[10], data[11]);
				_pm1_sum += pm1; _pm25_sum += pm25; _pm10_sum += pm10;
				_n1_sum += n1;   _n25_sum += n25;   _n10_sum += n10;
				if (pm1 < _pm1_min) _pm1_min = pm1;   if (pm1 > _pm1_max) _pm1_max = pm1;
				if (pm25 < _pm25_min) _pm25_min = pm25; if (pm25 > _pm25_max) _pm25_max = pm25;
				if (pm10 < _pm10_min) _pm10_min = pm10; if (pm10 > _pm10_max) _pm10_max = pm10;
				if (n1 < _n1_min) _n1_min = n1;   if (n1 > _n1_max) _n1_max = n1;
				if (n25 < _n25_min) _n25_min = n25; if (n25 > _n25_max) _n25_max = n25;
				if (n10 < _n10_min) _n10_min = n10; if (n10 > _n10_max) _n10_max = n10;
				_val_count++;
			}
			_waiting_for = HEADER_16;
			break;
		}
		}
	}
}

void NpmSensor::collect(std::vector<Reading> &out)
{
	long pm1 = _pm1_sum, pm10 = _pm10_sum, pm25 = _pm25_sum;
	long n1 = _n1_sum, n10 = _n10_sum, n25 = _n25_sum;
	int count = _val_count;
	if (count > 2) {
		pm1 -= _pm1_min + _pm1_max; pm10 -= _pm10_min + _pm10_max; pm25 -= _pm25_min + _pm25_max;
		n1 -= _n1_min + _n1_max;    n10 -= _n10_min + _n10_max;    n25 -= _n25_min + _n25_max;
		count -= 2;
	}
	if (count > 0) {
		out.emplace_back(F("NPM_P0"), float(pm1) / (count * 10.0f), 2);
		out.emplace_back(F("NPM_P1"), float(pm10) / (count * 10.0f), 2);
		out.emplace_back(F("NPM_P2"), float(pm25) / (count * 10.0f), 2);
		out.emplace_back(F("NPM_N1"), float(n1) / (count * 1000.0f), 2);
		out.emplace_back(F("NPM_N10"), float(n10) / (count * 1000.0f), 2);
		out.emplace_back(F("NPM_N25"), float(n25) / (count * 1000.0f), 2);
	} else {
		LOG_ERROR(F("NextPM: keine gültigen Werte im Zyklus"));
	}
	resetAccumulators();

	if (cfg.sending_intervall_ms > (WARMUPTIME_MS + READINGTIME_MS) &&
		_running && !cfg.npm_fulltime) {
		_running = startStop();
	}
}
#endif // FEATURE_SENSOR_NPM
