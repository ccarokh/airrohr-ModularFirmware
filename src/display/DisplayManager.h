#pragma once
// Display management. Supports OLED (SSD1306/SH1106, optionally flipped) and
// LCD displays (1602/2004, I2C address 0x27 or 0x3F). Rotates through info and
// measurement pages. Port of display_values()/init_display().

#include "../sensors/Sensor.h"
#include <Arduino.h>
#include <vector>

class OLEDDisplay;        // ThingPulse base class (SSD1306Wire/SH1106Wire)
class LiquidCrystal_I2C;  // LCD

class DisplayManager {
public:
	~DisplayManager();
	void begin();
	void setReadings(const std::vector<Reading> &readings) { _readings = readings; }
	void update();

private:
	void renderOled();
	void renderLcd();

	OLEDDisplay       *_oled = nullptr;
	LiquidCrystal_I2C *_lcd = nullptr;
	uint8_t _lcdCols = 0, _lcdRows = 0;

	std::vector<Reading> _readings;
	unsigned _page = 0;
	unsigned long _last_switch = 0;
};
