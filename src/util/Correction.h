#pragma once
// Small helper: convert correction/offset strings from the config (e.g. "-2.0")
// into a float. Taken 1:1 from utils.cpp of the original firmware
// (deliberately without atof, to save code size).

#include <Arduino.h>

float readCorrectionOffset(const char *correction);
