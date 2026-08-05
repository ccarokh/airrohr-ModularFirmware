#pragma once
// TEMPLATE for a new language. Three steps:
//
//   1. Copy this file to strings_xx.h and replace the prefix XX_ with your
//      ISO code (uppercase), e.g. FR_ for French.
//   2. Features.h: add  #ifndef FEATURE_LANG_XX / #define FEATURE_LANG_XX 0.
//      Default 0 - the language is opted in via build_flags
//      (-D FEATURE_LANG_FR=1), so nobody pays flash for it unasked (~4-5 kB).
//   3. Lang.cpp: add an #include block, a table block (copy the DE block and
//      swap the prefix) and one line in LANGS[].
//
// Every ID from LANG_STRING_IDS (Lang.h) must be defined here - the build
// fails otherwise, which is intentional: a half-translated UI would silently
// fall back to garbage.
//
// The texts are placed into HTML: use entities (&amp;, &ndash;) for
// markup-significant characters. Encoding: UTF-8 without BOM.

#define XX_NAME "<native name of the language>"

// --- Frame / navigation ---
#define XX_AppTitle             "airRohr particulate matter sensor"
#define XX_FooterText           "airRohr &ndash; modular firmware"
#define XX_FirmwareLabel        "Firmware"
#define XX_BackToOverview       "Overview"
#define XX_MenuHeading          "Overview"
#define XX_MenuValues           "Current values"
#define XX_MenuStatus           "Device status"
#define XX_MenuConfig           "Configuration"
#define XX_MenuUpdate           "Firmware update"
#define XX_MenuMetrics          "Metrics"
#define XX_MenuRemoveConfig     "Delete configuration"
#define XX_MenuRestart          "Restart sensor"
#define XX_ConfirmRemoveConfig  "Really delete the configuration?"
#define XX_ConfirmRestart       "Restart the sensor?"
#define XX_BtnSaveRestart       "Save &amp; restart"

// --- WLAN setup / access point ---
#define XX_WifiSetupTitle       "Set up WiFi"
#define XX_ApIntro              "Configuration mode. Please set up WiFi first."
#define XX_WifiNetworks         "WiFi networks"
#define XX_Refresh              "refresh"
#define XX_Loading              "Loading..."
#define XX_NoNetworksYet        "No networks yet - please wait a moment..."
#define XX_StaticIpToggle       "Static IP instead of DHCP"
#define XX_LabelIp              "IP address"
#define XX_LabelSubnet          "Subnet mask"
#define XX_LabelGateway         "Gateway"
#define XX_LabelDns             "DNS server"

// --- Saving ---
#define XX_SavedTitle           "Saved"
#define XX_SavedText            "Saved. The device is restarting."
#define XX_RestartRunning       "Restarting…"
#define XX_WaitingForDevice     "Waiting for the device…"
#define XX_RestartingPlain      "Restarting..."
#define XX_ConfigDeletedPlain   "Configuration deleted. Restarting..."

// --- Current values ---
#define XX_ValuesTitle              "Current values"
#define XX_SecondsSinceMeasurement  "seconds since the last measurement."
#define XX_NoMeasurementYet         "No measurement yet."
#define XX_ThSensor                 "Sensor"
#define XX_ThParameter              "Parameter"
#define XX_ThValue                  "Value"
#define XX_NotFound                 "not found"
#define XX_DeviceRowLabel           "Device"

// --- Device status ---
#define XX_StatusTitle          "Device status"
#define XX_StNode               "Node"
#define XX_StBoard              "Board"
#define XX_StChip               "Chip"
#define XX_StCores              "cores"
#define XX_StFlash              "Flash"
#define XX_StSdk                "SDK"
#define XX_StUptime             "Uptime"
#define XX_StFreeHeap           "Free heap"
#define XX_StWifi               "WiFi"
#define XX_StApMode             "AP mode, IP"
#define XX_StIp                 "IP"

// --- Firmware update ---
#define XX_UpdateTitle          "Firmware update"
#define XX_UpdateIntro          "Select a .bin file and upload it."
#define XX_UpdateButton         "Upload &amp; flash"
#define XX_UpdateOk             "Update OK. Restarting..."
#define XX_UpdateFailed         "Update FAILED."
#define XX_BackPlain            "Back"

// --- Config form: tabs ---
#define XX_TabWifi              "WiFi"
#define XX_TabSensors           "Sensors"
#define XX_TabTargets           "Data destinations"
#define XX_TabSettings          "Settings"

// --- Config form: groups ---
#define XX_GrpParticulate       "Particulate matter"
#define XX_GrpClimate           "Temperature / humidity / pressure"
#define XX_GrpNoise             "Noise"
#define XX_GrpPosition          "Position"
#define XX_GrpWeather           "Weather"
#define XX_GrpCalibration       "Calibration"
#define XX_GrpScMadavi          "sensor.community / Madavi"
#define XX_GrpMqtt              "MQTT"
#define XX_GrpInflux            "InfluxDB"
#define XX_GrpCustomApi         "Custom API"
#define XX_GrpOtherServices     "Other services"
#define XX_GrpGeneral           "General"
#define XX_GrpAccess            "Access"
#define XX_GrpDisplay           "Display"
#define XX_GrpApMode            "Configuration mode"

