#include "Config.h"
#include "Debug.h"
#include "Features.h"

#include <ArduinoJson.h>
#include <LittleFS.h>

Settings cfg;

namespace {

constexpr char CONFIG_PATH[] = "/config.json";

enum class FieldType : uint8_t { Bool, UInt, Str };

struct Field {
	FieldType   type;
	const char *key;
	void       *ptr;
	uint16_t    len; // only for Str: buffer size
};

// The single place where fields are registered. New config option?
// Add the field to Settings and add one line here.
#define F_BOOL(name) { FieldType::Bool, #name, &cfg.name, 0 }
#define F_UINT(name) { FieldType::UInt, #name, &cfg.name, 0 }
#define F_STR(name)  { FieldType::Str,  #name, cfg.name, sizeof(cfg.name) }

const Field CONFIG_FIELDS[] = {
	F_STR(current_lang),
	F_STR(wlanssid), F_STR(wlanpwd),
	F_STR(www_username), F_STR(www_password),
	F_STR(fs_ssid), F_STR(fs_pwd),
	F_BOOL(www_basicauth_enabled),
	// Only compiled-in features appear in the config UI (Config::field()
	// returns false for missing keys -> the form skips them). This way you
	// cannot "enable" a feature that the build does not even contain.
#if FEATURE_DISPLAY
	F_BOOL(has_display), F_BOOL(has_sh1106), F_BOOL(has_flipped_display),
	F_BOOL(has_lcd1602), F_BOOL(has_lcd1602_27), F_BOOL(has_lcd2004), F_BOOL(has_lcd2004_27),
	F_BOOL(display_wifi_info), F_BOOL(display_device_info),
#endif
	F_STR(temp_correction), F_STR(height_above_sealevel),
#if FEATURE_SENSOR_DNMS
	F_STR(dnms_correction),
#endif

#if FEATURE_SENSOR_SDS
	F_BOOL(sds_read),
#endif
#if FEATURE_SENSOR_PMS
	F_BOOL(pms_read),
#endif
#if FEATURE_SENSOR_HPM
	F_BOOL(hpm_read),
#endif
#if FEATURE_SENSOR_NPM
	F_BOOL(npm_read), F_BOOL(npm_fulltime),
#endif
#if FEATURE_SENSOR_IPS
	F_BOOL(ips_read),
#endif
#if FEATURE_SENSOR_PPD
	F_BOOL(ppd_read),
#endif
#if FEATURE_SENSOR_SPS30
	F_BOOL(sps30_read),
#endif
#if FEATURE_SENSOR_DHT
	F_BOOL(dht_read),
#endif
#if FEATURE_SENSOR_HTU21D
	F_BOOL(htu21d_read),
#endif
#if FEATURE_SENSOR_BMP
	F_BOOL(bmp_read),
#endif
#if FEATURE_SENSOR_BMX280
	F_BOOL(bmx280_read),
#endif
#if FEATURE_SENSOR_SHT3X
	F_BOOL(sht3x_read),
#endif
#if FEATURE_SENSOR_SCD30
	F_BOOL(scd30_read),
#endif
#if FEATURE_SENSOR_DS18B20
	F_BOOL(ds18b20_read),
#endif
#if FEATURE_SENSOR_DNMS
	F_BOOL(dnms_read),
#endif
#if FEATURE_SENSOR_GPS
	F_BOOL(gps_read),
#endif
#if FEATURE_SENSOR_RAIN
	F_BOOL(rain_read), F_STR(rain_dry_adc), F_STR(rain_wet_adc),
#endif
#if FEATURE_SENSOR_WIND
	F_BOOL(wind_read), F_STR(wind_divider), F_STR(wind_factor), F_STR(wind_offset),
#endif

#if FEATURE_SENDER_SENSORCOMMUNITY
	F_BOOL(send2dusti), F_BOOL(ssl_dusti),
#endif
#if FEATURE_SENDER_MADAVI
	F_BOOL(send2madavi), F_BOOL(ssl_madavi),
#endif
#if FEATURE_SENDER_SENSEMAP
	F_BOOL(send2sensemap), F_STR(senseboxid),
#endif
#if FEATURE_SENDER_FSAPP
	F_BOOL(send2fsapp),
#endif
#if FEATURE_SENDER_AIRCMS
	F_BOOL(send2aircms),
#endif

#if FEATURE_SENDER_INFLUX
	F_BOOL(send2influx), F_STR(host_influx), F_STR(url_influx), F_UINT(port_influx),
	F_STR(user_influx), F_STR(pwd_influx), F_STR(measurement_name_influx), F_BOOL(ssl_influx),
#endif

#if FEATURE_SENDER_CUSTOM
	F_BOOL(send2custom), F_STR(host_custom), F_STR(url_custom), F_UINT(port_custom),
	F_STR(user_custom), F_STR(pwd_custom), F_BOOL(ssl_custom),
#endif

#if FEATURE_SENDER_MQTT
	F_BOOL(send2mqtt), F_STR(host_mqtt), F_UINT(port_mqtt),
	F_STR(user_mqtt), F_STR(pwd_mqtt), F_STR(topic_mqtt), F_BOOL(ssl_mqtt),
	F_BOOL(mqtt_ha_discovery), F_UINT(mqtt_intervall_ms),
#endif

	F_STR(static_ip), F_STR(static_subnet), F_STR(static_gateway), F_STR(static_dns),
#if FEATURE_SENDER_CSV
	F_BOOL(send2csv),
#endif
	F_BOOL(powersave),
	F_UINT(debug), F_UINT(sending_intervall_ms), F_UINT(time_for_wifi_config),
};

constexpr size_t FIELD_COUNT = sizeof(CONFIG_FIELDS) / sizeof(CONFIG_FIELDS[0]);

} // namespace

