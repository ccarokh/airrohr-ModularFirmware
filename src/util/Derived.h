#pragma once
// Derived measurements (computed from temperature/humidity/pressure).

#include <Arduino.h>

// Dew point [°C] from temperature [°C] and rel. humidity [%] (Magnus formula).
float dewPoint(float t, float h);

// Air pressure at sea level [Pa] from measured pressure [Pa], temperature [°C]
// and the configured height above sea level.
float seaLevelPressurePa(float t, float pressurePa);