// --- Config form: field labels ---
#define XX_FldSsid              "WiFi name (SSID)"
#define XX_FldWifiPassword      "WiFi password"
#define XX_FldNpmFulltime       "NextPM continuous mode"
#define XX_FldTempCorrection    "Temperature correction (°C)"
#define XX_FldHeightAboveSealevel "Height above sea level (m)"
#define XX_FldDnmsCorrection    "DNMS correction (dB)"
#define XX_FldRainRead          "Rain sensor (LM393)"
#define XX_FldWindRead          "Anemometer (wind speed)"
#define XX_FldRainDryAdc        "Rain: ADC value when dry"
#define XX_FldRainWetAdc        "Rain: ADC value when wet"
#define XX_FldWindFactor        "Wind: m/s per volt"
#define XX_FldWindDivider       "Wind: divider factor"
#define XX_FldWindOffset        "Wind: zero point (V)"
#define XX_FldSend2Dusti        "Send to sensor.community"
#define XX_FldSslDusti          "sensor.community over HTTPS"
#define XX_FldSend2Madavi       "Send to Madavi.de"
#define XX_FldSslMadavi         "Madavi over HTTPS"
#define XX_FldSend2Mqtt         "Send to MQTT"
#define XX_FldBrokerHost        "Broker (host)"
#define XX_FldPort              "Port"
#define XX_FldUser              "User"
#define XX_FldPassword          "Password"
#define XX_FldTopicPrefix       "Topic prefix"
#define XX_FldTls               "TLS"
#define XX_FldHaDiscovery       "Home Assistant discovery"
#define XX_FldMqttInterval      "MQTT interval (ms, 0 = main cycle)"
#define XX_FldSend2Influx       "Send to InfluxDB"
#define XX_FldHost              "Host"
#define XX_FldPath              "Path"
#define XX_FldMeasurement       "Measurement"
#define XX_FldSslInflux         "InfluxDB over HTTPS"
#define XX_FldSend2Custom       "Send to custom API"
#define XX_FldSslCustom         "over HTTPS"
#define XX_FldSend2Sensemap     "Send to OpenSenseMap"
#define XX_FldSenseboxId        "senseBox ID"
#define XX_FldSend2Fsapp        "Send to Feinstaub app"
#define XX_FldSend2Aircms       "Send to aircms.online"
#define XX_FldSend2Csv          "Print CSV over USB"
#define XX_FldLanguage          "Language"
#define XX_FldSendingInterval   "Sending interval (ms)"
#define XX_FldDebugLevel        "Debug level (0-5)"
#define XX_FldPowersave         "Power save mode (WiFi)"
#define XX_FldWwwUser           "Web user"
#define XX_FldWwwPassword       "Web password"
#define XX_FldBasicAuth         "Protect access with a password"
#define XX_FldFlippedDisplay    "Flip display"
#define XX_FldDisplayWifiInfo   "Show WiFi info"
#define XX_FldDisplayDeviceInfo "Show device info"
#define XX_FldApSsid            "AP name (configuration mode)"
#define XX_FldApPassword        "AP password"
#define XX_FldTimeForWifiConfig "Time for configuration mode (ms)"

// --- Measurement names ---
#define XX_ValTemperature       "Temperature"
#define XX_ValHumidity          "rel. humidity"
#define XX_ValPressure          "Pressure"
#define XX_ValPressureSealevel  "Pressure at sea level"
#define XX_ValDewPoint          "Dew point"
#define XX_ValCo2               "CO2"
#define XX_ValNoiseLaeq         "Noise LAeq"
#define XX_ValNoiseMin          "Noise min"
#define XX_ValNoiseMax          "Noise max"
#define XX_ValLatitude          "Latitude"
#define XX_ValLongitude         "Longitude"
#define XX_ValAltitude          "Altitude"
#define XX_ValTimestamp         "Timestamp"
#define XX_ValRainMoisture      "Rain wetness"
#define XX_ValRainState         "Rain"
#define XX_ValRainAdc           "Rain raw value"
#define XX_ValWindSpeed         "Wind speed"
#define XX_ValWindGust          "Wind gust"
#define XX_ValWindVoltage       "Wind voltage"
#define XX_ValDeviceSsid        "WiFi"
#define XX_ValDeviceIp          "IP address"
#define XX_ValDeviceRssi        "Signal"
#define XX_ValDeviceUptime      "Uptime"
#define XX_ValDeviceHeap        "Free RAM"
#define XX_ValDeviceFirmware    "Firmware"
