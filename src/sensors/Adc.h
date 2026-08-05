#pragma once
// Shared ADC constants for the analog sensors (rain, wind).
//
// ESP32: 12 bit, ~3.3 V full scale with 11 dB attenuation - analog inputs only
//        on ADC1 (GPIO32-39), ADC2 is blocked while WiFi is running.
// ESP8266: 10 bit; on the NodeMCU the on-board divider puts full scale at the
//        A0 header at ~3.3 V (the bare chip pin measures 0-1 V).

#include <Arduino.h>

#if defined(ESP32)
constexpr int   ADC_MAX_COUNTS  = 4095;
constexpr float ADC_FULLSCALE_V = 3.3f;
#else
constexpr int   ADC_MAX_COUNTS  = 1023;
constexpr float ADC_FULLSCALE_V = 3.3f;
#endif

// Averaged raw value - a single analogRead() is noisy on both platforms.
inline int adcReadAveraged(uint8_t pin, uint8_t samples = 16)
{
	long sum = 0;
	for (uint8_t i = 0; i < samples; ++i) {
		sum += analogRead(pin);
		delayMicroseconds(200);
	}
	return (int)(sum / samples);
}

inline float adcToVolts(int raw) { return (float)raw * ADC_FULLSCALE_V / (float)ADC_MAX_COUNTS; }
