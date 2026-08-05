#pragma once
// Platform abstraction for network/web server. Keeps ESP32/ESP8266 differences
// in one place so the remaining modules stay platform-neutral
// (ESP32 = primary target, ESP8266 remains retrofittable).

#include "Features.h"

#if defined(ESP32)
  #include <WiFi.h>
  #include <HTTPClient.h>
  #include <WebServer.h>
  #if FEATURE_TLS
    #include <WiFiClientSecure.h>
  #endif
  using WebServerClass = WebServer;
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <ESP8266WebServer.h>
  #if FEATURE_TLS
    #include <WiFiClientSecure.h>
  #endif
  using WebServerClass = ESP8266WebServer;
#else
  #error "Unsupported platform"
#endif
