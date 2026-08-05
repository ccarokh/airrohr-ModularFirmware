#include "FormLabels.h"

namespace FormLabels {

// Tabs in display order.
static const char *TABS[] = { "WLAN", "Sensoren", "Datenziele", "Einstellungen" };

// Ordered form layout. static_ip/subnet/gateway/dns are deliberately missing here:
// they are rendered separately in the WLAN tab behind the DHCP/static toggle.
static const Entry L[] = {
	// --- WLAN ---
	{ "wlanssid", "WLAN-Name (SSID)", "WLAN", "" },
	{ "wlanpwd", "WLAN-Passwort", "WLAN", "" },

	// --- Sensors: grouped by type ---
	{ "sds_read", "SDS011", "Sensoren", "Feinstaub" },
	{ "pms_read", "Plantower PMSx003", "Sensoren", "Feinstaub" },
	{ "hpm_read", "Honeywell HPM", "Sensoren", "Feinstaub" },
	{ "npm_read", "Tera NextPM", "Sensoren", "Feinstaub" },
	{ "npm_fulltime", "NextPM Dauerbetrieb", "Sensoren", "Feinstaub" },
	{ "ips_read", "Piera IPS-7100", "Sensoren", "Feinstaub" },
	{ "ppd_read", "Shinyei PPD42NS", "Sensoren", "Feinstaub" },
	{ "sps30_read", "Sensirion SPS30", "Sensoren", "Feinstaub" },
	{ "dht_read", "DHT22", "Sensoren", "Temperatur / Feuchte / Druck" },
	{ "htu21d_read", "HTU21D", "Sensoren", "Temperatur / Feuchte / Druck" },
	{ "sht3x_read", "SHT3x", "Sensoren", "Temperatur / Feuchte / Druck" },
	{ "bmp_read", "BMP180", "Sensoren", "Temperatur / Feuchte / Druck" },
	{ "bmx280_read", "BMP280 / BME280", "Sensoren", "Temperatur / Feuchte / Druck" },
	{ "scd30_read", "SCD30 (CO2)", "Sensoren", "Temperatur / Feuchte / Druck" },
	{ "ds18b20_read", "DS18B20", "Sensoren", "Temperatur / Feuchte / Druck" },
	{ "dnms_read", "DNMS", "Sensoren", "Lärm" },
	{ "gps_read", "GPS", "Sensoren", "Position" },
	{ "temp_correction", "Temperatur-Korrektur (°C)", "Sensoren", "Kalibrierung" },
	{ "height_above_sealevel", "Höhe über NN (m)", "Sensoren", "Kalibrierung" },
	{ "dnms_correction", "DNMS-Korrektur (dB)", "Sensoren", "Kalibrierung" },

	// --- Data destinations ---
	{ "send2dusti", "An sensor.community senden", "Datenziele", "sensor.community / Madavi" },
	{ "ssl_dusti", "sensor.community über HTTPS", "Datenziele", "sensor.community / Madavi" },
	{ "send2madavi", "An Madavi.de senden", "Datenziele", "sensor.community / Madavi" },
	{ "ssl_madavi", "Madavi über HTTPS", "Datenziele", "sensor.community / Madavi" },
	{ "send2mqtt", "An MQTT senden", "Datenziele", "MQTT" },
	{ "host_mqtt", "Broker (Host)", "Datenziele", "MQTT" },
	{ "port_mqtt", "Port", "Datenziele", "MQTT" },
	{ "user_mqtt", "Benutzer", "Datenziele", "MQTT" },
	{ "pwd_mqtt", "Passwort", "Datenziele", "MQTT" },
	{ "topic_mqtt", "Topic-Präfix", "Datenziele", "MQTT" },
	{ "ssl_mqtt", "TLS", "Datenziele", "MQTT" },
	{ "mqtt_ha_discovery", "Home Assistant Discovery", "Datenziele", "MQTT" },
	{ "mqtt_intervall_ms", "MQTT-Intervall (ms, 0 = Hauptzyklus)", "Datenziele", "MQTT" },
	{ "send2influx", "An InfluxDB senden", "Datenziele", "InfluxDB" },
	{ "host_influx", "Host", "Datenziele", "InfluxDB" },
	{ "url_influx", "Pfad", "Datenziele", "InfluxDB" },
	{ "port_influx", "Port", "Datenziele", "InfluxDB" },
	{ "user_influx", "Benutzer", "Datenziele", "InfluxDB" },
	{ "pwd_influx", "Passwort", "Datenziele", "InfluxDB" },
	{ "measurement_name_influx", "Measurement", "Datenziele", "InfluxDB" },
	{ "ssl_influx", "InfluxDB über HTTPS", "Datenziele", "InfluxDB" },
	{ "send2custom", "An eigene API senden", "Datenziele", "Eigene API" },
	{ "host_custom", "Host", "Datenziele", "Eigene API" },
	{ "url_custom", "Pfad", "Datenziele", "Eigene API" },
	{ "port_custom", "Port", "Datenziele", "Eigene API" },
	{ "user_custom", "Benutzer", "Datenziele", "Eigene API" },
	{ "pwd_custom", "Passwort", "Datenziele", "Eigene API" },
	{ "ssl_custom", "über HTTPS", "Datenziele", "Eigene API" },
	{ "send2sensemap", "An OpenSenseMap senden", "Datenziele", "Weitere Dienste" },
	{ "senseboxid", "senseBox-ID", "Datenziele", "Weitere Dienste" },
	{ "send2fsapp", "An Feinstaub-App senden", "Datenziele", "Weitere Dienste" },
	{ "send2aircms", "An aircms.online senden", "Datenziele", "Weitere Dienste" },
	{ "send2csv", "CSV über USB ausgeben", "Datenziele", "Weitere Dienste" },

	// --- Settings ---
	{ "current_lang", "Sprache", "Einstellungen", "Allgemein" },
	{ "sending_intervall_ms", "Sendeintervall (ms)", "Einstellungen", "Allgemein" },
	{ "debug", "Debug-Level (0-5)", "Einstellungen", "Allgemein" },
	{ "powersave", "Stromsparmodus (WLAN)", "Einstellungen", "Allgemein" },
	{ "www_username", "Web-Benutzer", "Einstellungen", "Zugang" },
	{ "www_password", "Web-Passwort", "Einstellungen", "Zugang" },
	{ "www_basicauth_enabled", "Zugriff mit Passwort schützen", "Einstellungen", "Zugang" },
	{ "has_display", "OLED SSD1306", "Einstellungen", "Anzeige" },
	{ "has_sh1106", "OLED SH1106", "Einstellungen", "Anzeige" },
	{ "has_flipped_display", "Anzeige gespiegelt", "Einstellungen", "Anzeige" },
	{ "has_lcd1602", "LCD 16x2 (0x3F)", "Einstellungen", "Anzeige" },
	{ "has_lcd1602_27", "LCD 16x2 (0x27)", "Einstellungen", "Anzeige" },
	{ "has_lcd2004", "LCD 20x4 (0x3F)", "Einstellungen", "Anzeige" },
	{ "has_lcd2004_27", "LCD 20x4 (0x27)", "Einstellungen", "Anzeige" },
	{ "display_wifi_info", "WLAN-Infos anzeigen", "Einstellungen", "Anzeige" },
	{ "display_device_info", "Geräte-Infos anzeigen", "Einstellungen", "Anzeige" },
	{ "fs_ssid", "AP-Name (Konfig-Modus)", "Einstellungen", "Konfig-Modus" },
	{ "fs_pwd", "AP-Passwort", "Einstellungen", "Konfig-Modus" },
	{ "time_for_wifi_config", "Zeit für Konfig-Modus (ms)", "Einstellungen", "Konfig-Modus" },
};

int count() { return (int)(sizeof(L) / sizeof(L[0])); }
const Entry &at(int i) { return L[i]; }

int tabCount() { return (int)(sizeof(TABS) / sizeof(TABS[0])); }
const char *tabName(int i) { return TABS[i]; }

} // namespace FormLabels
