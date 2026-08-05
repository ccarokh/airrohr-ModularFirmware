#include "../Features.h"
#if FEATURE_SENDER_SENSORCOMMUNITY
#include "SensorCommunitySender.h"
#include "HttpPost.h"
#include "../Board.h"
#include "../Config.h"
#include "../Debug.h"

#include <ArduinoJson.h>

namespace {
constexpr char HOST[] = "api.sensor.community";
constexpr char URL[]  = "/v1/push-sensor-data/";
}

bool SensorCommunitySender::enabled() const { return cfg.send2dusti; }

bool SensorCommunitySender::send(const std::vector<SensorReadings> &groups, const String & /*json*/)
{
	bool all_ok = true;

	for (const SensorReadings &g : groups) {
		if (g.apiPin == 0 || g.readings.empty()) {
			continue; // this sensor is not reported to sensor.community
		}

		// Build the payload for exactly this sensor; value_type without the sensor prefix.
		DynamicJsonDocument doc(1536);
		doc["software_version"] = AIRROHR_VERSION;
		JsonArray values = doc.createNestedArray("sensordatavalues");
		for (const Reading &r : g.readings) {
			String vt = r.value_type;
			int us = vt.indexOf('_');
			if (us >= 0) {
				vt.remove(0, us + 1); // "SDS_P1" -> "P1", "BME280_temperature" -> "temperature"
			}
			// sensor.community only understands its own schema. Our derived
			// extra values (dew point, sea-level pressure) would otherwise reject
			// the ENTIRE POST (BME280 then shows "no data"). They still go on to
			// MQTT/Madavi/Influx.
			if (vt == F("dew_point") || vt == F("pressure_sealevel")) {
				continue;
			}
			JsonObject o = values.createNestedObject();
			o["value_type"] = vt;
			o["value"] = r.value;
		}
		if (values.size() == 0) {
			continue; // nothing reportable left
		}

		String body;
		serializeJson(doc, body);

		const bool ok = httpPost(HOST, cfg.ssl_dusti ? 443 : 80, URL, cfg.ssl_dusti,
								 "application/json", body, nullptr, nullptr, g.apiPin);
		if (ok) {
			LOG_INFO(F("sensor.community: gesendet, Pin "), String(g.apiPin));
		} else {
			all_ok = false;
		}
	}
	return all_ok;
}
#endif // FEATURE_SENDER_SENSORCOMMUNITY
