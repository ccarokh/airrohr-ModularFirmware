#include "Debug.h"

namespace {
LogLevel g_level = LogLevel::Info;

const __FlashStringHelper *prefix(LogLevel level)
{
	switch (level) {
	case LogLevel::Error:   return F("[E] ");
	case LogLevel::Warning: return F("[W] ");
	case LogLevel::Info:    return F("[I] ");
	case LogLevel::Debug:   return F("[D] ");
	case LogLevel::Verbose: return F("[V] ");
	default:                return F("");
	}
}
} // namespace

namespace Log {

void begin(unsigned long baud) { Serial.begin(baud); }

void setLevel(LogLevel level) { g_level = level; }
LogLevel level() { return g_level; }

void line(LogLevel level, const __FlashStringHelper *msg)
{
	if ((uint8_t)level > (uint8_t)g_level) return;
	Serial.print(prefix(level));
	Serial.println(msg);
}

void line(LogLevel level, const __FlashStringHelper *msg, const String &value)
{
	if ((uint8_t)level > (uint8_t)g_level) return;
	Serial.print(prefix(level));
	Serial.print(msg);
	Serial.println(value);
}

void line(LogLevel level, const String &msg)
{
	if ((uint8_t)level > (uint8_t)g_level) return;
	Serial.print(prefix(level));
	Serial.println(msg);
}

} // namespace Log
