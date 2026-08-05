#pragma once
// Central logging. Replaces the scattered debug_outln_* macros of the old .ino
// with a small, testable API with log levels.

#include <Arduino.h>

enum class LogLevel : uint8_t {
	None = 0,
	Error = 1,
	Warning = 2,
	Info = 3,
	Debug = 4,
	Verbose = 5,
};

namespace Log {

// Initializes the serial output.
void begin(unsigned long baud = 115200);

// Set/read the current log level (fed from the config).
void setLevel(LogLevel level);
LogLevel level();

// Print a line with an optional appended value.
void line(LogLevel level, const __FlashStringHelper *msg);
void line(LogLevel level, const __FlashStringHelper *msg, const String &value);
void line(LogLevel level, const String &msg);

} // namespace Log

// Convenient shorthands
#define LOG_ERROR(...)   Log::line(LogLevel::Error, __VA_ARGS__)
#define LOG_WARN(...)    Log::line(LogLevel::Warning, __VA_ARGS__)
#define LOG_INFO(...)    Log::line(LogLevel::Info, __VA_ARGS__)
#define LOG_DEBUG(...)   Log::line(LogLevel::Debug, __VA_ARGS__)
#define LOG_VERBOSE(...) Log::line(LogLevel::Verbose, __VA_ARGS__)
