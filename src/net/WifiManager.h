#pragma once
// WiFi management: connects as a station (STA) to the configured network.
// If that fails (or no SSID is set), an own access point with captive-portal
// DNS is opened, so WiFi/MQTT can be configured directly on the device.
// Replaces the WiFiManager library of the original firmware.

#include "../Platform.h"
#include <DNSServer.h>

class WifiManager {
public:
	void begin();
	void handle(); // call in loop() (captive DNS + background WiFi scan)

	bool   isAccessPoint() const { return _ap; }
	String apSsid() const;

	// Cached network list (JSON) from the asynchronous background scan.
	const String &scanJson() const { return _scanJson; }

private:
	bool tryStation();
	void startAccessPoint();
	void updateScan(); // kick off an asynchronous scan / cache the result

	DNSServer     _dns;
	bool          _ap = false;

	String        _scanJson = "[]";
	unsigned long _lastScanKick = 0;
	bool          _scanRunning = false;
};
