#include "WebPortal.h"
#include "../Board.h"
#include "../Config.h"
#include "../Debug.h"

#include "FormLabels.h"
#include "../net/WifiManager.h"
#include "../sensors/SensorManager.h"
#include "../util/Correction.h"

#include <math.h>

#if defined(ESP32)
  #include <Update.h>
#endif
#include <ArduinoJson.h>
#include <LittleFS.h>

void WebPortal::begin()
{
	_server.on("/", HTTP_GET, [this]() { handleRoot(); });
	_server.on("/config", HTTP_GET, [this]() { handleConfigForm(); });
	_server.on("/save", HTTP_POST, [this]() { handleSave(); });
	_server.on("/data.json", HTTP_GET, [this]() { handleDataJson(); });
	_server.on("/values", HTTP_GET, [this]() { handleValues(); });
	_server.on("/status", HTTP_GET, [this]() { handleStatus(); });
	_server.on("/metrics", HTTP_GET, [this]() { handleMetrics(); });
	_server.on("/wifi", HTTP_GET, [this]() { handleWifiScan(); });
	_server.on("/reset", HTTP_GET, [this]() { handleReset(); });
	_server.on("/removeConfig", HTTP_GET, [this]() { handleRemoveConfig(); });
	_server.on("/debug", HTTP_GET, [this]() { handleDebug(); });
	_server.on("/favicon.ico", HTTP_GET, [this]() { handleFavicon(); });
	_server.on("/favicon.svg", HTTP_GET, [this]() { handleFavicon(); });
	_server.on("/generate_204", HTTP_GET, [this]() { handleRoot(); });   // Android captive
	_server.on("/fwlink", HTTP_GET, [this]() { handleRoot(); });         // Windows captive
	_server.on("/update", HTTP_GET, [this]() { handleUpdatePage(); });
	// Two-part: completion callback + upload handler (called per chunk).
	_server.on("/update", HTTP_POST,
			   [this]() { handleUpdateDone(); },
			   [this]() { handleUpdateUpload(); });
	// Captive portal: redirect every unknown address to the start page.
	_server.onNotFound([this]() {
		_server.sendHeader("Location", "/", true);
		_server.send(302, F("text/plain"), "");
	});
	_server.begin();
	LOG_INFO(F("Web: Portal gestartet auf Port 80"));
}

void WebPortal::handle()
{
	_server.handleClient();
}

bool WebPortal::requireAuth()
{
	if (!cfg.www_basicauth_enabled) {
		return true;
	}
	if (!_server.authenticate(cfg.www_username, cfg.www_password)) {
		_server.requestAuthentication();
		return false;
	}
	return true;
}

// Shared WLAN scan block (network list with signal bars, fills #wlanssid).
static void appendScanBlock(String &p)
{
	p += F(
		"<p><b>WLAN-Netze</b> &middot; <a href='#' onclick='sc();return false'>aktualisieren</a></p>"
		"<ul id='wl'></ul>"
		"<script>"
		"function bars(r){var lvl=r>=-55?4:r>=-65?3:r>=-75?2:r>=-85?1:0;var h=[5,8,11,14];"
		"var s='<span style=\"display:inline-block;width:20px;height:15px;margin-right:6px\">';"
		"for(var i=0;i<4;i++){s+='<span style=\"display:inline-block;width:3px;margin-right:1px;vertical-align:bottom;'"
		"+'height:'+h[i]+'px;background:'+(i<lvl?'#2a7':'#ccc')+'\"></span>';}return s+'</span>';}"
		"function sc(){var u=document.getElementById('wl');u.innerHTML='Lade...';"
		"fetch('/wifi').then(r=>r.json()).then(function(l){u.innerHTML='';"
		"if(!l.length){u.innerHTML='<li>Noch keine Netze - bitte kurz warten...</li>';"
		"setTimeout(sc,3000);return;}"
		"l.sort(function(a,b){return b.rssi-a.rssi});l.forEach(function(n){"
		"var li=document.createElement('li');li.innerHTML=bars(n.rssi);"
		"li.appendChild(document.createTextNode(n.ssid+' ('+n.rssi+' dBm)'));"
		"li.onclick=function(){document.getElementById('wlanssid').value=n.ssid};"
		"u.appendChild(li)})});}"
		"sc();</script>");
}

