#include "../Features.h"
#if FEATURE_SENDER_SENSEMAP
#include "SensemapSender.h"
#include "HttpPost.h"
#include "../Config.h"

bool SensemapSender::enabled() const { return cfg.send2sensemap && cfg.senseboxid[0]; }

bool SensemapSender::send(const std::vector<SensorReadings> & /*groups*/, const String &json)
{
	String path = F("/boxes/");
	path += cfg.senseboxid;
	path += F("/data?luftdaten=1");
	return httpPost("ingress.opensensemap.org", 443, path.c_str(), true,
					"application/json", json);
}
#endif // FEATURE_SENDER_SENSEMAP
