#pragma once
// Board and platform abstraction.
// Pin assignments and platform-specific switches (ESP32 vs. ESP8266) will
// live here in the future. Currently focused on ESP32.

#include <Arduino.h>

// Firmware identifier
#define AIRROHR_VERSION "AR-modular-1.1.0"

#if defined(ESP32)
  #define SENSOR_BASENAME "esp32-"
#elif defined(ESP8266)
  #define SENSOR_BASENAME "esp8266-"
#else
  #error "Unsupported platform - ESP32 or ESP8266 required"
#endif

// --- Pin assignment: one file per board under src/boards/ ------------------
// Own board/PCB? -> create src/boards/pins_<name>.h and include it via a
//   -D BOARD_PINS=\"boards/pins_myboard.h\"
// build flag. Without an override, a default board is chosen per platform.
#ifndef DHT_TYPE
  #define DHT_TYPE 22   // DHT22 / AM2302
#endif

#if defined(BOARD_PINS)
  #include BOARD_PINS
#elif defined(ESP32)
  #include "boards/pins_esp32dev.h"
#elif defined(ESP8266)
  #include "boards/pins_nodemcu.h"
#endif

#ifndef OLED_ADDRESS
  #define OLED_ADDRESS 0x3c
#endif

// Serial interface for UART sensors (SDS/PMS/HPM/NPM/IPS/GPS):
// ESP32 = HardwareSerial (with pin remapping), ESP8266 = SoftwareSerial.
#if defined(ESP32)
  #include <HardwareSerial.h>
  using SensorSerial = HardwareSerial;
  #define SENSOR_SERIAL_8N1 SERIAL_8N1
#elif defined(ESP8266)
  #include <SoftwareSerial.h>
  using SensorSerial = SoftwareSerial;
  #define SENSOR_SERIAL_8N1 SWSERIAL_8N1
#endif

// Unique chip ID as a string (e.g. for topics / node names).
String boardChipId();

// Full node name: SENSOR_BASENAME + chip ID.
inline String boardNodeId() { return String(SENSOR_BASENAME) + boardChipId(); }