String WebPortal::pageStart(const String &subtitle)
{
	String p;
	p.reserve(1400);
	p += F("<!doctype html><html><head><meta charset='utf-8'>"
		   "<meta name='viewport' content='width=device-width,initial-scale=1'>"
		   "<title>airRohr</title>"
		   "<link rel='icon' type='image/svg+xml' href='/favicon.svg'>"
		   "<style>"
		   "*{box-sizing:border-box}body{font-family:sans-serif;margin:0;color:#222;background:#fafafa}"
		   "header{background:#26a69a;color:#fff;padding:1em;display:flex;align-items:center;gap:1em}"
		   "header h1{font-size:1.2em;margin:0}.meta{font-size:.82em;opacity:.95;margin-top:.25em}"
		   "main{max-width:760px;margin:1em auto;padding:0 1em}"
		   "a.btn,button{display:block;width:100%;text-align:left;background:#26a69a;color:#fff;"
		   "padding:.9em 1em;margin:.5em 0;border:0;border-radius:4px;text-decoration:none;font-size:1em;cursor:pointer}"
		   "a.btn:hover,button:hover{background:#1f8e86}"
		   "details{background:#fff;border:1px solid #ddd;border-radius:4px;margin:.5em 0;padding:.2em .9em .6em}"
		   "summary{font-weight:bold;cursor:pointer;padding:.5em 0}"
		   "label{display:block;margin:.4em 0}"
		   ".row{display:flex;align-items:center;justify-content:space-between;gap:1em;"
		   "padding:.5em 0;border-top:1px solid #f0f0f0}.row:first-of-type{border-top:0}"
		   ".row span{flex:1}.row input[type=text],.row input[type=password],.row input[type=number]"
		   "{flex:0 0 55%;max-width:55%;padding:.35em}.row input[type=checkbox]{width:20px;height:20px;flex:0 0 auto}"
		   "input[type=text],input[type=password],input[type=number]{padding:.35em}"
		   ".tabs{display:flex;flex-wrap:wrap;gap:.25em;margin:.8em 0 0}"
		   ".tab{width:auto;display:inline-block;background:#e2e2e2;color:#333;padding:.5em .9em;margin:0;"
		   "border-radius:5px 5px 0 0;font-size:.9em}.tab.active{background:#26a69a;color:#fff}"
		   ".tab:hover{background:#d0d0d0}.tab.active:hover{background:#26a69a}"
		   ".panel{background:#fff;border:1px solid #ddd;border-radius:0 5px 5px 5px;padding:.3em .9em .7em}"
		   ".grp{font-weight:bold;color:#555;margin:.9em 0 .1em;font-size:.95em;border-bottom:2px solid #eee;padding-bottom:.2em}"
		   ".chk{display:flex;align-items:center;gap:.6em;padding:.45em 0;border-top:1px solid #f4f4f4;cursor:pointer}"
		   ".chk input{width:18px;height:18px;flex:0 0 auto;margin:0}"
		   "table{border-collapse:collapse;width:100%;background:#fff}td,th{border:1px solid #ddd;padding:.35em .6em;text-align:left}"
		   "#wl{list-style:none;padding:0}#wl li{cursor:pointer;padding:.35em;border-bottom:1px solid #eee}"
		   "#wl li:hover{background:#e7f5f3}"
		   "footer{color:#888;padding:1em;text-align:center;font-size:.8em}</style></head><body>");
	p += F("<header><svg width='42' height='42' viewBox='0 0 24 24' fill='#fff'>"
		   "<path d='M19 18H6a4 4 0 010-8 5 5 0 019.6-1.5A3.5 3.5 0 0119 18z'/></svg>"
		   "<div><h1>airRohr Feinstaubsensor</h1><div class='meta'>ID: ");
	p += boardNodeId();
	p += F("<br>Firmware: " AIRROHR_VERSION);
	if (subtitle.length()) { p += F(" &middot; "); p += subtitle; }
	p += F("</div></div></header><main>");
	return p;
}

