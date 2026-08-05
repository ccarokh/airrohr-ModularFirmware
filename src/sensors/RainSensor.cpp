#include "../Features.h"
#if FEATURE_SENSOR_RAIN
#include "RainSensor.h"
#include "Adc.h"
#include "../Config.h"
#include "../Debug.h"

#include <stdlib.h>

bool RainSensor::begin()
{
	pinMode(_pinDo, INPUT);
#if defined(ESP32)
	// Full 0-3.3 V range on the analog pin (the module runs in 3.3 V mode).
	analogSetPinAttenuation(_pinAo, ADC_11db);
#endif
	// No handshake possible - an analog input is always "there". A raw value of
	// exactly 0 usually means the module is not connected/not powered.
	const int raw = adcReadAveraged(_pinAo, 4);
	LOG_INFO(F("RAIN: Rohwert beim Start: "), String(raw));
	return true;
}

void RainSensor::collect(std::vector<Reading> &out)
{
	const int raw = adcReadAveraged(_pinAo);

	// Two calibration points from the config: dry (high raw value) and
	// wet (low raw value) -> 0-100 % wetness, clamped.
	const long dry = strtol(cfg.rain_dry_adc, nullptr, 10);
	const long wet = strtol(cfg.rain_wet_adc, nullptr, 10);
	float moisture = 0.0f;
	if (dry != wet) {
		moisture = 100.0f * (float)(dry - raw) / (float)(dry - wet);
		if (moisture < 0.0f)   moisture = 0.0f;
		if (moisture > 100.0f) moisture = 100.0f;
	}

	// The comparator pulls DO low when it is wet.
	const bool wetDigital = (digitalRead(_pinDo) == LOW);

	out.emplace_back(F("rain_moisture"), moisture, 1);
	out.emplace_back(String(F("rain_state")), String(wetDigital ? 1 : 0));
	out.emplace_back(String(F("rain_adc")), String(raw));
}
#endif // FEATURE_SENSOR_RAIN
