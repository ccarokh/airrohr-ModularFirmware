#pragma once
// Central compile-time switches - the single place where the feature set of a
// build is decided.
//
//   * Changing what a build contains: edit the values below.
//   * Build variants (minimal build, ESP8266, ...): add a PROFILE here.
//     platformio.ini only names the profile, it does not list features.
//   * -D FEATURE_X=0 on the command line still wins over everything in this
//     file - but that is meant for quick experiments, not for maintaining
//     variants. Anything permanent belongs here.
//
// Setting a feature to 0 removes its code AND its library from the binary
// -> smaller flash. Requires `lib_ldf_mode = chain+` in platformio.ini, which
// evaluates the #if guards.

// ---- Profiles --------------------------------------------------------------
// A profile pre-sets a few switches; everything it does not mention falls back
// to the defaults further down. Selected with a single flag in platformio.ini,
// e.g. `build_flags = -D PROFILE_MINIMAL`.

#if defined(PROFILE_MINIMAL)
  // Size demo: SDS011 + BMP280/BME280, sensor.community + MQTT, no display.
  #define FEATURE_SENSORS_DEFAULT 0
  #define FEATURE_SENDERS_DEFAULT 0
  #define FEATURE_SENSOR_SDS      1
  #define FEATURE_SENSOR_BMX280   1
  #define FEATURE_SENDER_SENSORCOMMUNITY 1
  #define FEATURE_SENDER_MQTT     1
  #define FEATURE_DISPLAY         0
  #define FEATURE_LANG_EN         0

#elif defined(PROFILE_CARRIER)
  // airRohr-ESP32 carrier board: full set plus the two weather sensors.
  #define FEATURE_SENSOR_RAIN 1
  #define FEATURE_SENSOR_WIND 1

#elif defined(PROFILE_ESP8266)
  // Reduced set for the ESP8266: SDS011 + MQTT/sensor.community/Madavi,
  // no display (RAM), climate sensors off.
  #define FEATURE_SENSORS_DEFAULT 0
  #define FEATURE_SENSOR_SDS      1
  #define FEATURE_SENSOR_BMX280   1
  #define FEATURE_SENDER_SENSEMAP 0
  #define FEATURE_SENDER_FSAPP    0
  #define FEATURE_SENDER_AIRCMS   0
  #define FEATURE_SENDER_INFLUX   0
  #define FEATURE_SENDER_CUSTOM   0
  #define FEATURE_SENDER_CSV      0
  #define FEATURE_DISPLAY         0
#endif

// Group defaults - a profile flips these in one line instead of listing every
// single sensor/sender.
#ifndef FEATURE_SENSORS_DEFAULT
  #define FEATURE_SENSORS_DEFAULT 1
#endif
#ifndef FEATURE_SENDERS_DEFAULT
  #define FEATURE_SENDERS_DEFAULT 1
#endif

// ---- Sensors ---------------------------------------------------------------
#ifndef FEATURE_SENSOR_SDS
  #define FEATURE_SENSOR_SDS FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_PMS
  #define FEATURE_SENSOR_PMS FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_HPM
  #define FEATURE_SENSOR_HPM FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_NPM
  #define FEATURE_SENSOR_NPM FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_IPS
  #define FEATURE_SENSOR_IPS FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_PPD
  #define FEATURE_SENSOR_PPD FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_SPS30
  #define FEATURE_SENSOR_SPS30 FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_DHT
  #define FEATURE_SENSOR_DHT FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_HTU21D
  #define FEATURE_SENSOR_HTU21D FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_BMP
  #define FEATURE_SENSOR_BMP FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_BMX280
  #define FEATURE_SENSOR_BMX280 FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_SHT3X
  #define FEATURE_SENSOR_SHT3X FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_SCD30
  #define FEATURE_SENSOR_SCD30 FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_DS18B20
  #define FEATURE_SENSOR_DS18B20 FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_DNMS
  #define FEATURE_SENSOR_DNMS FEATURE_SENSORS_DEFAULT
#endif
#ifndef FEATURE_SENSOR_GPS
  #define FEATURE_SENSOR_GPS FEATURE_SENSORS_DEFAULT
#endif

// Weather sensors of the airRohr-ESP32 carrier board (analog, ADC1).
// Deliberately NOT part of FEATURE_SENSORS_DEFAULT and off even in the full
// build: they are not sensor.community reference hardware and occupy two
// analog pins that other boards use differently. Whoever has the carrier
// board switches them on here.
#ifndef FEATURE_SENSOR_RAIN
  #define FEATURE_SENSOR_RAIN 0
#endif
#ifndef FEATURE_SENSOR_WIND
  #define FEATURE_SENSOR_WIND 0
#endif

// ---- Sender ----------------------------------------------------------------
#ifndef FEATURE_SENDER_SENSORCOMMUNITY
  #define FEATURE_SENDER_SENSORCOMMUNITY FEATURE_SENDERS_DEFAULT
#endif
#ifndef FEATURE_SENDER_MADAVI
  #define FEATURE_SENDER_MADAVI FEATURE_SENDERS_DEFAULT
#endif
#ifndef FEATURE_SENDER_SENSEMAP
  #define FEATURE_SENDER_SENSEMAP FEATURE_SENDERS_DEFAULT
#endif
#ifndef FEATURE_SENDER_FSAPP
  #define FEATURE_SENDER_FSAPP FEATURE_SENDERS_DEFAULT
#endif
#ifndef FEATURE_SENDER_AIRCMS
  #define FEATURE_SENDER_AIRCMS FEATURE_SENDERS_DEFAULT
#endif
#ifndef FEATURE_SENDER_INFLUX
  #define FEATURE_SENDER_INFLUX FEATURE_SENDERS_DEFAULT
#endif
#ifndef FEATURE_SENDER_CUSTOM
  #define FEATURE_SENDER_CUSTOM FEATURE_SENDERS_DEFAULT
#endif
#ifndef FEATURE_SENDER_CSV
  #define FEATURE_SENDER_CSV FEATURE_SENDERS_DEFAULT
#endif
#ifndef FEATURE_SENDER_MQTT
  #define FEATURE_SENDER_MQTT FEATURE_SENDERS_DEFAULT
#endif

// ---- Languages -------------------------------------------------------------
// Every language compiled in costs roughly 3-4 kB of flash; switching between
// the compiled-in ones happens at runtime (config field current_lang). The
// FIRST language enabled here is the fallback when the stored code is unknown.
// New language -> src/i18n/strings_template.h describes the three steps.
#ifndef FEATURE_LANG_DE
  #define FEATURE_LANG_DE 1
#endif
#ifndef FEATURE_LANG_EN
  #define FEATURE_LANG_EN 1
#endif

// ---- Miscellaneous ---------------------------------------------------------
#ifndef FEATURE_DISPLAY
  #define FEATURE_DISPLAY 1
#endif

// TLS/HTTPS (WiFiClientSecure). Set to 0 -> no TLS for MQTT (ssl_mqtt) AND no
// HTTPS senders; in return the entire TLS stack (mbedTLS/BearSSL) drops out of
// the binary -> significantly smaller. Useful for purely local brokers/HTTP.
#ifndef FEATURE_TLS
  #define FEATURE_TLS 1
#endif
