// airRohr – modular rewrite. Entry point.
// P0 state: Config + logging + WiFi. Sensors/senders/web/display follow in
// the next phases (see project roadmap).

#include <Arduino.h>

#include "Board.h"
#include "Debug.h"
#include "Config.h"
#include "sensors/SensorManager.h"
#include "senders/SenderManager.h"
#include "web/WebPortal.h"
#include "display/DisplayManager.h"
#include "i18n/Lang.h"
#include "net/TimeSync.h"
#include "net/WifiManager.h"

static SensorManager sensors;
static SenderManager senders;
static WebPortal webPortal;
static DisplayManager display;
static WifiManager wifi;

// Builds the device group: self-telemetry (IP, WiFi, RSSI, uptime, heap,
// firmware). apiPin=0 -> NOT sent to sensor.community, but to Madavi/Custom/
// Influx/MQTT/CSV. This yields useful data even without a connected sensor.
static SensorReadings buildSystemGroup()
{
	SensorReadings sys;
	sys.sensor = "device";
	sys.apiPin = 0;
	// device_* -> MQTT topics airrohr/<node>/device/ssid, /ip, /rssi, ...
	sys.readings.emplace_back(String(F("device_ssid")), WiFi.SSID());
	sys.readings.emplace_back(String(F("device_ip")), WiFi.localIP().toString());
	sys.readings.emplace_back(String(F("device_rssi")), String(WiFi.RSSI()));
	sys.readings.emplace_back(String(F("device_uptime")), String(millis() / 1000));
	sys.readings.emplace_back(String(F("device_heap")), String(ESP.getFreeHeap()));
	sys.readings.emplace_back(String(F("device_firmware")), String(F(AIRROHR_VERSION)));
	return sys;
}

void setup()
{
	Log::begin(115200);
	delay(200);
	LOG_INFO(F("airRohr " AIRROHR_VERSION " startet"));
	LOG_INFO(F("Node-ID: "), boardNodeId());

	Config::begin();
	Log::setLevel(static_cast<LogLevel>(cfg.debug));
	I18n::begin();      // UI language from cfg.current_lang

	wifi.begin();       // connect as STA or open AP / captive portal
	TimeSync::begin();

	sensors.begin();
	senders.begin();
	webPortal.attachWifi(&wifi);
	webPortal.attachSensors(&sensors);
	webPortal.begin();
	display.begin();
}

// Timestamp of the last MQTT publish (for the own, shorter MQTT interval).
static unsigned long lastMqttMs = 0;

void loop()
{
	wifi.handle();      // serve captive-portal DNS (in AP mode)
	sensors.update();   // service sampling sensors
	webPortal.handle(); // handle web requests (incl. firmware upload)
	display.update();   // rotate OLED pages

	if (sensors.shouldSend()) {
		std::vector<SensorReadings> groups = sensors.collect();
		groups.push_back(buildSystemGroup());

		std::vector<Reading> readings = flattenReadings(groups);
		String payload = buildPayloadJson(readings);

		LOG_INFO(F("Messzyklus, Werte: "), String((unsigned)readings.size()));
		LOG_INFO(F("Payload: "), payload);

		webPortal.setLastData(payload);
		webPortal.setLastGroups(groups);
		display.setReadings(readings);
		senders.dispatch(groups, payload);
		lastMqttMs = millis(); // main cycle just serviced MQTT
	}
	// Between cloud cycles, publish more often via MQTT only: live climate values
	// (instantaneous values) + device telemetry. Particulate matter stays on the
	// main cycle (needs the measurement window). Only if a shorter interval is set.
	else if (cfg.mqtt_intervall_ms > 0 &&
			 cfg.mqtt_intervall_ms < cfg.sending_intervall_ms &&
			 senders.mqttEnabled() &&
			 (millis() - lastMqttMs) >= cfg.mqtt_intervall_ms) {
		std::vector<SensorReadings> groups = sensors.instantReadings();
		groups.push_back(buildSystemGroup());
		std::vector<Reading> readings = flattenReadings(groups);
		String payload = buildPayloadJson(readings);
		LOG_INFO(F("MQTT-Zwischenupdate, Werte: "), String((unsigned)readings.size()));
		senders.publishMqtt(groups, payload);
		lastMqttMs = millis();
	}

	delay(10);
}
