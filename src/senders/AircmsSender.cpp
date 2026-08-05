#include "../Features.h"
#if FEATURE_SENDER_AIRCMS
#include "AircmsSender.h"
#include "HttpPost.h"
#include "../Board.h"
#include "../Config.h"
#include "../Platform.h"

#if defined(ESP32)
  #include <mbedtls/sha1.h>
#elif defined(ESP8266)
  #include <bearssl/bearssl_hash.h>
#endif

namespace {
String sha1Hex(const String &s)
{
	unsigned char out[20];
#if defined(ESP32)
	mbedtls_sha1((const unsigned char *)s.c_str(), s.length(), out);
#elif defined(ESP8266)
	br_sha1_context ctx;
	br_sha1_init(&ctx);
	br_sha1_update(&ctx, s.c_str(), s.length());
	br_sha1_out(&ctx, out);
#endif
	char hex[41];
	for (int i = 0; i < 20; ++i) {
		sprintf(hex + i * 2, "%02x", out[i]);
	}
	return String(hex);
}

// hmac1 as in the original firmware (utils.cpp): sha1( secret + sha1(s) ).
String hmac1(const String &secret, const String &s)
{
	return sha1Hex(secret + sha1Hex(s));
}
}

bool AircmsSender::enabled() const { return cfg.send2aircms; }

bool AircmsSender::send(const std::vector<SensorReadings> & /*groups*/, const String &json)
{
	const unsigned long ts = millis() / 1000;
	const String token = WiFi.macAddress();

	String body("L=");
	body += boardChipId();
	body += "&t=";
	body += String(ts, DEC);
	body += F("&airrohr=");
	body += json;

	String path = F("/php/sensors.php?h=");
	path += hmac1(sha1Hex(token), body + token);

	return httpPost("doiot.ru", 80, path.c_str(), false, "text/plain", body);
}
#endif // FEATURE_SENDER_AIRCMS
