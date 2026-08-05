#include "SensorManager.h"
#include "../Features.h"
#include "../Board.h"
#include "../Config.h"
#include "../Debug.h"

#include "DhtSensor.h"
#include "SdsSensor.h"
#include "PmsSensor.h"
#include "HpmSensor.h"
#include "PpdSensor.h"
#include "NpmSensor.h"
#include "IpsSensor.h"
#include "Htu21dSensor.h"
#include "BmpSensor.h"
#include "Bmx280Sensor.h"
#include "Sht3xSensor.h"
#include "Scd30Sensor.h"
#include "Ds18b20Sensor.h"
#include "SpsSensor.h"
#include "DnmsSensor.h"
#include "GpsSensor.h"

#include "../util/Derived.h"

#include <ArduinoJson.h>
#include <Wire.h>
#include <math.h>

// Adds derived readings (dew point, sea-level pressure) based on the existing
// temperature/humidity/pressure values of a sensor group. This way they get
// sent everywhere (MQTT/backends) and displayed.
static void addDerivedReadings(SensorReadings &g)
{
	float t = NAN, h = NAN, pPa = NAN;
	String prefix;
	for (const Reading &r : g.readings) {
		if (r.value_type.indexOf("temperature") >= 0) {
			t = r.value.toFloat();
			int u = r.value_type.indexOf('_');
			if (u >= 0) prefix = r.value_type.substring(0, u);
		} else if (r.value_type.indexOf("humidity") >= 0) {
			h = r.value.toFloat();
		} else if (r.value_type.indexOf("pressure") >= 0 && r.value_type.indexOf("sealevel") < 0) {
			pPa = r.value.toFloat();
		}
	}
	const String pre = prefix.length() ? (prefix + "_") : String();
	if (!isnan(t) && !isnan(h)) {
		g.readings.emplace_back(pre + F("dew_point"), String(dewPoint(t, h), 1));
	}
	if (!isnan(t) && !isnan(pPa)) {
		g.readings.emplace_back(pre + F("pressure_sealevel"), String(seaLevelPressurePa(t, pPa), 0));
	}
}

#if defined(ESP8266)
static SoftwareSerial pmSerialSW;   // one PM sensor at a time
static SoftwareSerial gpsSerialSW;
#endif

void SensorManager::begin()
{
	Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL); // I2C on the airrohr pins (platform-dependent)
	_sensors.clear();
#if defined(ESP32)
	SensorSerial &pmSerial = Serial1;
	SensorSerial &gpsSerial = Serial2;
#else
	SensorSerial &pmSerial = pmSerialSW;
	SensorSerial &gpsSerial = gpsSerialSW;
#endif


	// Register the active sensors based on the config. Further sensors
	// (P2) are added here following the same pattern.
#if FEATURE_SENSOR_DHT
	if (cfg.dht_read) {
		_sensors.emplace_back(new DhtSensor(PIN_DHT, DHT_TYPE));
	}
#endif
#if FEATURE_SENSOR_SDS
	if (cfg.sds_read) {
		_sensors.emplace_back(new SdsSensor(pmSerial, PIN_PM_SERIAL_RX, PIN_PM_SERIAL_TX));
	}
#endif
#if FEATURE_SENSOR_PMS
	if (cfg.pms_read) {
		_sensors.emplace_back(new PmsSensor(pmSerial, PIN_PM_SERIAL_RX, PIN_PM_SERIAL_TX));
	}
#endif
#if FEATURE_SENSOR_HPM
	if (cfg.hpm_read) {
		_sensors.emplace_back(new HpmSensor(pmSerial, PIN_PM_SERIAL_RX, PIN_PM_SERIAL_TX));
	}
#endif
#if FEATURE_SENSOR_PPD
	if (cfg.ppd_read) {
		_sensors.emplace_back(new PpdSensor(PPD_PIN_PM1, PPD_PIN_PM2));
	}
#endif
#if FEATURE_SENSOR_NPM
	if (cfg.npm_read) {
		_sensors.emplace_back(new NpmSensor(pmSerial, PIN_PM_SERIAL_RX, PIN_PM_SERIAL_TX));
	}
#endif
#if FEATURE_SENSOR_IPS
	if (cfg.ips_read) {
		_sensors.emplace_back(new IpsSensor(pmSerial, PIN_PM_SERIAL_RX, PIN_PM_SERIAL_TX));
	}
