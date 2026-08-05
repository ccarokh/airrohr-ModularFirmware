#pragma once
// Configuration: typed Settings struct + persistence on LittleFS.
// Replaces the cfg:: namespace construct and the generated airrohr-cfg.h.
//
// Design: fixed char arrays (no String heap on the MCU), plus a
// descriptor table that maps field <-> JSON key. New fields are added
// in exactly ONE place (Config.cpp: CONFIG_FIELDS).

#include <Arduino.h>

// ---- Field lengths --------------------------------------------------------
#define LEN_LANG        3
#define LEN_SSID        35
#define LEN_PASSWORD    65
#define LEN_USERNAME    65
#define LEN_HOST        100
#define LEN_URL         100
#define LEN_TOPIC       100
#define LEN_MEASUREMENT 100

struct Settings {
	// --- General / network ---
	char  current_lang[LEN_LANG]      = "DE";
	char  wlanssid[LEN_SSID]          = "";
	char  wlanpwd[LEN_PASSWORD]       = "";
	char  www_username[LEN_USERNAME]  = "admin";
	char  www_password[LEN_PASSWORD]  = "";
	char  fs_ssid[LEN_SSID]           = "";  // AP SSID in configuration mode
	char  fs_pwd[LEN_PASSWORD]        = "";
	bool  www_basicauth_enabled       = false;

	// Display
	bool  has_display                 = false; // SSD1306 OLED
	bool  has_sh1106                  = false; // SH1106 OLED
	bool  has_flipped_display         = false;
	bool  has_lcd1602                 = false; // LCD 16x2 (0x3F)
	bool  has_lcd1602_27              = false; // LCD 16x2 (0x27)
	bool  has_lcd2004                 = false; // LCD 20x4 (0x3F)
	bool  has_lcd2004_27              = false; // LCD 20x4 (0x27)
	bool  display_wifi_info           = true;
	bool  display_device_info         = true;

	// Calibration/correction (as strings, like in the original firmware)
	char  temp_correction[8]          = "0";
	char  height_above_sealevel[8]    = "0";
	char  dnms_correction[8]          = "0";

	// --- Sensors (activation) ---
	bool  sds_read   = false;
	bool  pms_read   = false;
	bool  hpm_read   = false;
	bool  npm_read   = false;
	bool  npm_fulltime = false;
	bool  ips_read   = false;
	bool  ppd_read   = false;
	bool  sps30_read = false;
	bool  dht_read   = false;
	bool  htu21d_read = false;
	bool  bmp_read   = false;
	bool  bmx280_read = false;
	bool  sht3x_read = false;
	bool  scd30_read = false;
	bool  ds18b20_read = false;
	bool  dnms_read  = false;
	bool  gps_read   = false;
	bool  rain_read  = false;   // LM393 rain sensor (analog + digital)
	bool  wind_read  = false;   // analog anemometer

	// Calibration of the analog weather sensors (strings like temp_correction).
	// Rain: ADC raw values for "bone dry" and "soaking wet" -> 0-100 %.
	char  rain_dry_adc[8]  = "3000";
	char  rain_wet_adc[8]  = "1200";
	// Wind: U_sensor = U_ADC * divider; speed = (U_sensor - offset) * factor.
	// Defaults follow the carrier board (10k/20k divider -> 1.5).
	char  wind_divider[8]  = "1.5";
	char  wind_factor[8]   = "14.0";
	char  wind_offset[8]   = "0.0";

	// --- Sender: sensor.community / madavi ---
	bool  send2dusti  = true;
	bool  ssl_dusti   = true;
	bool  send2madavi = false;
	bool  ssl_madavi  = false;

	// --- Sender: OpenSenseMap / Feinstaub app / aircms ---
	bool  send2sensemap = false;
	char  senseboxid[30] = "";
	bool  send2fsapp    = false;
	bool  send2aircms   = false;

	// --- Sender: InfluxDB ---
	bool     send2influx = false;
	char     host_influx[LEN_HOST]              = "";
	char     url_influx[LEN_URL]                = "/write?db=sensorcommunity";
	unsigned port_influx                        = 8086;
	char     user_influx[LEN_USERNAME]          = "";
	char     pwd_influx[LEN_PASSWORD]           = "";
	char     measurement_name_influx[LEN_MEASUREMENT] = "feinstaub";
	bool     ssl_influx = false;

	// --- Sender: custom HTTP API ---
	bool     send2custom = false;
	char     host_custom[LEN_HOST]     = "";
	char     url_custom[LEN_URL]       = "/data.php";
	unsigned port_custom               = 80;
	char     user_custom[LEN_USERNAME] = "";
	char     pwd_custom[LEN_PASSWORD]  = "";
	bool     ssl_custom = false;

	// --- Sender: MQTT (new) ---
	bool     send2mqtt = false;
	char     host_mqtt[LEN_HOST]      = "";
	unsigned port_mqtt                = 1883;
	char     user_mqtt[LEN_USERNAME]  = "";
	char     pwd_mqtt[LEN_PASSWORD]   = "";
	char     topic_mqtt[LEN_TOPIC]    = "airrohr";
	bool     ssl_mqtt = false;
	bool     mqtt_ha_discovery = false;
	// Own, usually shorter MQTT interval (live climate values more often than the
	// 145 s cloud cycle). 0 or >= sending_intervall_ms -> only in the main cycle.
	unsigned mqtt_intervall_ms = 30000;

	// --- Network (static) ---
	char  static_ip[16]      = "";
	char  static_subnet[16]  = "";
	char  static_gateway[16] = "";
	char  static_dns[16]     = "";

	// --- Miscellaneous ---
	bool     send2csv = false;
	bool     powersave = false;
	unsigned debug = (unsigned)3;              // corresponds to LogLevel::Info
	unsigned sending_intervall_ms = 145000;
	unsigned time_for_wifi_config = 600000;
};

// Global instance (defined in Config.cpp).
extern Settings cfg;

namespace Config {
// Mount LittleFS and load the configuration (defaults if no file exists).
void begin();
// Write the current configuration to LittleFS. true on success.
bool save();
// Load the configuration from LittleFS. true if a file was read.
bool load();

// --- Field introspection (for the auto-generated web config) ---
enum class Kind : uint8_t { Bool, UInt, Str };
struct FieldMeta {
	Kind        kind;
	const char *key;
};
size_t   fieldCount();
FieldMeta fieldMeta(size_t i);
String   fieldValue(size_t i);              // current value as a string
bool     field(const char *key, Kind &kind, String &value); // lookup by key
void     setField(const char *key, const String &value);    // set by key
} // namespace Config
