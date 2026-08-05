#include "../Features.h"
#if FEATURE_SENSOR_SCD30
#include "Scd30Sensor.h"
#include "../Debug.h"

bool Scd30Sensor::begin()
{
	_ok = _scd.begin();
	return _ok;
}

void Scd30Sensor::collect(std::vector<Reading> &out)
{
	const float t = _scd.getTemperature();
	const float h = _scd.getHumidity();
	const float c = _scd.getCO2();
	if (isnan(t) || isnan(h) || isnan(c)) {
		LOG_ERROR(F("SCD30: Lesefehler"));
		return;
	}
	out.emplace_back(F("SCD30_temperature"), t, 2);
	out.emplace_back(F("SCD30_humidity"), h, 2);
	out.emplace_back(F("SCD30_co2_ppm"), c, 0);
}
#endif // FEATURE_SENSOR_SCD30
