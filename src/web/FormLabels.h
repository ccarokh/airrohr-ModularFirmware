#pragma once
// Explicit, ordered layout table for the config form:
// each field -> tab + subgroup + label. The order here determines the
// presentation (independent of the config storage order).
//
// Labels are translated (Str id). Proper nouns - sensor models, protocol and
// service names - stay literal: `literal` wins over `label` when set, so
// translators never have to copy "SDS011" into 25 files.

#include <Arduino.h>

#include "../i18n/Lang.h"

namespace FormLabels {

struct Entry {
	const char *key;
	const char *literal; // untranslated label, or nullptr -> use `label`
	Str         label;
	Str         tab;
	Str         group;   // Str::COUNT = no subgroup heading
};

int          count();
const Entry &at(int i);

int tabCount();
Str tabId(int i);

// Label of an entry in the active language (literal or translation).
void appendLabel(String &out, const Entry &e);

} // namespace FormLabels
