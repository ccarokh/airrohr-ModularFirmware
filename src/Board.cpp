#include "Board.h"

#if defined(ESP32)
#include <WiFi.h>
#endif

String boardChipId()
{
#if defined(ESP32)
	// Derive a 24-bit ID from the MAC (analogous to the ESP8266 chip ID).
	uint64_t mac = ESP.getEfuseMac();
	uint32_t id = (uint32_t)(mac >> 24); // upper bytes of the MAC
	char buf[9];
	snprintf(buf, sizeof(buf), "%06X", id & 0xFFFFFF);
	return String(buf);
#elif defined(ESP8266)
	// Decimal chip ID like original airrohr / sensor.community (e.g. 1908787).
	return String(ESP.getChipId());
#endif
}