#endif
#if FEATURE_SENSOR_HTU21D
	if (cfg.htu21d_read) {
		_sensors.emplace_back(new Htu21dSensor());
	}
#endif
#if FEATURE_SENSOR_BMP
	if (cfg.bmp_read) {
		_sensors.emplace_back(new BmpSensor());
	}
#endif
#if FEATURE_SENSOR_BMX280
	if (cfg.bmx280_read) {
		_sensors.emplace_back(new Bmx280Sensor());
	}
#endif
#if FEATURE_SENSOR_SHT3X
	if (cfg.sht3x_read) {
		_sensors.emplace_back(new Sht3xSensor());
	}
#endif
#if FEATURE_SENSOR_SCD30
	if (cfg.scd30_read) {
		_sensors.emplace_back(new Scd30Sensor());
	}
#endif
#if FEATURE_SENSOR_DS18B20
	if (cfg.ds18b20_read) {
		_sensors.emplace_back(new Ds18b20Sensor(PIN_ONEWIRE));
	}
#endif
#if FEATURE_SENSOR_SPS30
	if (cfg.sps30_read) {
		_sensors.emplace_back(new SpsSensor());
	}
#endif
#if FEATURE_SENSOR_DNMS
	if (cfg.dnms_read) {
		_sensors.emplace_back(new DnmsSensor());
	}
#endif
#if FEATURE_SENSOR_GPS
	if (cfg.gps_read) {
		_sensors.emplace_back(new GpsSensor(gpsSerial, PIN_GPS_RX, PIN_GPS_TX));
	}
#endif

	for (auto &s : _sensors) {
		bool ok = s->begin();
		s->available = ok;
		LOG_INFO(F("Sensor init: "), String(s->name()) + (ok ? F(" ok") : F(" FEHLER")));
	}
	LOG_INFO(F("Aktive Sensoren: "), String((unsigned)_sensors.size()));
	_cycle_start = millis();
}

void SensorManager::update()
{
	const unsigned long elapsed = millis() - _cycle_start;
	for (auto &s : _sensors) {
		s->update(elapsed);
	}
}

bool SensorManager::shouldSend() const
{
	return (millis() - _cycle_start) >= cfg.sending_intervall_ms;
}

int SensorManager::availability(const char *name) const
{
	for (const auto &s : _sensors) {
		if (strcmp(s->name(), name) == 0) {
			return s->available ? 1 : 0;
		}
	}
	return -1; // not active
}

std::vector<SensorReadings> SensorManager::instantReadings()
{
	std::vector<SensorReadings> out;
	for (auto &s : _sensors) {
		if (!s->canReadInstant()) continue; // skip particulate matter
		SensorReadings g;
		g.sensor = s->name();
		g.apiPin = s->apiPin();
		g.available = s->available;
		s->collect(g.readings); // an immediate instantaneous value for I2C sensors
		addDerivedReadings(g);
		out.push_back(std::move(g));
	}
	return out;
}

std::vector<SensorReadings> SensorManager::collect()
{
	std::vector<SensorReadings> groups;
	for (auto &s : _sensors) {
		SensorReadings g;
		g.sensor = s->name();
		g.apiPin = s->apiPin();
		g.available = s->available;
		s->collect(g.readings);
		addDerivedReadings(g);
		// Keep empty groups too -> every active sensor appears on /values
		// (with "no data"/"not found"). Senders ignore empty groups.
		groups.push_back(std::move(g));
	}
	_cycle_start = millis(); // start the next measurement cycle
	return groups;
}

std::vector<Reading> flattenReadings(const std::vector<SensorReadings> &groups)
{
	std::vector<Reading> flat;
	for (const auto &g : groups) {
		for (const auto &r : g.readings) {
			flat.push_back(r);
		}
	}
	return flat;
}

String buildPayloadJson(const std::vector<Reading> &readings)
{
	DynamicJsonDocument doc(2048);
	doc["software_version"] = AIRROHR_VERSION;
	JsonArray values = doc.createNestedArray("sensordatavalues");
	for (const Reading &r : readings) {
		JsonObject o = values.createNestedObject();
		o["value_type"] = r.value_type;
		o["value"] = r.value;
	}
	String out;
	serializeJson(doc, out);
	return out;
}