namespace Config {

bool load()
{
	File f = LittleFS.open(CONFIG_PATH, "r");
	if (!f) {
		LOG_WARN(F("Config: keine Datei, nutze Defaults"));
		return false;
	}

	DynamicJsonDocument doc(4096);
	DeserializationError err = deserializeJson(doc, f);
	f.close();
	if (err) {
		LOG_ERROR(F("Config: JSON-Fehler: "), String(err.c_str()));
		return false;
	}

	for (const Field &fld : CONFIG_FIELDS) {
		if (!doc.containsKey(fld.key)) continue;
		switch (fld.type) {
		case FieldType::Bool:
			*static_cast<bool *>(fld.ptr) = doc[fld.key].as<bool>();
			break;
		case FieldType::UInt:
			*static_cast<unsigned *>(fld.ptr) = doc[fld.key].as<unsigned>();
			break;
		case FieldType::Str: {
			const char *v = doc[fld.key].as<const char *>();
			if (v) {
				strlcpy(static_cast<char *>(fld.ptr), v, fld.len);
			}
			break;
		}
		}
	}
	LOG_INFO(F("Config: geladen"));
	return true;
}

bool save()
{
	DynamicJsonDocument doc(4096);
	for (const Field &fld : CONFIG_FIELDS) {
		switch (fld.type) {
		case FieldType::Bool:
			doc[fld.key] = *static_cast<bool *>(fld.ptr);
			break;
		case FieldType::UInt:
			doc[fld.key] = *static_cast<unsigned *>(fld.ptr);
			break;
		case FieldType::Str:
			doc[fld.key] = static_cast<const char *>(fld.ptr);
			break;
		}
	}

	File f = LittleFS.open(CONFIG_PATH, "w");
	if (!f) {
		LOG_ERROR(F("Config: kann Datei nicht schreiben"));
		return false;
	}
	serializeJson(doc, f);
	f.close();
	LOG_INFO(F("Config: gespeichert"));
	return true;
}

size_t fieldCount() { return FIELD_COUNT; }

FieldMeta fieldMeta(size_t i)
{
	const Field &f = CONFIG_FIELDS[i];
	Kind k = (f.type == FieldType::Bool) ? Kind::Bool
		   : (f.type == FieldType::UInt) ? Kind::UInt
										 : Kind::Str;
	return FieldMeta{ k, f.key };
}

String fieldValue(size_t i)
{
	const Field &f = CONFIG_FIELDS[i];
	switch (f.type) {
	case FieldType::Bool: return *static_cast<bool *>(f.ptr) ? "1" : "0";
	case FieldType::UInt: return String(*static_cast<unsigned *>(f.ptr));
	case FieldType::Str:  return String(static_cast<const char *>(f.ptr));
	}
	return String();
}

bool field(const char *key, Kind &kind, String &value)
{
	for (size_t i = 0; i < FIELD_COUNT; ++i) {
		if (strcmp(CONFIG_FIELDS[i].key, key) == 0) {
			kind = fieldMeta(i).kind;
			value = fieldValue(i);
			return true;
		}
	}
	return false;
}

void setField(const char *key, const String &value)
{
	for (const Field &f : CONFIG_FIELDS) {
		if (strcmp(f.key, key) != 0) continue;
		switch (f.type) {
		case FieldType::Bool:
			*static_cast<bool *>(f.ptr) = (value == "1" || value == "on" || value == "true");
			break;
		case FieldType::UInt:
			*static_cast<unsigned *>(f.ptr) = (unsigned)value.toInt();
			break;
		case FieldType::Str:
			strlcpy(static_cast<char *>(f.ptr), value.c_str(), f.len);
			break;
		}
		return;
	}
}

void begin()
{
#if defined(ESP32)
	const bool mounted = LittleFS.begin(true /* formatOnFail */);
#else
	const bool mounted = LittleFS.begin(); // ESP8266: no formatOnFail argument
#endif
	if (!mounted) {
		LOG_ERROR(F("Config: LittleFS mount fehlgeschlagen"));
		return;
	}
	// Set the default AP SSID if empty.
	if (!cfg.fs_ssid[0]) {
		String ap = String("airRohr-") ;
		strlcpy(cfg.fs_ssid, ap.c_str(), sizeof(cfg.fs_ssid));
	}
	load();
	(void)FIELD_COUNT;
}

} // namespace Config
