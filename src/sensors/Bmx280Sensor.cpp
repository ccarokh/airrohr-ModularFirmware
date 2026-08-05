#include "../Features.h"
#if FEATURE_SENSOR_BMX280
#include "Bmx280Sensor.h"
#include "../Debug.h"
#include "../Config.h"
#include "../util/Correction.h"

bool Bmx280Sensor::tryInit(uint8_t addr)
{
	if (!_bmx.begin(addr)) {
		return false;
	}
	_bmx.setSampling(
		BMX280::MODE_FORCED,
		BMX280::SAMPLING_X1,
		BMX280::SAMPLING_X1,
		BMX280::SAMPLING_X1);
	return true;
}

bool Bmx280Sensor::begin()
{
	// Try the standard I2C addresses 0x76 and 0x77.
	_ok = tryInit(0x76) || tryInit(0x77);
	if (_ok) {
		_isBme = (_bmx.sensorID() == BME280_SENSOR_ID);
	}
	return _ok;
}

void Bmx280Sensor::collect(std::vector<Reading> &out)
{
	if (!_ok) {
		return;
	}
	_bmx.takeForcedMeasurement();
	const float t = _bmx.readTemperature();
	const float p = _bmx.readPressure();
	const float h = _bmx.readHumidity();
	if (isnan(t) || isnan(p)) {
		LOG_ERROR(F("BMP/BME280: Lesefehler"));
		return;
	}
	const float tc = t + readCorrectionOffset(cfg.temp_correction);
	if (_bmx.sensorID() == BME280_SENSOR_ID) {
		out.emplace_back(F("BME280_temperature"), tc, 2);
		out.emplace_back(F("BME280_pressure"), p, 2);
		out.emplace_back(F("BME280_humidity"), h, 2);
	} else {
		out.emplace_back(F("BMP280_pressure"), p, 2);
		out.emplace_back(F("BMP280_temperature"), tc, 2);
	}
}
#endif // FEATURE_SENSOR_BMX280
