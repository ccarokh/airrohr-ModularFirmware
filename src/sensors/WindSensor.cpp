#include "../Features.h"
#if FEATURE_SENSOR_WIND
#include "WindSensor.h"
#include "Adc.h"
#include "../Config.h"
#include "../Debug.h"

#include <stdlib.h>

namespace {
constexpr unsigned long SAMPLE_INTERVAL_MS = 250;
}

bool WindSensor::begin()
{
#if defined(ESP32)
	analogSetPinAttenuation(_pinAo, ADC_11db);
#endif
	_last_sample = millis();
	const int raw = adcReadAveraged(_pinAo, 4);
	LOG_INFO(F("WIND: Rohwert beim Start: "), String(raw));
	return true;
}

float WindSensor::speedFromRaw(int raw) const
{
	// Divider back-calculation: U_sensor = U_ADC * divider (10k/20k -> 1.5).
	const float divider = atof(cfg.wind_divider);
	const float factor  = atof(cfg.wind_factor);   // m/s per volt at the sensor
	const float offset  = atof(cfg.wind_offset);   // dead voltage at standstill
	const float volts   = adcToVolts(raw) * (divider > 0.0f ? divider : 1.0f);
	const float speed   = (volts - offset) * factor;
	return speed > 0.0f ? speed : 0.0f;
}

void WindSensor::update(unsigned long /*ms_since_start*/)
{
	const unsigned long now = millis();
	if (now - _last_sample < SAMPLE_INTERVAL_MS) {
		return;
	}
	_last_sample = now;

	const int raw = adcReadAveraged(_pinAo, 4); // short burst, loop() must stay responsive
	const float speed = speedFromRaw(raw);
	_sum_speed += speed;
	_sum_volts += adcToVolts(raw) * (atof(cfg.wind_divider) > 0.0f ? atof(cfg.wind_divider) : 1.0f);
	++_samples;
	if (speed > _gust) {
		_gust = speed;
	}
}

void WindSensor::collect(std::vector<Reading> &out)
{
	float mean = 0.0f, volts = 0.0f, gust = _gust;
	if (_samples > 0) {
		mean  = (float)(_sum_speed / (double)_samples);
		volts = (float)(_sum_volts / (double)_samples);
	} else {
		// Cycle without a single sample (e.g. right after begin()) -> read once.
		const int raw = adcReadAveraged(_pinAo);
		mean  = speedFromRaw(raw);
		volts = adcToVolts(raw) * (atof(cfg.wind_divider) > 0.0f ? atof(cfg.wind_divider) : 1.0f);
		gust  = mean;
	}

	out.emplace_back(F("wind_speed"), mean, 1);
	out.emplace_back(F("wind_gust"), gust, 1);
	out.emplace_back(F("wind_voltage"), volts, 2);

	_sum_speed = 0.0;
	_sum_volts = 0.0;
	_samples   = 0;
	_gust      = 0.0f;
}
#endif // FEATURE_SENSOR_WIND
