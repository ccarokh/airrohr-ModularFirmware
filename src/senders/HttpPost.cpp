#include "HttpPost.h"
#include "../Features.h"
#include "../Platform.h"
#include "../Board.h"
#include "../Debug.h"

#include <memory>

bool httpPost(const char *host, uint16_t port, const char *path, bool ssl,
			  const char *contentType, const String &body,
			  const char *user, const char *pass, int xpin)
{
	std::unique_ptr<WiFiClient> client;
	bool useSsl = ssl;
	uint16_t usePort = port;
#if FEATURE_TLS
	if (ssl) {
		auto *sc = new WiFiClientSecure;
		sc->setInsecure();
#if defined(ESP8266)
		// ESP8266 has little RAM: small TLS buffers (MFLN) instead of the 16 KB
		// default, otherwise the handshake stalls. As in the original firmware.
		sc->setBufferSizes(1024, 1024);
#endif
		client.reset(sc);
	} else {
		client.reset(new WiFiClient);
	}
#else
	// TLS disabled -> plaintext HTTP; fall back from 443 to 80.
	useSsl = false;
	if (usePort == 443) usePort = 80;
	client.reset(new WiFiClient);
#endif

	HTTPClient http;
	http.setTimeout(6 * 1000); // keep it short: blocking sends must not stall loop() for long
#if defined(ESP32)
	http.setConnectTimeout(4 * 1000); // ESP32 only; ESP8266 uses setTimeout
#endif
	http.setReuse(false);

	if (!http.begin(*client, String(host), usePort, String(path), useSsl)) {
		LOG_ERROR(F("HTTP: begin fehlgeschlagen für "), String(host));
		return false;
	}

	http.addHeader(F("Content-Type"), contentType);
	http.addHeader(F("X-Sensor"), boardNodeId());
	if (xpin > 0) {
		http.addHeader(F("X-Pin"), String(xpin));
	}
	if (user && *user) {
		http.setAuthorization(user, pass ? pass : "");
	}

	const int code = http.POST(body);
	const bool ok = (code >= 200 && code < 300);
	if (!ok) {
		LOG_WARN(F("HTTP: Fehlercode "), String(code) + F(" von ") + host);
	}
	http.end();
	return ok;
}
