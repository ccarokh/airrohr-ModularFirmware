#include "FormLabels.h"

namespace FormLabels {

// Tabs in display order.
static const Str TABS[] = { Str::TabWifi, Str::TabSensors, Str::TabTargets, Str::TabSettings };

// Shorthands: L = literal label, T = translated label.
#define L(key, text, tab, grp) { key, text, Str::COUNT, Str::tab, Str::grp }
#define T(key, id, tab, grp)   { key, nullptr, Str::id, Str::tab, Str::grp }

// Ordered form layout. static_ip/subnet/gateway/dns are deliberately missing here:
// they are rendered separately in the WLAN tab behind the DHCP/static toggle.
static const Entry LIST[] = {
	// --- WLAN ---
	T("wlanssid", FldSsid, TabWifi, COUNT),
	T("wlanpwd", FldWifiPassword, TabWifi, COUNT),

	// --- Sensors: grouped by type ---
	L("sds_read", "SDS011", TabSensors, GrpParticulate),
	L("pms_read", "Plantower PMSx003", TabSensors, GrpParticulate),
	L("hpm_read", "Honeywell HPM", TabSensors, GrpParticulate),
	L("npm_read", "Tera NextPM", TabSensors, GrpParticulate),
	T("npm_fulltime", FldNpmFulltime, TabSensors, GrpParticulate),
	L("ips_read", "Piera IPS-7100", TabSensors, GrpParticulate),
	L("ppd_read", "Shinyei PPD42NS", TabSensors, GrpParticulate),
	L("sps30_read", "Sensirion SPS30", TabSensors, GrpParticulate),
	L("dht_read", "DHT22", TabSensors, GrpClimate),
	L("htu21d_read", "HTU21D", TabSensors, GrpClimate),
	L("sht3x_read", "SHT3x", TabSensors, GrpClimate),
	L("bmp_read", "BMP180", TabSensors, GrpClimate),
	L("bmx280_read", "BMP280 / BME280", TabSensors, GrpClimate),
	L("scd30_read", "SCD30 (CO2)", TabSensors, GrpClimate),
	L("ds18b20_read", "DS18B20", TabSensors, GrpClimate),
	L("dnms_read", "DNMS", TabSensors, GrpNoise),
	L("gps_read", "GPS", TabSensors, GrpPosition),
	T("temp_correction", FldTempCorrection, TabSensors, GrpCalibration),
	T("height_above_sealevel", FldHeightAboveSealevel, TabSensors, GrpCalibration),
	T("dnms_correction", FldDnmsCorrection, TabSensors, GrpCalibration),

	// --- Data destinations ---
	T("send2dusti", FldSend2Dusti, TabTargets, GrpScMadavi),
	T("ssl_dusti", FldSslDusti, TabTargets, GrpScMadavi),
	T("send2madavi", FldSend2Madavi, TabTargets, GrpScMadavi),
	T("ssl_madavi", FldSslMadavi, TabTargets, GrpScMadavi),
	T("send2mqtt", FldSend2Mqtt, TabTargets, GrpMqtt),
	T("host_mqtt", FldBrokerHost, TabTargets, GrpMqtt),
	T("port_mqtt", FldPort, TabTargets, GrpMqtt),
	T("user_mqtt", FldUser, TabTargets, GrpMqtt),
	T("pwd_mqtt", FldPassword, TabTargets, GrpMqtt),
	T("topic_mqtt", FldTopicPrefix, TabTargets, GrpMqtt),
	T("ssl_mqtt", FldTls, TabTargets, GrpMqtt),
	T("mqtt_ha_discovery", FldHaDiscovery, TabTargets, GrpMqtt),
	T("mqtt_intervall_ms", FldMqttInterval, TabTargets, GrpMqtt),
	T("send2influx", FldSend2Influx, TabTargets, GrpInflux),
	T("host_influx", FldHost, TabTargets, GrpInflux),
	T("url_influx", FldPath, TabTargets, GrpInflux),
	T("port_influx", FldPort, TabTargets, GrpInflux),
	T("user_influx", FldUser, TabTargets, GrpInflux),
	T("pwd_influx", FldPassword, TabTargets, GrpInflux),
	T("measurement_name_influx", FldMeasurement, TabTargets, GrpInflux),
	T("ssl_influx", FldSslInflux, TabTargets, GrpInflux),
	T("send2custom", FldSend2Custom, TabTargets, GrpCustomApi),
	T("host_custom", FldHost, TabTargets, GrpCustomApi),
	T("url_custom", FldPath, TabTargets, GrpCustomApi),
	T("port_custom", FldPort, TabTargets, GrpCustomApi),
	T("user_custom", FldUser, TabTargets, GrpCustomApi),
	T("pwd_custom", FldPassword, TabTargets, GrpCustomApi),
	T("ssl_custom", FldSslCustom, TabTargets, GrpCustomApi),
	T("send2sensemap", FldSend2Sensemap, TabTargets, GrpOtherServices),
	T("senseboxid", FldSenseboxId, TabTargets, GrpOtherServices),
	T("send2fsapp", FldSend2Fsapp, TabTargets, GrpOtherServices),
	T("send2aircms", FldSend2Aircms, TabTargets, GrpOtherServices),
	T("send2csv", FldSend2Csv, TabTargets, GrpOtherServices),

	// --- Settings ---
	T("current_lang", FldLanguage, TabSettings, GrpGeneral),
	T("sending_intervall_ms", FldSendingInterval, TabSettings, GrpGeneral),
	T("debug", FldDebugLevel, TabSettings, GrpGeneral),
	T("powersave", FldPowersave, TabSettings, GrpGeneral),
	T("www_username", FldWwwUser, TabSettings, GrpAccess),
	T("www_password", FldWwwPassword, TabSettings, GrpAccess),
	T("www_basicauth_enabled", FldBasicAuth, TabSettings, GrpAccess),
	L("has_display", "OLED SSD1306", TabSettings, GrpDisplay),
	L("has_sh1106", "OLED SH1106", TabSettings, GrpDisplay),
	T("has_flipped_display", FldFlippedDisplay, TabSettings, GrpDisplay),
	L("has_lcd1602", "LCD 16x2 (0x3F)", TabSettings, GrpDisplay),
	L("has_lcd1602_27", "LCD 16x2 (0x27)", TabSettings, GrpDisplay),
	L("has_lcd2004", "LCD 20x4 (0x3F)", TabSettings, GrpDisplay),
	L("has_lcd2004_27", "LCD 20x4 (0x27)", TabSettings, GrpDisplay),
	T("display_wifi_info", FldDisplayWifiInfo, TabSettings, GrpDisplay),
	T("display_device_info", FldDisplayDeviceInfo, TabSettings, GrpDisplay),
	T("fs_ssid", FldApSsid, TabSettings, GrpApMode),
	T("fs_pwd", FldApPassword, TabSettings, GrpApMode),
	T("time_for_wifi_config", FldTimeForWifiConfig, TabSettings, GrpApMode),
};

#undef L
#undef T

int count() { return (int)(sizeof(LIST) / sizeof(LIST[0])); }
const Entry &at(int i) { return LIST[i]; }

int tabCount() { return (int)(sizeof(TABS) / sizeof(TABS[0])); }
Str tabId(int i) { return TABS[i]; }

void appendLabel(String &out, const Entry &e)
{
	if (e.literal) out += e.literal;
	else           out += TR(e.label);
}

} // namespace FormLabels
