#pragma once
// Pin assignment: generic ESP32 DevKit (esp32dev). Standard ESP32 pins.
// All overridable via build_flags.

#ifndef PIN_I2C_SDA
  #define PIN_I2C_SDA 21
#endif
#ifndef PIN_I2C_SCL
  #define PIN_I2C_SCL 22
#endif
// PM serial as in the original firmware (ext_def.h, #ifdef ESP32) and on the
// airRohr-ESP32 carrier board: ESP-RX = GPIO27 (to SDS TXD), ESP-TX = GPIO33.
// Older wiring on 16/17 -> -D PIN_PM_SERIAL_RX=16 -D PIN_PM_SERIAL_TX=17.
#ifndef PIN_PM_SERIAL_RX
  #define PIN_PM_SERIAL_RX 27
#endif
#ifndef PIN_PM_SERIAL_TX
  #define PIN_PM_SERIAL_TX 33
#endif
#ifndef PIN_ONEWIRE
  #define PIN_ONEWIRE 23
#endif
#ifndef PIN_DHT
  #define PIN_DHT 23
#endif
#ifndef PIN_GPS_RX
  #define PIN_GPS_RX 4
#endif
#ifndef PIN_GPS_TX
  #define PIN_GPS_TX 2
#endif
#ifndef PPD_PIN_PM1
  #define PPD_PIN_PM1 18
#endif
#ifndef PPD_PIN_PM2
  #define PPD_PIN_PM2 19
#endif

// --- Weather sensors of the airRohr-ESP32 carrier board --------------------
// Analog inputs MUST be on ADC1 (GPIO32-39) - ADC2 is unusable while WiFi is
// active. Max. 3.3 V at the pin: run the rain module in 3.3 V mode, feed the
// anemometer through the R1/R2 divider.
#ifndef PIN_RAIN_AO
  #define PIN_RAIN_AO 35   // LM393 analog output (ADC1)
#endif
#ifndef PIN_RAIN_DO
  #define PIN_RAIN_DO 16   // LM393 digital threshold (potentiometer on the module)
#endif
#ifndef PIN_WIND_AO
  #define PIN_WIND_AO 34   // Anemometer via divider (ADC1)
#endif
