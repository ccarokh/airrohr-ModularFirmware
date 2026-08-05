#include "DisplayManager.h"
#include "../Features.h"

#if !FEATURE_DISPLAY
// Display disabled -> no OLED/LCD libraries, only empty stubs.
DisplayManager::~DisplayManager() {}
void DisplayManager::begin() {}
void DisplayManager::update() {}
void DisplayManager::renderOled() {}
void DisplayManager::renderLcd() {}
#else

#include "../Board.h"
#include "../Config.h"
#include "../Debug.h"
#include "../Platform.h"

#include <SSD1306Wire.h>
#include <SH1106Wire.h>
#include <LiquidCrystal_I2C.h>

namespace {
constexpr unsigned long PAGE_INTERVAL_MS = 5000;
constexpr unsigned ROWS_PER_PAGE = 5;
}

DisplayManager::~DisplayManager()
{
	delete _oled;
	delete _lcd;
}

void DisplayManager::begin()
{
	if (cfg.has_display || cfg.has_sh1106) {
		if (cfg.has_sh1106) {
			_oled = new SH1106Wire(OLED_ADDRESS, PIN_I2C_SDA, PIN_I2C_SCL);
		} else {
			_oled = new SSD1306Wire(OLED_ADDRESS, PIN_I2C_SDA, PIN_I2C_SCL);
		}
		_oled->init();
		if (cfg.has_flipped_display) {
			_oled->flipScreenVertically();
		}
		_oled->setFont(ArialMT_Plain_10);
		_oled->setTextAlignment(TEXT_ALIGN_LEFT);
		LOG_INFO(F("Display: OLED initialisiert"));
	} else if (cfg.has_lcd1602 || cfg.has_lcd1602_27 || cfg.has_lcd2004 || cfg.has_lcd2004_27) {
		const uint8_t addr = (cfg.has_lcd1602_27 || cfg.has_lcd2004_27) ? 0x27 : 0x3F;
		_lcdCols = (cfg.has_lcd2004 || cfg.has_lcd2004_27) ? 20 : 16;
		_lcdRows = (cfg.has_lcd2004 || cfg.has_lcd2004_27) ? 4 : 2;
		_lcd = new LiquidCrystal_I2C(addr, _lcdCols, _lcdRows);
		_lcd->init();
		_lcd->backlight();
		LOG_INFO(F("Display: LCD initialisiert"));
	}
}

void DisplayManager::update()
{
	if (!_oled && !_lcd) return;
	const unsigned long now = millis();
	if (now - _last_switch < PAGE_INTERVAL_MS) return;
	_last_switch = now;

	const unsigned rows = _lcd ? _lcdRows : ROWS_PER_PAGE;
	const unsigned data_pages = rows ? (_readings.size() + rows - 1) / rows : 0;
	const unsigned total_pages = 1 + (data_pages ? data_pages : 0);
	_page = (_page + 1) % total_pages;

	if (_oled) renderOled();
	else       renderLcd();
}

void DisplayManager::renderOled()
{
	_oled->clear();
	if (_page == 0) {
		_oled->drawString(0, 0, F("airRohr " AIRROHR_VERSION));
		if (cfg.display_device_info) {
			_oled->drawString(0, 14, String(F("Node: ")) + boardNodeId());
		}
		if (cfg.display_wifi_info) {
			_oled->drawString(0, 28, String(F("IP: ")) + WiFi.localIP().toString());
			_oled->drawString(0, 42, String(F("RSSI: ")) + String(WiFi.RSSI()) + F(" dBm"));
		}
	} else {
		const unsigned start = (_page - 1) * ROWS_PER_PAGE;
		for (unsigned i = 0; i < ROWS_PER_PAGE; ++i) {
			const unsigned idx = start + i;
			if (idx >= _readings.size()) break;
			_oled->drawString(0, i * 12, _readings[idx].value_type + ": " + _readings[idx].value);
		}
	}
	_oled->display();
}

void DisplayManager::renderLcd()
{
	_lcd->clear();
	if (_page == 0) {
		_lcd->setCursor(0, 0);
		_lcd->print("airRohr");
		if (_lcdRows > 1) {
			_lcd->setCursor(0, 1);
			_lcd->print(boardNodeId().c_str());
		}
	} else {
		const unsigned start = (_page - 1) * _lcdRows;
		for (unsigned i = 0; i < _lcdRows; ++i) {
			const unsigned idx = start + i;
			if (idx >= _readings.size()) break;
			_lcd->setCursor(0, i);
			String s = _readings[idx].value_type + ":" + _readings[idx].value;
			_lcd->print(s.substring(0, _lcdCols).c_str());
		}
	}
}

#endif // FEATURE_DISPLAY
