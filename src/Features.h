#pragma once
// Central compile-time switches. Setting a feature to 0 (here or via
// build_flags in platformio.ini, e.g. -D FEATURE_SENSOR_GPS=0) removes its
// code AND its library from the binary -> smaller flash.
//
// Requirement: platformio.ini has `lib_ldf_mode = chain+` (evaluates the #if).

// ---- Sensors ---------------------------------------------------------------
#ifndef FEATURE_SENSOR_SDS
  #define FEATURE_SENSOR_SDS 1
#endif
#ifndef FEATURE_SENSOR_PMS
  #define FEATURE_SENSOR_PMS 1
#endif
#ifndef FEATURE_SENSOR_HPM
  #define FEATURE_SENSOR_HPM 1
#endif
#ifndef FEATURE_SENSOR_NPM
  #define FEATURE_SENSOR_NPM 1
#endif
#ifndef FEATURE_SENSOR_IPS
  #define FEATURE_SENSOR_IPS 1
#endif
#ifndef FEATURE_SENSOR_PPD
  #define FEATURE_SENSOR_PPD 1
#endif
#ifndef FEATURE_SENSOR_SPS30
  #define FEATURE_SENSOR_SPS30 1
#endif
#ifndef FEATURE_SENSOR_DHT
  #define FEATURE_SENSOR_DHT 1
#endif
#ifndef FEATURE_SENSOR_HTU21D
  #define FEATURE_SENSOR_HTU21D 1
#endif
#ifndef FEATURE_SENSOR_BMP
  #define FEATURE_SENSOR_BMP 1
#endif
#ifndef FEATURE_SENSOR_BMX280
  #define FEATURE_SENSOR_BMX280 1
#endif
#ifndef FEATURE_SENSOR_SHT3X
  #define FEATURE_SENSOR_SHT3X 1
#endif
#ifndef FEATURE_SENSOR_SCD30
  #define FEATURE_SENSOR_SCD30 1
#endif
#ifndef FEATURE_SENSOR_DS18B20
  #define FEATURE_SENSOR_DS18B20 1
#endif
#ifndef FEATURE_SENSOR_DNMS
  #define FEATURE_SENSOR_DNMS 1
#endif
#ifndef FEATURE_SENSOR_GPS
  #define FEATURE_SENSOR_GPS 1
#endif

// ---- Sender ----------------------------------------------------------------
#ifndef FEATURE_SENDER_SENSORCOMMUNITY
  #define FEATURE_SENDER_SENSORCOMMUNITY 1
#endif
#ifndef FEATURE_SENDER_MADAVI
  #define FEATURE_SENDER_MADAVI 1
#endif
#ifndef FEATURE_SENDER_SENSEMAP
  #define FEATURE_SENDER_SENSEMAP 1
#endif
#ifndef FEATURE_SENDER_FSAPP
  #define FEATURE_SENDER_FSAPP 1
#endif
#ifndef FEATURE_SENDER_AIRCMS
  #define FEATURE_SENDER_AIRCMS 1
#endif
#ifndef FEATURE_SENDER_INFLUX
  #define FEATURE_SENDER_INFLUX 1
#endif
#ifndef FEATURE_SENDER_CUSTOM
  #define FEATURE_SENDER_CUSTOM 1
#endif
#ifndef FEATURE_SENDER_CSV
  #define FEATURE_SENDER_CSV 1
#endif
#ifndef FEATURE_SENDER_MQTT
  #define FEATURE_SENDER_MQTT 1
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