const __FlashStringHelper *WebPortal::pageEnd()
{
	return F("</main><footer>airRohr &ndash; modulare Firmware</footer></body></html>");
}

void WebPortal::handleRoot()
{
	if (!requireAuth()) return;

	// --- AP/configuration mode: only set up WLAN (SSID/PW + DHCP/static) ---
	// Detect the mode via the WifiManager (not WiFi.getMode(), which the scan changes).
	if (_wifi && _wifi->isAccessPoint()) {
		String p = pageStart(F("WLAN einrichten"));
		p.reserve(p.length() + 4000);
		p += F("<p>Konfigurationsmodus. Bitte zuerst das WLAN einrichten. Sensoren, MQTT "
			   "und Datenziele lassen sich anschließend im Heimnetz einstellen.</p>"
			   "<form action='/save' method='POST'>");
		appendScanBlock(p);
		p += F("<label>WLAN-Name (SSID) <input type='text' id='wlanssid' name='wlanssid' value='");
		p += cfg.wlanssid;
		p += F("'></label><label>WLAN-Passwort <input type='password' name='wlanpwd' value=''></label>");

		// DHCP / static IP
		p += F("<label>Statische IP statt DHCP <input type='checkbox' id='us' onchange='tgl()'></label>"
			   "<div id='sb' style='display:none'>");
		const char *sf[] = { "static_ip", "static_subnet", "static_gateway", "static_dns" };
		const char *sl[] = { "IP-Adresse", "Subnetzmaske", "Gateway", "DNS-Server" };
		const char *sv[] = { cfg.static_ip, cfg.static_subnet, cfg.static_gateway, cfg.static_dns };
		for (int i = 0; i < 4; ++i) {
			p += F("<label>");
			p += sl[i];
			p += F(" <input type='text' id='");
			p += sf[i];
			p += F("' name='");
			p += sf[i];
			p += F("' value='");
			p += sv[i];
			p += F("'></label>");
		}
		p += F("</div><script>function tgl(){var c=document.getElementById('us').checked;"
			   "document.getElementById('sb').style.display=c?'block':'none';"
			   "if(!c){['static_ip','static_subnet','static_gateway','static_dns'].forEach("
			   "function(k){document.getElementById(k).value=''})}}"
			   "if(document.getElementById('static_ip').value){document.getElementById('us').checked=true;tgl()}</script>");

		p += F("<button type='submit'>Speichern &amp; Neustart</button></form>");
		p += pageEnd();
		_server.send(200, F("text/html; charset=utf-8"), p);
		return;
	}

	// --- Normal operation: overview menu ---
	String p = pageStart(String());
	p += F("<h2>Übersicht &raquo;</h2>"
		   "<a class='btn' href='/values'>Aktuelle Werte</a>"
		   "<a class='btn' href='/status'>Gerätestatus</a>"
		   "<a class='btn' href='/config'>Konfiguration</a>"
		   "<a class='btn' href='/update'>Firmware-Update</a>"
		   "<a class='btn' href='/metrics'>Metrics</a>"
		   "<a class='btn' href='/removeConfig' onclick=\"return confirm('Konfiguration wirklich löschen?')\">Konfiguration löschen</a>"
		   "<a class='btn' href='/reset' onclick=\"return confirm('Sensor neu starten?')\">Sensor neu starten</a>");
	p += pageEnd();
	_server.send(200, F("text/html; charset=utf-8"), p);
}

// Append a single form field (checkbox on the left / text row).
static void appendField(String &p, const char *key, const char *label,
						Config::Kind kind, const String &val)
{
	const bool isPwd = strstr(key, "pwd") || strstr(key, "password");
	if (kind == Config::Kind::Bool) {
		p += F("<label class='chk'><input type='checkbox' name='");
		p += key;
		p += F("' value='1'");
		if (val == "1") p += F(" checked");
		p += F("> ");
		p += label;
		p += F("</label>");
	} else {
		p += F("<div class='row'><span>");
		p += label;
		p += F("</span><input id='");
		p += key;
		p += F("' type='");
		p += (kind == Config::Kind::UInt) ? F("number") : (isPwd ? F("password") : F("text"));
		p += F("' name='");
		p += key;
		p += F("' value='");
		if (!isPwd) p += val;
		p += F("'></div>");
	}
}

