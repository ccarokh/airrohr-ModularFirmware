#pragma once
// Pin assignment: ESP8266 NodeMCU – standard airrohr assignment (D labels of
// the NodeMCU board), matches existing airrohr hardware. Overridable.
//
//   D1=GPIO5  D2=GPIO4  D3=GPIO0  D4=GPIO2  D5=GPIO14  D6=GPIO12  D7=GPIO13

#ifndef PIN_I2C_SDA
  #define PIN_I2C_SDA D3   // GPIO0
#endif
#ifndef PIN_I2C_SCL
  #define PIN_I2C_SCL D4   // GPIO2
#endif
#ifndef PIN_PM_SERIAL_RX
  #define PIN_PM_SERIAL_RX D1  // GPIO5
#endif
#ifndef PIN_PM_SERIAL_TX
  #define PIN_PM_SERIAL_TX D2  // GPIO4
#endif
#ifndef PIN_ONEWIRE
  #define PIN_ONEWIRE D7   // GPIO13
#endif
#ifndef PIN_DHT
  #define PIN_DHT D7
#endif
#ifndef PIN_GPS_RX
  #define PIN_GPS_RX D5    // GPIO14
#endif
#ifndef PIN_GPS_TX
  #define PIN_GPS_TX D6    // GPIO12
#endif
#ifndef PPD_PIN_PM1
  #define PPD_PIN_PM1 D5
#endif
#ifndef PPD_PIN_PM2
  #define PPD_PIN_PM2 D6
#endif
