#pragma once
// Pin assignment: generic ESP32 DevKit (esp32dev). Standard ESP32 pins.
// All overridable via build_flags.

#ifndef PIN_I2C_SDA
  #define PIN_I2C_SDA 21
#endif
#ifndef PIN_I2C_SCL
  #define PIN_I2C_SCL 22
#endif
#ifndef PIN_PM_SERIAL_RX
  #define PIN_PM_SERIAL_RX 16
#endif
#ifndef PIN_PM_SERIAL_TX
  #define PIN_PM_SERIAL_TX 17
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
