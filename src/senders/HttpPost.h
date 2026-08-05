#pragma once
// Small HTTP(S) POST helper, shared by the HTTP senders.

#include <Arduino.h>

// Sends body via POST to host:port + path. ssl=true uses HTTPS (setInsecure).
// user/pass optional (Basic-Auth). xpin>0 sets the X-Pin header (sensor.community).
// true on HTTP 2xx.
bool httpPost(const char *host, uint16_t port, const char *path, bool ssl,
			  const char *contentType, const String &body,
			  const char *user = nullptr, const char *pass = nullptr,
			  int xpin = 0);