void WebPortal::handleConfigForm()
{
	if (!requireAuth()) return;
	String p = pageStart(F("Konfiguration"));
	p.reserve(p.length() + 16000);

	// Tab bar
	const int tabs = FormLabels::tabCount();
	p += F("<div class='tabs'>");
	for (int t = 0; t < tabs; ++t) {
		p += F("<button type='button' class='tab' onclick='T(");
		p += String(t);
		p += F(")'>");
		p += FormLabels::tabName(t);
		p += F("</button>");
	}
	p += F("</div><form action='/save' method='POST'>");

	// Panels per tab
	const int nf = FormLabels::count();
	for (int t = 0; t < tabs; ++t) {
		const char *tabname = FormLabels::tabName(t);
		p += F("<div class='panel'>");
		String curGroup;
		for (int i = 0; i < nf; ++i) {
			const FormLabels::Entry &e = FormLabels::at(i);
			if (strcmp(e.tab, tabname) != 0) continue;
			if (e.group[0] && curGroup != e.group) {
				curGroup = e.group;
				p += F("<div class='grp'>");
				p += e.group;
				p += F("</div>");
			}
			Config::Kind kind;
			String val;
			if (Config::field(e.key, kind, val)) {
				appendField(p, e.key, e.label, kind, val);
			}
		}
		// In the WLAN tab: DHCP/static-IP toggle (fields hidden by default).
		if (!strcmp(tabname, "WLAN")) {
			p += F("<label class='chk'><input type='checkbox' id='us' onchange='tgl()'> "
				   "Statische IP statt DHCP</label><div id='sb' style='display:none'>");
			const char *sf[] = { "static_ip", "static_subnet", "static_gateway", "static_dns" };
			const char *sl[] = { "IP-Adresse", "Subnetzmaske", "Gateway", "DNS-Server" };
			const char *sv[] = { cfg.static_ip, cfg.static_subnet, cfg.static_gateway, cfg.static_dns };
			for (int k = 0; k < 4; ++k) {
				p += F("<div class='row'><span>");
				p += sl[k];
				p += F("</span><input type='text' id='");
				p += sf[k];
				p += F("' name='");
				p += sf[k];
				p += F("' value='");
				p += sv[k];
				p += F("'></div>");
			}
			p += F("</div>");
		}
		p += F("</div>"); // panel
	}

	p += F("<button type='submit'>Speichern &amp; Neustart</button></form>"
		   "<p><a href='/'>&laquo; Übersicht</a></p>"
		   "<script>"
		   "function T(i){var t=document.querySelectorAll('.tab'),pa=document.querySelectorAll('.panel');"
		   "for(var j=0;j<pa.length;j++){pa[j].style.display=j==i?'block':'none';t[j].className=j==i?'tab active':'tab'}}"
		   "function tgl(){var c=document.getElementById('us').checked;"
		   "document.getElementById('sb').style.display=c?'block':'none';"
		   "var rq=['static_ip','static_subnet','static_gateway'];"
		   "['static_ip','static_subnet','static_gateway','static_dns'].forEach(function(k){"
		   "var el=document.getElementById(k);if(c){el.required=rq.indexOf(k)>=0}else{el.value='';el.required=false}})}"
		   "if(document.getElementById('static_ip').value){document.getElementById('us').checked=true}tgl();"
		   "T(0);</script>");
	p += pageEnd();
	_server.send(200, F("text/html; charset=utf-8"), p);
}

