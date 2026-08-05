#include "../Features.h"
#if FEATURE_SENDER_MADAVI
#include "MadaviSender.h"
#include "HttpPost.h"
#include "../Config.h"

namespace {
constexpr char HOST[] = "api-rrd.madavi.de";
constexpr char URL[]  = "/data.php";
}

bool MadaviSender::enabled() const { return cfg.send2madavi; }

bool MadaviSender::send(const std::vector<SensorReadings> & /*groups*/, const String &json)
{
	return httpPost(HOST, cfg.ssl_madavi ? 443 : 80, URL, cfg.ssl_madavi,
					"application/json", json);
}
#endif // FEATURE_SENDER_MADAVI
