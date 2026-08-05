#include "../Features.h"
#if FEATURE_SENSOR_SHT3X
#include "Sht3xSensor.h"
#include "../Debug.h"

bool Sht3xSensor::begin()
{
	_ok = _sht.begin(0x44);
	return _ok;
}

void Sht3xSensor::collect(std::vector<Reading> &out)
{
	const float t = _sht.readTemperature();
	const float h = _sht.readHumidity();
	if (isnan(t) || isnan(h)) {
		LOG_ERROR(F("SHT3X: Lesefehler"));
		return;
	}
	out.emplace_back(F("SHT3X_temperature"), t, 2);
	out.emplace_back(F("SHT3X_humidity"), h, 2);
}
#endif // FEATURE_SENSOR_SHT3X
