#pragma once
// Runtime language switching for the web UI.
//
// Design: ONE master list of string IDs (LANG_STRING_IDS below) generates both
// the Str enum and, per language, the flash table. A language file
// (strings_de.h, strings_en.h, ...) defines one macro per ID -> a missing
// translation is a COMPILE error, and the table order can never drift.
//
// Which languages end up in the binary is decided at compile time
// (FEATURE_LANG_* in Features.h, ~4-5 kB flash each); choosing between the
// compiled-in languages happens at runtime via the config field current_lang.
//
// Adding a language: copy src/i18n/strings_template.h to strings_xx.h,
// translate, add FEATURE_LANG_XX to Features.h and one block to Lang.cpp.

#include <Arduino.h>

// ---- Master list of all translatable UI strings ---------------------------
// Sensor/device names (SDS011, BME280, MQTT, ...) are deliberately NOT in
// here - they are proper nouns and stay literal in the code.
#define LANG_STRING_IDS(X)                                                     \
	/* Frame / navigation */                                                   \
	X(AppTitle) X(FooterText) X(FirmwareLabel) X(BackToOverview)               \
	X(MenuHeading) X(MenuValues) X(MenuStatus) X(MenuConfig) X(MenuUpdate)     \
	X(MenuMetrics) X(MenuRemoveConfig) X(MenuRestart)                          \
	X(ConfirmRemoveConfig) X(ConfirmRestart)                                   \
	X(BtnSaveRestart)                                                          \
	/* WLAN setup / access point */                                            \
	X(WifiSetupTitle) X(ApIntro) X(WifiNetworks) X(Refresh) X(Loading)         \
	X(NoNetworksYet) X(StaticIpToggle)                                         \
	X(LabelIp) X(LabelSubnet) X(LabelGateway) X(LabelDns)                      \
	/* Saving */                                                               \
	X(SavedTitle) X(SavedText) X(RestartRunning) X(WaitingForDevice)           \
	X(RestartingPlain) X(ConfigDeletedPlain)                                   \
	/* Current values */                                                       \
	X(ValuesTitle) X(SecondsSinceMeasurement) X(NoMeasurementYet)              \
	X(ThSensor) X(ThParameter) X(ThValue) X(NotFound) X(DeviceRowLabel)        \
	/* Device status */                                                        \
	X(StatusTitle) X(StNode) X(StBoard) X(StChip) X(StCores) X(StFlash)        \
	X(StSdk) X(StUptime) X(StFreeHeap) X(StWifi) X(StApMode) X(StIp)           \
	/* Firmware update */                                                      \
	X(UpdateTitle) X(UpdateIntro) X(UpdateButton) X(UpdateOk)                  \
	X(UpdateFailed) X(BackPlain)                                               \
	/* Config form: tabs */                                                    \
	X(TabWifi) X(TabSensors) X(TabTargets) X(TabSettings)                      \
	/* Config form: groups */                                                  \
	X(GrpParticulate) X(GrpClimate) X(GrpNoise) X(GrpPosition) X(GrpWeather)   \
	X(GrpCalibration) X(GrpScMadavi) X(GrpMqtt) X(GrpInflux) X(GrpCustomApi)   \
	X(GrpOtherServices) X(GrpGeneral) X(GrpAccess) X(GrpDisplay) X(GrpApMode)  \
	/* Config form: field labels */                                            \
	X(FldSsid) X(FldWifiPassword)                                              \
	X(FldNpmFulltime) X(FldTempCorrection) X(FldHeightAboveSealevel)           \
	X(FldDnmsCorrection) X(FldRainRead) X(FldWindRead) X(FldRainDryAdc)        \
	X(FldRainWetAdc) X(FldWindFactor) X(FldWindDivider) X(FldWindOffset)       \
	X(FldSend2Dusti) X(FldSslDusti) X(FldSend2Madavi) X(FldSslMadavi)          \
	X(FldSend2Mqtt) X(FldBrokerHost) X(FldPort) X(FldUser) X(FldPassword)      \
	X(FldTopicPrefix) X(FldTls) X(FldHaDiscovery) X(FldMqttInterval)           \
	X(FldSend2Influx) X(FldHost) X(FldPath) X(FldMeasurement) X(FldSslInflux)  \
	X(FldSend2Custom) X(FldSslCustom) X(FldSend2Sensemap) X(FldSenseboxId)     \
	X(FldSend2Fsapp) X(FldSend2Aircms) X(FldSend2Csv)                          \
	X(FldLanguage) X(FldSendingInterval) X(FldDebugLevel) X(FldPowersave)      \
	X(FldWwwUser) X(FldWwwPassword) X(FldBasicAuth)                            \
	X(FldFlippedDisplay) X(FldDisplayWifiInfo) X(FldDisplayDeviceInfo)         \
	X(FldApSsid) X(FldApPassword) X(FldTimeForWifiConfig)                      \
	/* Measurement names (/values, display) */                                 \
	X(ValTemperature) X(ValHumidity) X(ValPressure) X(ValPressureSealevel)     \
	X(ValDewPoint) X(ValCo2) X(ValNoiseLaeq) X(ValNoiseMin) X(ValNoiseMax)     \
	X(ValLatitude) X(ValLongitude) X(ValAltitude) X(ValTimestamp)              \
	X(ValRainMoisture) X(ValRainState) X(ValRainAdc)                           \
	X(ValWindSpeed) X(ValWindGust) X(ValWindVoltage)                           \
	X(ValDeviceSsid) X(ValDeviceIp) X(ValDeviceRssi) X(ValDeviceUptime)        \
	X(ValDeviceHeap) X(ValDeviceFirmware)

enum class Str : uint16_t {
#define LANG_ENUM_ENTRY(id) id,
	LANG_STRING_IDS(LANG_ENUM_ENTRY)
#undef LANG_ENUM_ENTRY
	COUNT
};

namespace I18n {

// Activates the language from cfg.current_lang (falls back to the first
// compiled-in language if the code is unknown).
void begin();

// Switch language by ISO code ("DE", "en", ...). false = not compiled in.
bool setLang(const char *code);

// Active language: uppercase code ("DE") / native name ("Deutsch").
const char *code();
const char *name();

// Compiled-in languages (for the selection field in the web UI).
int         count();
const char *codeAt(int i);
const char *nameAt(int i);

// The string in the active language (always non-null).
const __FlashStringHelper *str(Str id);

} // namespace I18n

// Short form for the UI code: p += TR(Str::MenuStatus);
inline const __FlashStringHelper *TR(Str id) { return I18n::str(id); }
