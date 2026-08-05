#include "../Features.h"
#if FEATURE_SENSOR_BMP
#include "BmpSensor.h"
#include "../Debug.h"

bool BmpSensor::begin()
{
	_ok = _bmp.begin();
	return _ok;
}

void BmpSensor::collect(std::vector<Reading> &out)
{
	const float p = _bmp.readPressure();
	const float t = _bmp.readTemperature();
	if (isnan(p) || isnan(t)) {
		LOG_ERROR(F("BMP180: Lesefehler"));
		return;
	}
	out.emplace_back(F("BMP_pressure"), p, 2);
	out.emplace_back(F("BMP_temperature"), t, 2);
}
#endif // FEATURE_SENSOR_BMP