void WebPortal::handleSave()
{
	if (!requireAuth()) return;
	const size_t n = Config::fieldCount();
	for (size_t i = 0; i < n; ++i) {
		const Config::FieldMeta m = Config::fieldMeta(i);
		if (m.kind == Config::Kind::Bool) {
			// Checkboxes are only submitted when "on".
			Config::setField(m.key, _server.hasArg(m.key) ? "1" : "0");
		} else if (_server.hasArg(m.key)) {
			const String v = _server.arg(m.key);
			// Empty password field = leave unchanged (do not overwrite).
			const bool isPwd = strstr(m.key, "pwd") || strstr(m.key, "password");
			if (isPwd && v.length() == 0) continue;
			Config::setField(m.key, v);
		}
	}
	Config::save();

	String p = pageStart(F("Gespeichert"));
	p += F("<p>Gespeichert. Das Gerät startet neu &ndash; du wirst automatisch "
		   "zurückgeleitet, sobald es wieder erreichbar ist.</p>"
		   "<p id='st'>Neustart läuft…</p>"
		   "<script>function chk(){fetch('/',{cache:'no-store'}).then(function(){"
		   "location.href='/'}).catch(function(){document.getElementById('st').textContent="
		   "'Warte auf Gerät…';setTimeout(chk,2000)})}setTimeout(chk,6000);</script>");
	p += pageEnd();
	_server.send(200, F("text/html; charset=utf-8"), p);
	delay(500);
	ESP.restart();
}

void WebPortal::handleDataJson()
{
	_server.send(200, F("application/json"), _lastData);
}

void WebPortal::handleFavicon()
{
	// Teal cloud (same logo as in the header) as an SVG favicon.
	_server.sendHeader(F("Cache-Control"), F("max-age=86400"));
	_server.send(200, F("image/svg+xml"),
				 F("<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24'>"
				   "<path fill='#26a69a' d='M19 18H6a4 4 0 010-8 5 5 0 019.6-1.5A3.5 3.5 0 0119 18z'/></svg>"));
}

// value_type -> human-readable parameter name + unit (+ display scaling).
// empty label -> show value_type raw. scale != 1 -> convert the value (e.g. Pa->hPa).
static void valueInfo(const String &vt, String &label, String &unit, float &scale)
{
	label = ""; unit = ""; scale = 1.0f;

	// Device telemetry
	if (vt == "device_ssid")     { label = "WLAN"; return; }
	if (vt == "device_ip")       { label = "IP-Adresse"; return; }
	if (vt == "device_rssi")     { label = "Signal"; unit = "dBm"; return; }
	if (vt == "device_uptime")   { label = "Uptime"; unit = "s"; return; }
	if (vt == "device_heap")     { label = "Freier RAM"; unit = "Bytes"; return; }
	if (vt == "device_firmware") { label = "Firmware"; return; }

	// Derived values (before the generic "pressure" check!)
	if (vt.indexOf("sealevel") >= 0)  { label = "Luftdruck auf Meereshöhe"; unit = "hPa"; scale = 0.01f; return; }
	if (vt.indexOf("dew_point") >= 0) { label = "Taupunkt"; unit = "°C"; return; }

	// Climate
	if (vt.indexOf("temperature") >= 0) { label = "Temperatur"; unit = "°C"; return; }
	if (vt.indexOf("humidity") >= 0)    { label = "rel. Luftfeuchte"; unit = "%"; return; }
	if (vt.indexOf("pressure") >= 0)    { label = "Luftdruck"; unit = "hPa"; scale = 0.01f; return; }
	if (vt.indexOf("co2") >= 0 || vt.indexOf("CO2") >= 0) { label = "CO2"; unit = "ppm"; return; }
	if (vt.indexOf("noise") >= 0) {
		if (vt.endsWith("min")) label = "Lärm min";
		else if (vt.endsWith("max")) label = "Lärm max";
		else label = "Lärm LAeq";
		unit = "dB(A)"; return;
	}

	// Particulate mass (µg/m³)
	unit = "µg/m³";
	if (vt.endsWith("_P01")) { label = "PM0.1"; return; }
	if (vt.endsWith("_P03")) { label = "PM0.3"; return; }
	if (vt.endsWith("_P05")) { label = "PM0.5"; return; }
	if (vt.endsWith("_P0"))  { label = "PM1"; return; }
	if (vt.endsWith("_P1"))  { label = "PM10"; return; }
	if (vt.endsWith("_P2"))  { label = "PM2.5"; return; }
	if (vt.endsWith("_P4"))  { label = "PM4"; return; }
	if (vt.endsWith("_P5"))  { label = "PM5"; return; }

	// Particle count (#/cm³)
	unit = "#/cm³";
	if (vt.endsWith("_N01")) { label = "NC0.1"; return; }
	if (vt.endsWith("_N03")) { label = "NC0.3"; return; }
	if (vt.endsWith("_N05")) { label = "NC0.5"; return; }
	if (vt.endsWith("_N10")) { label = "NC10"; return; }
	if (vt.endsWith("_N25")) { label = "NC2.5"; return; }
	if (vt.endsWith("_N4"))  { label = "NC4"; return; }
	if (vt.endsWith("_N5"))  { label = "NC5"; return; }
	if (vt.endsWith("_N1"))  { label = "NC1"; return; }

	unit = "";
	// PPD42NS
	if (vt == "P1") { label = "PM10"; return; }
	if (vt == "P2") { label = "PM2.5"; return; }
	if (vt.startsWith("dur"))   { label = "LPO"; unit = "µs"; return; }
	if (vt.startsWith("ratio")) { label = "Ratio"; unit = "%"; return; }

	// GPS
	if (vt == "GPS_lat")       { label = "Breitengrad"; return; }
	if (vt == "GPS_lon")       { label = "Längengrad"; return; }
	if (vt == "GPS_height")    { label = "Höhe"; unit = "m"; return; }
	if (vt == "GPS_timestamp") { label = "Zeitstempel"; return; }
}

