#include "SenderManager.h"
#include "../Features.h"
#include "../Debug.h"

#include "SensorCommunitySender.h"
#include "MadaviSender.h"
#include "SensemapSender.h"
#include "FsAppSender.h"
#include "AircmsSender.h"
#include "InfluxSender.h"
#include "HttpJsonSender.h"
#include "CsvSender.h"
#include "MqttSender.h"

void SenderManager::begin()
{
	_senders.clear();
#if FEATURE_SENDER_SENSORCOMMUNITY
	_senders.emplace_back(new SensorCommunitySender());
#endif
#if FEATURE_SENDER_MADAVI
	_senders.emplace_back(new MadaviSender());
#endif
#if FEATURE_SENDER_SENSEMAP
	_senders.emplace_back(new SensemapSender());
#endif
#if FEATURE_SENDER_FSAPP
	_senders.emplace_back(new FsAppSender());
#endif
#if FEATURE_SENDER_AIRCMS
	_senders.emplace_back(new AircmsSender());
#endif
#if FEATURE_SENDER_INFLUX
	_senders.emplace_back(new InfluxSender());
#endif
#if FEATURE_SENDER_CUSTOM
	_senders.emplace_back(new HttpJsonSender());
#endif
#if FEATURE_SENDER_CSV
	_senders.emplace_back(new CsvSender());
#endif
#if FEATURE_SENDER_MQTT
	_senders.emplace_back(new MqttSender());
#endif
}

// Sender name -> compact topic key (lowercase letters/digits only).
static String statusKey(const char *name)
{
	String k;
	for (const char *p = name; *p; ++p) {
		char c = *p;
		if (c >= 'A' && c <= 'Z') c += 32;
		if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) k += c;
	}
	return k;
}

void SenderManager::dispatch(const std::vector<SensorReadings> &groups, const String &json)
{
	if (groups.empty()) return;

	// Status group (apiPin=0 -> not sent to sensor.community). value_type
	// "status_<key>" -> MQTT topic status/<key>.
	SensorReadings st;
	st.sensor = "status";
	st.apiPin = 0;

	IDataSender *mqtt = nullptr;
	for (auto &s : _senders) {
		if (!s->enabled()) continue;
		if (s->isMqtt()) { mqtt = s.get(); continue; } // MQTT last
		LOG_INFO(F("Sende an: "), String(s->name()));
		const bool ok = s->send(groups, json);
		st.readings.emplace_back(String(F("status_")) + statusKey(s->name()),
								 ok ? String(F("ok")) : String(F("error")));
		if (!ok) LOG_WARN(F("Sende-Fehler: "), String(s->name()));
	}

	// MQTT last - together with the status of the others (same-cycle).
	if (mqtt && mqtt->enabled()) {
		st.readings.emplace_back(String(F("status_mqtt")), String(F("ok")));
		std::vector<SensorReadings> withStatus = groups;
		withStatus.push_back(std::move(st));
		LOG_INFO(F("Sende an: "), String(mqtt->name()));
		mqtt->send(withStatus, json);
	}
}

bool SenderManager::publishMqtt(const std::vector<SensorReadings> &groups, const String &json)
{
	for (auto &s : _senders) {
		if (s->isMqtt() && s->enabled()) {
			return s->send(groups, json);
		}
	}
	return false;
}

bool SenderManager::mqttEnabled() const
{
	for (const auto &s : _senders) {
		if (s->isMqtt() && s->enabled()) return true;
	}
	return false;
}
