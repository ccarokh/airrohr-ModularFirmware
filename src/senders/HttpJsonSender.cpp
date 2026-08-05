#include "../Features.h"
#if FEATURE_SENDER_CUSTOM
#include "HttpJsonSender.h"
#include "HttpPost.h"
#include "../Board.h"
#include "../Config.h"

bool HttpJsonSender::enabled() const { return cfg.send2custom; }

bool HttpJsonSender::send(const std::vector<SensorReadings> & /*groups*/, const String &json)
{
	// Insert the node ID up front: {"esp8266id":"<id>", <rest of json without leading {>
	String body = F("{\"esp8266id\": \"");
	body += boardChipId();
	body += F("\", ");
	// json starts with '{' - strip it and append the rest.
	body += json.substring(1);

	return httpPost(cfg.host_custom, cfg.port_custom, cfg.url_custom, cfg.ssl_custom,
					"application/json", body,
					cfg.user_custom[0] ? cfg.user_custom : nullptr,
					cfg.pwd_custom);
}
#endif // FEATURE_SENDER_CUSTOM
