#include "../Features.h"
#if FEATURE_SENDER_MQTT
#include "MqttSender.h"
#include "../Board.h"
#include "../Config.h"
#include "../Debug.h"

#include "../Platform.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <memory>

bool MqttSender::enabled() const { return cfg.send2mqtt; }

const char *MqttSender::unitForValueType(const String &vt)
{
	if (vt.startsWith("dur"))                              return "";       // PPD pulse duration
	if (vt.startsWith("ratio"))                            return "%";
	if (vt.indexOf("dew_point") >= 0)                     return "°C";
	if (vt.indexOf("temperature") >= 0)                   return "°C";
	if (vt.indexOf("humidity") >= 0)                      return "%";
	if (vt.indexOf("pressure") >= 0)                      return "Pa";
	if (vt.indexOf("co2") >= 0 || vt.indexOf("CO2") >= 0) return "ppm";
	if (vt.indexOf("noise") >= 0)                         return "dB(A)";
	if (vt.indexOf("height") >= 0)                        return "m";
	if (vt == "signal")                                   return "dBm";
	if (vt == "rain_moisture")                            return "%";
	if (vt == "wind_speed" || vt == "wind_gust")          return "m/s";
	if (vt == "wind_voltage")                             return "V";
	if (vt.indexOf("_N") >= 0)                             return "#/cm³";  // particle count
	int p = vt.lastIndexOf('P');                                           // particulate matter mass
	if (p >= 0 && p + 1 < (int)vt.length() && isDigit(vt.charAt(p + 1)))
		return "µg/m³";
	return "";
}

// value_type -> MQTT topic branch with descriptive particulate-matter names:
// "SDS_P1" -> "SDS/PM10", "SDS_P2" -> "SDS/PM2.5", "SPS30_N05" -> "SPS30/NC0.5".
static String mqttSubtopic(const String &vt)
{
	int us = vt.indexOf('_');
	const String prefix = (us >= 0) ? vt.substring(0, us) : String();
	const String meas = (us >= 0) ? vt.substring(us + 1) : vt;
	String m = meas;
	if      (meas == "P0")  m = "PM1";
	else if (meas == "P1")  m = "PM10";
	else if (meas == "P2")  m = "PM2.5";
	else if (meas == "P4")  m = "PM4";
	else if (meas == "P5")  m = "PM5";
	else if (meas == "P01") m = "PM0.1";
	else if (meas == "P03") m = "PM0.3";
	else if (meas == "P05") m = "PM0.5";
	else if (meas == "N05") m = "NC0.5";
	else if (meas == "N1")  m = "NC1";
	else if (meas == "N10") m = "NC10";
	else if (meas == "N25") m = "NC2.5";
	else if (meas == "N4")  m = "NC4";
	else if (meas == "N5")  m = "NC5";
	else if (meas == "N01") m = "NC0.1";
	else if (meas == "N03") m = "NC0.3";
	return prefix.length() ? (prefix + '/' + m) : m;
}

bool MqttSender::send(const std::vector<SensorReadings> &groups, const String & /*json*/)
{
	const std::vector<Reading> readings = flattenReadings(groups);

	std::unique_ptr<WiFiClient> client;
#if FEATURE_TLS
	if (cfg.ssl_mqtt) {
		auto *sc = new WiFiClientSecure;
		sc->setInsecure();
		client.reset(sc);
	} else
#endif
	{
		client.reset(new WiFiClient);
	}

	PubSubClient mqtt(*client);
	mqtt.setServer(cfg.host_mqtt, cfg.port_mqtt);
	mqtt.setBufferSize(512); // room for HA-Discovery config messages

	const String node = boardNodeId();

	bool connected;
	if (cfg.user_mqtt[0] || cfg.pwd_mqtt[0]) {
		connected = mqtt.connect(node.c_str(), cfg.user_mqtt, cfg.pwd_mqtt);
	} else {
		connected = mqtt.connect(node.c_str());
	}
	if (!connected) {
		LOG_ERROR(F("MQTT: Broker-Verbindung fehlgeschlagen, state: "), String(mqtt.state()));
		return false;
	}

	const String base = String(cfg.topic_mqtt) + '/' + node;

	for (const Reading &r : readings) {
		const String topic = base + '/' + mqttSubtopic(r.value_type);
		mqtt.publish(topic.c_str(), r.value.c_str(), true /* retain */);

		if (cfg.mqtt_ha_discovery) {
			// Derive a descriptive name from the mapped topic branch:
			// "SDS/PM2.5" -> slug "SDS_PM2_5" (for unique_id/object_id),
			// display "SDS PM2.5". Otherwise the HA entities would be named "SDS_P1"/"SDS_P2".
			const String friendly = mqttSubtopic(r.value_type); // e.g. "SDS/PM2.5"
			String slug = friendly;
			slug.replace('/', '_');
			slug.replace('.', '_');
			String display = friendly;
			display.replace('/', ' ');

			const String uid = node + '_' + slug;

			// Delete the old, now-renamed retained config entry (empty
			// retained payload). Only if the value_type name changed
			// (P1->PM10, P2->PM2.5, N05->NC0.5 ...), otherwise identical -> do nothing.
			const String legacy_uid = node + '_' + r.value_type;
			if (legacy_uid != uid) {
				String legacy_topic = F("homeassistant/sensor/");
				legacy_topic += legacy_uid;
				legacy_topic += F("/config");
				mqtt.publish(legacy_topic.c_str(), "", true /* retain: deletes */);
			}

			String cfg_topic = F("homeassistant/sensor/");
			cfg_topic += uid;
			cfg_topic += F("/config");

			DynamicJsonDocument ha(512);
			ha[F("name")] = node + ' ' + display;
			ha[F("state_topic")] = topic;
			ha[F("unique_id")] = uid;
			const char *unit = unitForValueType(r.value_type);
			if (*unit) ha[F("unit_of_measurement")] = unit;
			JsonObject dev = ha.createNestedObject(F("device"));
			dev.createNestedArray(F("identifiers")).add(node);
			dev[F("name")] = node;
			dev[F("manufacturer")] = F("sensor.community");
			dev[F("model")] = F(AIRROHR_VERSION);

			String payload;
			serializeJson(ha, payload);
			mqtt.publish(cfg_topic.c_str(), payload.c_str(), true /* retain */);
		}
		mqtt.loop();
	}

	mqtt.disconnect();
	LOG_INFO(F("MQTT: gesendet an "), String(cfg.host_mqtt));
	return true;
}
#endif // FEATURE_SENDER_MQTT
