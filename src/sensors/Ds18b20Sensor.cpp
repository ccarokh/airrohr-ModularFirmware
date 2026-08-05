#include "../Features.h"
#if FEATURE_SENSOR_DS18B20
#include "Ds18b20Sensor.h"
#include "../Debug.h"
#include "../Config.h"
#include "../util/Correction.h"

bool Ds18b20Sensor::begin()
{
	_ds.begin();
	_ok = true;
	return _ok;
}

void Ds18b20Sensor::collect(std::vector<Reading> &out)
{
	// -127/85 °C are typical error values; up to 5 attempts (as in the original).
	const int MAX_ATTEMPTS = 5;
	int count = 0;
	float t;
	do {
		_ds.requestTemperatures();
		t = _ds.getTempCByIndex(0);
		++count;
	} while (count < MAX_ATTEMPTS && (isnan(t) || t >= 85.0f || t <= -127.0f));

	if (count == MAX_ATTEMPTS) {
		LOG_ERROR(F("DS18B20: Lesefehler"));
		return;
	}
	t += readCorrectionOffset(cfg.temp_correction);
	out.emplace_back(F("DS18B20_temperature"), t, 2);
}
#endif // FEATURE_SENSOR_DS18B20
