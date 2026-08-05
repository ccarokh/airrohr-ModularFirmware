#include "../Features.h"
#if FEATURE_SENSOR_SPS30
#include "SpsSensor.h"
#include "../Debug.h"

#include <sps30_i2c.h>

namespace {
constexpr uint32_t AUTO_CLEANING_INTERVAL = 604800; // 1 week (seconds)
}

void SpsSensor::resetAccumulators()
{
	_p0 = _p1 = _p2 = _p4 = 0;
	_n05 = _n1 = _n25 = _n4 = _n10 = 0;
	_tps = 0;
	_count = 0;
}

bool SpsSensor::begin()
{
	sps30_reset();
	delay(200);
	char serial[SPS_MAX_SERIAL_LEN];
	if (sps30_get_serial(serial) != 0) {
		LOG_ERROR(F("SPS30 nicht gefunden (Verkabelung prüfen)"));
		_ok = false;
		return false;
	}
	sps30_set_fan_auto_cleaning_interval(AUTO_CLEANING_INTERVAL);
	delay(100);
	if (sps30_start_measurement() != 0) {
		LOG_ERROR(F("SPS30 Start der Messung fehlgeschlagen"));
		_ok = false;
		return false;
	}
	_ok = true;
	resetAccumulators();
	return true;
}

void SpsSensor::update(unsigned long)
{
	if (!_ok) return;
	struct sps30_measurement m;
	int16_t ret = sps30_read_measurement(&m);
	if (ret < 0 || SPS_IS_ERR_STATE(ret)) {
		return;
	}
	_p0 += m.mc_1p0; _p2 += m.mc_2p5; _p4 += m.mc_4p0; _p1 += m.mc_10p0;
	_n05 += m.nc_0p5; _n1 += m.nc_1p0; _n25 += m.nc_2p5; _n4 += m.nc_4p0; _n10 += m.nc_10p0;
	_tps += m.tps;
	++_count;
}

void SpsSensor::collect(std::vector<Reading> &out)
{
	if (_count == 0) {
		LOG_ERROR(F("SPS30: keine Messwerte im Zyklus"));
		return;
	}
	const float c = (float)_count;
	out.emplace_back(F("SPS30_P0"), _p0 / c, 2);
	out.emplace_back(F("SPS30_P2"), _p2 / c, 2);
	out.emplace_back(F("SPS30_P4"), _p4 / c, 2);
	out.emplace_back(F("SPS30_P1"), _p1 / c, 2);
	out.emplace_back(F("SPS30_N05"), _n05 / c, 2);
	out.emplace_back(F("SPS30_N1"), _n1 / c, 2);
	out.emplace_back(F("SPS30_N25"), _n25 / c, 2);
	out.emplace_back(F("SPS30_N4"), _n4 / c, 2);
	out.emplace_back(F("SPS30_N10"), _n10 / c, 2);
	out.emplace_back(F("SPS30_TS"), _tps / c, 2);
	resetAccumulators();
}
#endif // FEATURE_SENSOR_SPS30
