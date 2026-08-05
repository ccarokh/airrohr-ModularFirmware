#pragma once
// Configuration web interface. The form is generated automatically from the
// Config field table (single source of truth) – new config options appear
// without any additional web code. Additionally /data.json with the latest
// readings.

#include "../Platform.h"
#include "../sensors/Sensor.h"
#include <Arduino.h>
#include <vector>

class WifiManager;
class SensorManager;

class WebPortal {
public:
	void begin();
	void handle();

	// Attach the WifiManager (for AP-mode detection + cached network list).
	void attachWifi(WifiManager *wm) { _wifi = wm; }

	// Attach the SensorManager (for live availability on /values).
	void attachSensors(SensorManager *sm) { _sensors = sm; }

	// Set the latest measurement payload (sensordatavalues JSON) for /data.json.
	void setLastData(const String &json) { _lastData = json; }

	// Latest measurement groups (per sensor) for the /values status display.
	void setLastGroups(const std::vector<SensorReadings> &g) { _lastGroups = g; _lastMillis = millis(); }

private:
	bool requireAuth();        // Check basic auth (if enabled)
	String pageStart(const String &subtitle); // Head + header (uniform layout)
	static String pageEnd();
	void handleRoot();         // Overview (or WLAN setup in AP mode)
	void handleConfigForm();   // /config: full configuration
	void handleSave();
	void handleDataJson();
	void handleValues();       // /values: human-readable readings table
	void handleStatus();       // /status: device/network status
	void handleMetrics();      // /metrics: Prometheus format
	void handleWifiScan();     // /wifi: network scan as JSON
	void handleReset();        // /reset: restart
	void handleRemoveConfig(); // /removeConfig: delete config + restart
	void handleDebug();        // /debug?level=N: set log level
	void handleFavicon();      // /favicon.svg + /favicon.ico
	void handleUpdatePage();
	void handleUpdateDone();
	void handleUpdateUpload();

	WebServerClass _server{80};
	WifiManager *_wifi = nullptr;
	SensorManager *_sensors = nullptr;
	String _lastData = "{}";
	std::vector<SensorReadings> _lastGroups;
	unsigned long _lastMillis = 0;
};