void WebPortal::handleValues()
{
	// Configured sensors (config flag -> display name, must match ISensor::name()).
	static const struct { const bool *flag; const char *name; } SENS[] = {
		{ &cfg.sds_read, "SDS011" }, { &cfg.pms_read, "PMSx003" }, { &cfg.hpm_read, "HPM" },
		{ &cfg.npm_read, "NextPM" }, { &cfg.ips_read, "IPS-7100" }, { &cfg.ppd_read, "PPD42NS" },
		{ &cfg.sps30_read, "SPS30" }, { &cfg.dht_read, "DHT" }, { &cfg.htu21d_read, "HTU21D" },
		{ &cfg.bmp_read, "BMP180" }, { &cfg.bmx280_read, "BMx280" }, { &cfg.sht3x_read, "SHT3X" },
		{ &cfg.scd30_read, "SCD30" }, { &cfg.ds18b20_read, "DS18B20" }, { &cfg.dnms_read, "DNMS" },
		{ &cfg.gps_read, "GPS" },
	};

	String out = pageStart(F("Aktuelle Werte"));
	if (_lastMillis) {
		out += F("<p>");
		out += String((millis() - _lastMillis) / 1000);
		out += F(" Sekunden seit der letzten Messung.</p>");
	} else {
		out += F("<p style='color:#888'>Noch keine Messung &ndash; erste Werte nach dem "
				 "Sendeintervall.</p>");
	}
	out += F("<table><tr><th>Sensor</th><th>Parameter</th><th style='text-align:right'>Wert</th></tr>");

	auto renderRow = [&](const char *sensor, const String &vt, const String &rawVal) {
		String label, unit; float scale;
		valueInfo(vt, label, unit, scale);
		String value = rawVal;
		if (rawVal.length() && scale != 1.0f) value = String(rawVal.toFloat() * scale, 2);
		out += F("<tr><td>");
		out += sensor;
		out += F("</td><td>");
		out += label.length() ? label : (vt.length() ? vt : String(F("&mdash;")));
		out += F("</td><td style='text-align:right;white-space:nowrap'>");
		if (rawVal.length()) {
			out += value;
			if (unit.length()) { out += F(" "); out += unit; }
		} else {
			out += F("<span style='color:#aaa'>-.-</span>");
		}
		out += F("</td></tr>");
	};

	// Read live instantaneous values from the instant-capable sensors (I2C climate).
	std::vector<SensorReadings> instant =
		_sensors ? _sensors->instantReadings() : std::vector<SensorReadings>();

	// Always show the configured sensors (values "-.-" until measured).
	for (const auto &sc : SENS) {
		if (!*sc.flag) continue;
		const SensorReadings *g = nullptr;
		for (const SensorReadings &gr : instant) // prefer the live value
			if (strcmp(gr.sensor, sc.name) == 0) { g = &gr; break; }
		if (!g)
			for (const SensorReadings &gr : _lastGroups)
				if (strcmp(gr.sensor, sc.name) == 0) { g = &gr; break; }

		if (g && !g->readings.empty()) {
			// Display name from the detected chip (value_type prefix): BME280 instead of BMx280.
			String disp = sc.name;
			int u = g->readings[0].value_type.indexOf('_');
			if (u > 0) disp = g->readings[0].value_type.substring(0, u);
			for (const Reading &r : g->readings)
				renderRow(disp.c_str(), r.value_type, r.value);
		} else if (_sensors && _sensors->availability(sc.name) == 0) {
			// I2C sensor already not found on the bus at begin().
			out += F("<tr><td>");
			out += sc.name;
			out += F("</td><td>&mdash;</td><td style='text-align:right;color:#b00'>nicht gefunden</td></tr>");
		} else {
			renderRow(sc.name, "", ""); // configured, no data yet -> -.-
		}
	}

	// Device telemetry separately.
	for (const SensorReadings &gr : _lastGroups) {
		if (strcmp(gr.sensor, "device") != 0) continue;
		for (const Reading &r : gr.readings) renderRow("Gerät", r.value_type, r.value);
	}

	out += F("</table><p><a href='/'>&laquo; Übersicht</a></p>");
	out += pageEnd();
	_server.send(200, F("text/html; charset=utf-8"), out);
}

