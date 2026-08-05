#include "../Features.h"
#if FEATURE_SENSOR_DHT
#include "DhtSensor.h"
#include "../Debug.h"
#include "../Config.h"
#include "../util/Correction.h"

bool DhtSensor::begin()
{
	_dht.begin();
	_ok = true;
	return _ok;
}

void DhtSensor::collect(std::vector<Reading> &out)
{
	LOG_VERBOSE(F("DHT: lese..."));

	// Up to 5 attempts (as in the original version).
	const int MAX_ATTEMPTS = 5;
	for (int i = 0; i < MAX_ATTEMPTS; ++i) {
		float t = _dht.readTemperature();
		float h = _dht.readHumidity();
		if (isnan(t) || isnan(h)) {
			delay(100);
			t = _dht.readTemperature(true);
			h = _dht.readHumidity();
		}
		if (isnan(t) || isnan(h)) {
			LOG_ERROR(F("DHT: Lesefehler"));
			continue;
		}
		t += readCorrectionOffset(cfg.temp_correction);
		out.emplace_back(F("temperature"), t, 1);
		out.emplace_back(F("humidity"), h, 1);
		return;
	}
}
#endif // FEATURE_SENSOR_DHT
