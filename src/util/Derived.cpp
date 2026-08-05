#include "Derived.h"
#include "Correction.h"
#include "../Config.h"

#include <math.h>

float dewPoint(float t, float h)
{
	const float k2 = 17.62f, k3 = 243.12f;
	const float x = ((k2 * t) / (k3 + t)) + logf(h / 100.0f);
	return (k3 * x) / (k2 - x);
}

float seaLevelPressurePa(float t, float pressurePa)
{
	const float height = readCorrectionOffset(cfg.height_above_sealevel);
	return pressurePa * powf((t + 273.15f) / (t + 273.15f + (0.0065f * height)), -5.255f);
}
