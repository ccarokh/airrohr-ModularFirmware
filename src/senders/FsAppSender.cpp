#include "../Features.h"
#if FEATURE_SENDER_FSAPP
#include "FsAppSender.h"
#include "HttpPost.h"
#include "../Config.h"

bool FsAppSender::enabled() const { return cfg.send2fsapp; }

bool FsAppSender::send(const std::vector<SensorReadings> & /*groups*/, const String &json)
{
	return httpPost("server.chillibits.com", 80, "/data.php", false,
					"application/json", json);
}
#endif // FEATURE_SENDER_FSAPP
