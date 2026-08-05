#include "../Features.h"
#if FEATURE_SENSOR_PPD
#include "PpdSensor.h"
#include "../Debug.h"

namespace {
constexpr unsigned long SAMPLETIME_MS = 30000; // measurement window of the PPD42NS

// Concentration from the LPO ratio (characteristic curve from the datasheet).
inline float calcConcentration(float ratio)
{
	return (1.1f * ratio * ratio * ratio - 3.8f * ratio * ratio + 520.0f * ratio + 0.62f);
}
}

bool PpdSensor::begin()
{
	pinMode(_pinP1, INPUT);
	pinMode(_pinP2, INPUT);
	_lpoP1 = _lpoP2 = 0;
	return true;
}

void PpdSensor::update(unsigned long ms_since_start)
{
	if (ms_since_start > SAMPLETIME_MS) {
		return; // only measure in the first sample window of the cycle
	}
	const unsigned long now = micros();

	const bool valP1 = digitalRead(_pinP1);
	const bool valP2 = digitalRead(_pinP2);

	if (valP1 == LOW && !_trigP1) { _trigP1 = true; _trigOnP1 = now; }
	if (valP1 == HIGH && _trigP1) { _lpoP1 += now - _trigOnP1; _trigP1 = false; }

	if (valP2 == LOW && !_trigP2) { _trigP2 = true; _trigOnP2 = now; }
	if (valP2 == HIGH && _trigP2) { _lpoP2 += now - _trigOnP2; _trigP2 = false; }
}

void PpdSensor::collect(std::vector<Reading> &out)
{
	float ratioP1 = _lpoP1 / (SAMPLETIME_MS * 10.0f);
	float concP1 = calcConcentration(ratioP1);
	out.emplace_back(F("durP1"), (float)_lpoP1, 0);
	out.emplace_back(F("ratioP1"), ratioP1, 2);
	out.emplace_back(F("P1"), concP1, 2);

	float ratioP2 = _lpoP2 / (SAMPLETIME_MS * 10.0f);
	float concP2 = calcConcentration(ratioP2);
	out.emplace_back(F("durP2"), (float)_lpoP2, 0);
	out.emplace_back(F("ratioP2"), ratioP2, 2);
	out.emplace_back(F("P2"), concP2, 2);

	_lpoP1 = _lpoP2 = 0;
}
#endif // FEATURE_SENSOR_PPD
