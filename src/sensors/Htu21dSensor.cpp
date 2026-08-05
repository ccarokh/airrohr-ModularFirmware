#include "../Features.h"
#if FEATURE_SENSOR_HTU21D
#include "Htu21dSensor.h"
#include "../Debug.h"

bool Htu21dSensor::begin()
{
	_ok = _htu.begin();
	return _ok;
}

void Htu21dSensor::collect(std::vector<Reading> &out)
{
	const float t = _htu.readTemperature();
	const float h = _htu.readHumidity();
	if (isnan(t) || isnan(h)) {
		LOG_ERROR(F("HTU21D: Lesefehler"));
		return;
	}
	out.emplace_back(F("HTU21D_temperature"), t, 1);
	out.emplace_back(F("HTU21D_humidity"), h, 1);
}
#endif // FEATURE_SENSOR_HTU21D