void WebPortal::handleStatus()
{
	String p = pageStart(F("Gerätestatus"));
	p += F("<ul><li>Firmware: " AIRROHR_VERSION "</li><li>Node: ");
	p += boardNodeId();
#if defined(ESP32)
	p += F("</li><li>Board: ");
	p += ARDUINO_BOARD;
	p += F("</li><li>Chip: ");
	p += ESP.getChipModel();
	p += F(" Rev ");
	p += String(ESP.getChipRevision());
	p += F(", ");
	p += String(ESP.getChipCores());
	p += F(" Cores @ ");
	p += String(ESP.getCpuFreqMHz());
	p += F(" MHz</li><li>Flash: ");
	p += String(ESP.getFlashChipSize() / (1024 * 1024));
	p += F(" MB</li><li>SDK: ");
	p += ESP.getSdkVersion();
#elif defined(ESP8266)
	p += F("</li><li>Board: ESP8266</li><li>Flash: ");
	p += String(ESP.getFlashChipRealSize() / (1024 * 1024));
	p += F(" MB</li><li>SDK: ");
	p += ESP.getSdkVersion();
#endif
	p += F("</li><li>Uptime: ");
	p += String(millis() / 1000);
	p += F(" s</li><li>Freier Heap: ");
	p += String(ESP.getFreeHeap());
	p += F(" Bytes</li><li>WLAN: ");
	if (WiFi.getMode() == WIFI_AP) {
		p += F("AP-Modus, IP ");
		p += WiFi.softAPIP().toString();
	} else {
		p += WiFi.SSID();
		p += F(", IP ");
		p += WiFi.localIP().toString();
		p += F(", RSSI ");
		p += String(WiFi.RSSI());
		p += F(" dBm");
	}
	p += F("</li></ul><p><a href='/'>&laquo; Übersicht</a></p>");
	p += pageEnd();
	_server.send(200, F("text/html; charset=utf-8"), p);
}

