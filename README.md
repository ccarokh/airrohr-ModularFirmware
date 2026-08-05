# airRohr – Modular Firmware

A modular C++/PlatformIO rewrite of the [airRohr particulate-matter firmware](https://github.com/opendata-stuttgart/sensors-software/tree/master/airrohr-firmware) by the [Sensor.Community](https://sensor.community/) project (formerly luftdaten.info) – with **built-in MQTT** as its core feature.

The original firmware is a single ~6300-line `.ino` file. This rewrite splits it into small, testable C++ modules (largest file < 200 lines) and runs **dual-platform on ESP32 and ESP8266**.

> **Why a rewrite?** Built-in MQTT was deliberately left out of the original ([issue #33](https://github.com/opendata-stuttgart/sensors-software/issues/33): memory footprint, inconsistent payload/auth formats). On the ESP32 with enough flash – and, thanks to clean modularization, on the ESP8266 too – this is no longer a blocker.

---

## Features

- **One MQTT topic per measurement** (`airrohr/<node-id>/<sensor>/<measurement>`), e.g. `airrohr/esp8266-1908787/SDS/PM2.5`
- **Home Assistant MQTT discovery** (optional) – entities appear in HA automatically
- **Separate, shorter MQTT interval** for live values (e.g. every 30 s), independent of the cloud send cycle (145 s)
- **Per-backend send status** via MQTT (`status/sensorcommunity`, `status/madavi`, `status/mqtt` = `ok`/`error`)
- **All sensors of the original**: SDS011, PMSx003, HPM, NPM, PPD42, IPS, SPS30, DHT22, HTU21D, BMP180, BMP280/BME280, SHT3x, SCD30, DS18B20, DNMS (noise), GPS
- **Plus two weather sensors** the original does not have: **rain** (LM393, analog + digital) and **wind speed** (analog anemometer, mean + gust)
- **Multi-language web UI**, switchable at runtime (German/English built in, more via `Features.h`)
- **All data targets**: sensor.community, Madavi.de, OpenSenseMap, Feinstaub-App, aircms, InfluxDB, custom HTTP JSON, CSV over USB, MQTT
- **Derived values**: dew point + sea-level pressure (to MQTT/Madavi/InfluxDB; sensor.community computes sea-level pressure itself)
- **Web configuration portal** (tabs, WiFi scan, DHCP/static), **captive portal** in AP mode for initial setup
- **Web OTA**: firmware update by file upload in the browser (no USB required)
- **OLED display** (SSD1306/SH1106) and **LCD** (1602/2004), NTP, Prometheus `/metrics`
- **Compile-time feature switches** (`Features.h`, the single source of truth for what a build contains): disable unused sensors/senders/display/TLS to shrink flash. Disabled features automatically disappear from the web UI.

## Supported Boards

| Platform | Board | Toolchain |
|----------|-------|-----------|
| ESP32 | `esp32dev` (e.g. CP210x) | `espressif32` |
| ESP8266 | NodeMCU v2 (`nodemcuv2`) | `espressif8266` |

Pin assignments per board in [`src/boards/`](src/boards/) (`pins_esp32dev.h`, `pins_nodemcu.h`). Override via `-D BOARD_PINS="boards/pins_x.h"`.

> ⚠️ **Changed on ESP32:** the PM serial port now sits on **GPIO27 (RX) / GPIO33 (TX)** – the same pins as the original firmware (`ext_def.h`, `#ifdef ESP32`) and as the airRohr-ESP32 carrier board. Earlier builds of this firmware used 16/17. If your wiring follows the old assignment, build with `-D PIN_PM_SERIAL_RX=16 -D PIN_PM_SERIAL_TX=17`.

## Prebuilt Firmware Images

Prebuilt `.bin` files are available under **[Releases](../../releases)** – for ESP32 and ESP8266/NodeMCU. They can be flashed directly via **web OTA** (`/update`) or over USB for the first flash. Building yourself is only necessary if you change the feature set.

## Build & Flash

Requires [PlatformIO](https://platformio.org/) (`pip install --user platformio`).

```bash
# ESP32 (full feature set)
pio run -e esp32dev -t upload

# ESP8266 NodeMCU
pio run -e esp8266test -t upload --upload-port /dev/ttyUSB0
```

> **First flash:** after a partition change, run `pio run -t erase` once (otherwise a boot loop caused by the old filesystem). After that the firmware boots cleanly and creates the default config.

**What ends up in a build is decided in [`src/Features.h`](src/Features.h) – nowhere else.** `platformio.ini` names at most a *profile*; what that profile contains is defined in `Features.h`:

| Env | Profile | Contents |
|-----|---------|----------|
| `esp32dev` | – | all features |
| `esp32dev_minimal` | `PROFILE_MINIMAL` | SDS011 + BME280, sensor.community + MQTT, no display |
| `esp8266test` | `PROFILE_ESP8266` | reduced set for the smaller flash/RAM budget |

A profile flips the group defaults (`FEATURE_SENSORS_DEFAULT`, `FEATURE_SENDERS_DEFAULT`) and lists only its exceptions. Add a variant of your own there, not in `platformio.ini`.

`-D FEATURE_X=0` on the command line still overrides everything, but that is for quick experiments – anything permanent belongs in `Features.h`.

After the first flash, further updates are conveniently done via **web OTA** (`/update`).

## Configuration

If no (reachable) WiFi is configured, the device opens its own **access point + captive portal** on startup: an open WiFi network in the form `airRohr-<sensor-id>`. When a client connects, it is redirected to the sensor's web server at `http://192.168.4.1/` – enter WiFi and (optionally) the MQTT broker there.

> The open configuration AP turns itself off after a few minutes. To re-enable it, power-cycle the board.

Afterwards the full configuration is available at `http://<device-ip>/config`. Configurable options include:

- WiFi (with network scan, DHCP or static IP)
- which sensors to poll and at what interval
- which APIs/backends to send to
- display options

Key MQTT settings (tab *Data targets → MQTT*): broker/port, username/password, optional TLS, topic prefix, Home Assistant discovery, MQTT interval.

> Only compiled-in features appear in the web UI. If you disable a sensor or sender via `Features.h`/`build_flags`, it also disappears from the configuration.

## Language

The web UI ships in **German and English**; the language is chosen at runtime under *Settings → General → Language* and takes effect immediately (no reflash). Unlike the original firmware – which builds one binary per language via `-DINTL_DE` – the compiled-in languages are all available in the same image.

Which languages end up in the binary is a compile-time decision in `Features.h` (`FEATURE_LANG_DE`, `FEATURE_LANG_EN`), because each one costs roughly **3–4 kB of flash**.

The **first** language enabled in `Features.h` is the fallback when the stored code (`current_lang`) is not compiled in.

**Adding a language:** copy [`src/i18n/strings_template.h`](src/i18n/strings_template.h) to `strings_xx.h`, translate the entries, then add a `FEATURE_LANG_XX` switch in `Features.h` and one block in `Lang.cpp` (the file header describes all three steps). All string IDs come from a single master list in [`Lang.h`](src/i18n/Lang.h), so a missing translation fails the build instead of silently showing a blank label.

## Weather Sensors (rain + wind)

Two analog sensors that the original firmware does not support, designed for the airRohr-ESP32 carrier board (KiCad project kept next to this repository):

| Sensor | Values (`value_type`) | Default pin (ESP32) |
|--------|-----------------------|---------------------|
| **Rain** (LM393 module) | `rain_moisture` (%), `rain_state` (0/1), `rain_adc` (raw) | AO → GPIO35, DO → GPIO16 |
| **Wind** (analog anemometer) | `wind_speed` (m/s, mean), `wind_gust` (m/s, max), `wind_voltage` (V) | AO → GPIO34 (via divider) |

Both are **switched off at compile time by default** – they are not part of the sensor.community reference hardware. Turn them on in `Features.h` (`FEATURE_SENSOR_RAIN` / `FEATURE_SENSOR_WIND`); only then do they appear in the web UI, where they still have to be enabled per device.

Analog inputs must be on **ADC1** (GPIO32–39) – ADC2 is unusable while WiFi is active – and must not exceed **3.3 V**: run the rain module in 3.3 V mode, feed the anemometer through the R1/R2 divider.

Calibration in the web UI (*Sensors → Calibration*):

- **Rain:** `rain_dry_adc` / `rain_wet_adc` – the raw values of the dry and the soaking-wet plate; in between the firmware interpolates to 0–100 %. Read the current raw value off `/values` (`rain_adc`).
- **Wind:** `wind_divider` (divider factor, 1.5 for 10k/20k), `wind_factor` (m/s per volt at the sensor) and `wind_offset` (dead voltage at standstill). Speed = (U_ADC × divider − offset) × factor.

Wind is averaged over the whole measurement cycle (plus the maximum as gust), so – like the particulate-matter sensors – it only appears at the end of a cycle. Rain is read instantaneously and is therefore also part of the shorter MQTT interim updates.

> Neither sensor is part of the sensor.community schema (no API pin), so both are skipped for sensor.community. They *are* sent to MQTT, Madavi, InfluxDB, custom API and CSV.
>
> On the **ESP8266** there is only one ADC pin (A0). If both sensors are enabled, rain wins and wind is switched off with a warning in the log.

## MQTT Topics (example)

```
airrohr/<node-id>/SDS/PM10          6.40
airrohr/<node-id>/SDS/PM2.5         3.50
airrohr/<node-id>/BME280/temperature 21.8
airrohr/<node-id>/BME280/humidity    55.0
airrohr/<node-id>/BME280/pressure    100790
airrohr/<node-id>/rain/moisture      12.5
airrohr/<node-id>/rain/state         0
airrohr/<node-id>/wind/speed         3.4
airrohr/<node-id>/wind/gust          7.1
airrohr/<node-id>/device/{ssid,ip,rssi,uptime,heap,firmware}
airrohr/<node-id>/status/{sensorcommunity,madavi,mqtt}   ok | error
```

Particulate-matter keys are mapped to readable names (`P1→PM10`, `P2→PM2.5`, `N05→NC0.5`, …).

## Wiring (ESP8266 NodeMCU)

On the NodeMCU this firmware uses the **airrohr standard pinout** – so the wiring is identical to the original firmware. (For ESP32 pins see [`src/boards/pins_esp32dev.h`](src/boards/pins_esp32dev.h).)

Detailed guide incl. schematic in the [original wiki: NodeMCU v2/v3 pinout](https://github.com/opendata-stuttgart/meta/wiki/Pinouts-NodeMCU-v2,-v3).

> **Serial sensors:** RX/TX are always crossed (TX on one side → RX on the other).

| Sensor | Connection |
|--------|-----------|
| **SDS011 / PMSx / Honeywell** (serial) | TX→D1 (GPIO5), RX→D2 (GPIO4), GND→GND, 5V→VU |
| **BMP180 / BMP280 / BME280 / HTU21D / SHT3x / SPS30 / LCD / OLED** (I²C) | SDA→D3 (GPIO0), SCL→D4 (GPIO2), VCC→3V3 (SPS30/LCD/OLED: VU/5V), GND→GND |
| **DHT22** | Data→D7 (GPIO13), VCC→3V3, GND→GND |
| **DS18B20** (OneWire, shares D7 with DHT22) | DQ→D7 (GPIO13), VCC→3V3/VU, GND→GND |
| **GPS NEO-6M** (serial) | TX→D5 (RX, GPIO14), RX→D6 (TX, GPIO12), VCC→3V3, GND→GND |
| **Rain LM393** | AO→A0, DO→D0 (GPIO16), VCC→3V3, GND→GND |
| **Anemometer** (analog) | Signal→A0 (via divider), GND→GND |

> ⚠️ GPS combined with a PM sensor is considered unstable even in the original – use at your own risk.

## Sensor.Community API Pins

To run multiple sensors, Sensor.Community expects a *virtual* API pin during registration at [devices.sensor.community](https://devices.sensor.community/). The firmware uses these fixed values (same as the original):

| Sensor | API pin |
|--------|:------:|
| HPM / PMS / SDS011 / SPS30 | 1 |
| BMP180 / BMP280 | 3 |
| DHT22 / HTU21D / SHT3x | 7 |
| GPS (NEO-6M) | 9 |
| BME280 | 11 |
| DS18B20 | 13 |
| DNMS (noise) | 15 |

## Debug & CSV over USB Serial

On the USB port the device provides human-readable debug output at **115200 baud, 8N1**; the verbosity is set in the configuration (`debug`). The CSV sender (data target *CSV*) outputs the measurements as CSV over the same serial port – set `debug` to *None* so the two don't interfere.

> **Note on the DHT22:** originally an indoor sensor; outdoors it tends to get stuck at 99.9 % rel. humidity after condensation and is UV-sensitive. For outdoor use, **BME280** or **SHT3x** are the better choice.

## Project Structure

```
src/
  main.cpp            Loop: sensors → collect → senders → display
  Config.*            settings struct + field table + LittleFS persistence
  Features.h          compile-time feature switches
  Board.*             platform abstraction (pins, chip ID, serial)
  boards/             pin files per board
  sensors/            ISensor interface + one driver per sensor
  senders/            IDataSender interface + one backend each
  i18n/               UI languages (master ID list + one file per language)
  net/                WiFi manager, NTP
  web/                configuration portal, web OTA
  display/            OLED/LCD
  util/               derived values (dew point, sea-level pressure)
lib/                  bundled sensor libraries
```

## Relation to the Original

This firmware is a standalone rewrite, but it closely follows the original firmware and stays compatible with its data formats (sensor.community/Madavi payload, X-Sensor/X-Pin).

- **Original firmware:** <https://github.com/opendata-stuttgart/sensors-software/tree/master/airrohr-firmware>
- **Sensor.Community project:** <https://sensor.community/>

## License

**GNU General Public License v3.0** – adopted from the original firmware. See [LICENSE](LICENSE) or the [original](https://github.com/opendata-stuttgart/sensors-software/blob/master/airrohr-firmware/LICENSE.md).
