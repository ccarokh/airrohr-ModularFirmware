#pragma once
// Explicit, ordered layout table for the config form:
// each field -> tab + subgroup + label. The order here determines the
// presentation (independent of the config storage order).

#include <Arduino.h>

namespace FormLabels {

struct Entry {
	const char *key;
	const char *label;
	const char *tab;
	const char *group; // Subgroup heading ("" = none)
};

int          count();
const Entry &at(int i);

int          tabCount();
const char  *tabName(int i);

} // namespace FormLabels