void WebPortal::handleMetrics()
{
	// Prometheus text format: one line per reading.
	DynamicJsonDocument doc(4096);
	String out;
	if (!deserializeJson(doc, _lastData)) {
		const String node = boardNodeId();
		for (JsonObject m : doc["sensordatavalues"].as<JsonArray>()) {
			String vt = m["value_type"].as<const char *>();
			out += F("airrohr_");
			out += vt;
			out += F("{node=\"");
			out += node;
			out += F("\"} ");
			out += m["value"].as<const char *>();
			out += '\n';
		}
	}
	out += F("airrohr_uptime_seconds ");
	out += String(millis() / 1000);
	out += '\n';
	_server.send(200, F("text/plain; version=0.0.4"), out);
}

void WebPortal::handleWifiScan()
{
	// Returns the cached network list maintained in the background – NO
	// blocking scan here (that would kick out AP clients).
	_server.send(200, F("application/json"),
				 _wifi ? _wifi->scanJson() : String(F("[]")));
}

void WebPortal::handleReset()
{
	if (!requireAuth()) return;
	_server.send(200, F("text/html"), F("<html><body>Neustart...</body></html>"));
	delay(300);
	ESP.restart();
}

void WebPortal::handleRemoveConfig()
{
	if (!requireAuth()) return;
	LittleFS.remove("/config.json");
	_server.send(200, F("text/html"), F("<html><body>Config gelöscht. Neustart...</body></html>"));
	delay(300);
	ESP.restart();
}

void WebPortal::handleDebug()
{
	if (!requireAuth()) return;
	if (_server.hasArg("level")) {
		const unsigned lvl = (unsigned)_server.arg("level").toInt();
		cfg.debug = lvl;
		Log::setLevel(static_cast<LogLevel>(lvl));
	}
	_server.send(200, F("text/plain"), String(F("debug level = ")) + String(cfg.debug));
}

// --- Firmware update via the web UI (OTA into the second app slot) ---------

void WebPortal::handleUpdatePage()
{
	if (!requireAuth()) return;
	_server.send(200, F("text/html; charset=utf-8"),
				 F("<!doctype html><html><head><meta charset='utf-8'>"
				   "<meta name='viewport' content='width=device-width,initial-scale=1'>"
				   "<title>Firmware-Update</title></head><body>"
				   "<h1>Firmware-Update</h1>"
				   "<p>.bin-Datei auswählen und hochladen. Das Gerät flasht in den "
				   "zweiten App-Slot und startet neu.</p>"
				   "<form method='POST' action='/update' enctype='multipart/form-data'>"
				   "<input type='file' name='firmware' accept='.bin'> "
				   "<button type='submit'>Hochladen &amp; flashen</button></form>"
				   "<p><a href='/'>Zurück</a></p></body></html>"));
}

void WebPortal::handleUpdateDone()
{
	const bool ok = !Update.hasError();
	_server.send(200, F("text/html; charset=utf-8"),
				 ok ? F("<html><body><p>Update OK. Neustart...</p>"
						"<script>setTimeout(function(){location.href='/'},8000)</script></body></html>")
					: F("<html><body><p>Update FEHLGESCHLAGEN.</p><p><a href='/update'>Zurück</a></p></body></html>"));
	delay(500);
	if (ok) {
		ESP.restart();
	}
}

void WebPortal::handleUpdateUpload()
{
	HTTPUpload &up = _server.upload();
	switch (up.status) {
	case UPLOAD_FILE_START: {
		LOG_INFO(F("OTA: Upload startet: "), up.filename);
#if defined(ESP32)
		const size_t maxSize = UPDATE_SIZE_UNKNOWN;
#else
		const size_t maxSize = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
#endif
		if (!Update.begin(maxSize)) {
			Update.printError(Serial);
		}
		break;
	}
	case UPLOAD_FILE_WRITE:
		if (Update.write(up.buf, up.currentSize) != up.currentSize) {
			Update.printError(Serial);
		}
		break;
	case UPLOAD_FILE_END:
		if (Update.end(true /* set size if applicable */)) {
			LOG_INFO(F("OTA: geschrieben, Bytes: "), String(up.totalSize));
		} else {
			Update.printError(Serial);
		}
		break;
	default:
		break;
	}
}
