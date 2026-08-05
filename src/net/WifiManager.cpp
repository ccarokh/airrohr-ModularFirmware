#include "WifiManager.h"
#include "../Board.h"
#include "../Config.h"
#include "../Debug.h"

#if defined(ESP32)
  #include <ESPmDNS.h>
#elif defined(ESP8266)
  #include <ESP8266mDNS.h>
#endif

String WifiManager::apSsid() const
{
	// Own, unique AP name.
	if (cfg.fs_ssid[0] && strcmp(cfg.fs_ssid, "airRohr-") != 0) {
		return String(cfg.fs_ssid);
	}
	return String("airRohr-") + boardChipId();
}

bool WifiManager::tryStation()
{
	if (!cfg.wlanssid[0]) {
		return false;
	}
	LOG_INFO(F("WLAN: verbinde mit "), String(cfg.wlanssid));
	WiFi.mode(WIFI_STA);
	WiFi.setSleep(cfg.powersave); // modem sleep optional

	// Optional static IP configuration - only with VALID, non-null values.
	// Otherwise (empty/0.0.0.0/unparseable) fall back to DHCP automatically, so the
	// device does not get stuck with a broken 0.0.0.0 address.
	if (cfg.static_ip[0]) {
		IPAddress ip, gw, mask, dns;
		const IPAddress zero((uint32_t)0);
		const bool ok = ip.fromString(cfg.static_ip) &&
						gw.fromString(cfg.static_gateway) &&
						mask.fromString(cfg.static_subnet) &&
						ip != zero && mask != zero;
		if (ok) {
			dns.fromString(cfg.static_dns);
			WiFi.config(ip, gw, mask, dns);
			LOG_INFO(F("WLAN: statische IP "), String(cfg.static_ip));
		} else {
			LOG_WARN(F("WLAN: statische IP ungültig -> DHCP"));
		}
	}

	WiFi.begin(cfg.wlanssid, cfg.wlanpwd);
	for (int i = 0; i < 40 && WiFi.status() != WL_CONNECTED; ++i) {
		delay(250);
	}
	if (WiFi.status() == WL_CONNECTED) {
		LOG_INFO(F("WLAN verbunden, IP: "), WiFi.localIP().toString());
		// mDNS: reachable at <node>.local
		if (MDNS.begin(boardNodeId().c_str())) {
			MDNS.addService("http", "tcp", 80);
			LOG_INFO(F("mDNS aktiv: "), boardNodeId() + F(".local"));
		}
		return true;
	}
	LOG_WARN(F("WLAN: Verbindung fehlgeschlagen"));
	return false;
}

void WifiManager::startAccessPoint()
{
	_ap = true;
	const String ssid = apSsid();

	WiFi.persistent(false);
	WiFi.disconnect(true);   // clean up any leftover STA state
	delay(100);
	WiFi.mode(WIFI_AP);
	delay(100);

	// Channel 1, visible, max 4 clients; check the return value.
	const bool ok = WiFi.softAP(ssid.c_str(), nullptr, 1, 0, 4);
	delay(200);
	const IPAddress ip = WiFi.softAPIP();

	LOG_INFO(F("AP softAP() Rueckgabe: "), ok ? F("OK") : F("FEHLER"));
	LOG_INFO(F("AP MAC: "), WiFi.softAPmacAddress());
	LOG_INFO(F("AP IP: "), ip.toString());
	LOG_INFO(F("AP SSID: "), ssid);

	// Captive-Portal: redirect all DNS requests to our own IP.
	_dns.start(53, "*", ip);
}

void WifiManager::begin()
{
	if (!tryStation()) {
		startAccessPoint();
	}
}

void WifiManager::handle()
{
	if (_ap) {
		_dns.processNextRequest();
		updateScan(); // maintain the network list in the background (non-blocking)
	}
}

void WifiManager::updateScan()
{
	// Collect and cache a finished async scan result.
	const int16_t st = WiFi.scanComplete();
	if (st >= 0) {
		String j = "[";
		for (int i = 0; i < st && i < 25; ++i) {
			if (i) j += ',';
			String ss = WiFi.SSID(i);
			ss.replace("\\", "\\\\");
			ss.replace("\"", "\\\"");
			j += F("{\"ssid\":\"");
			j += ss;
			j += F("\",\"rssi\":");
			j += String(WiFi.RSSI(i));
			j += '}';
		}
		j += ']';
		_scanJson = j;
		WiFi.scanDelete();
		_scanRunning = false;
	}

	if (st >= 0) {
		LOG_INFO(F("WLAN-Scan fertig, Netze: "), String((int)st));
	}

	// IMPORTANT: Do not scan while an AP client is connected - the mode
	// switch/scan would disrupt the connection (especially on ESP8266).
	// The cache list filled before connecting keeps being served.
	if (WiFi.softAPgetStationNum() > 0) {
		return;
	}

	// Periodically kick off a new asynchronous scan (only without a client).
	if (!_scanRunning && (_lastScanKick == 0 || millis() - _lastScanKick > 15000)) {
		if (WiFi.getMode() != WIFI_AP_STA) {
			WiFi.mode(WIFI_AP_STA);
		}
		WiFi.scanNetworks(true /* async */, false /* no hidden ones */);
		_scanRunning = true;
		_lastScanKick = millis();
		LOG_INFO(F("WLAN-Scan gestartet"), emptyString);
	}
}
