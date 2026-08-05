#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_NPM
// Tera Sensor Next PM (NPM) particulate matter sensor (UART). Port of
// fetchSensorNPM() + NPM_start_stop() + NPM_cmd(). Provides mass concentration
// (NPM_P0/P1/P2) and particle count (NPM_N1/N10/N25).
//
// Note: The blocking wait loops of the original firmware are replaced here
// with bounded waits. The plain temp/humidity read (only for the display)
// is not part of the send payload and was omitted.

#include "Sensor.h"
#include "../Board.h"

class NpmSensor : public ISensor {
public:
	NpmSensor(SensorSerial &serial, int8_t rx, int8_t tx)
		: _serial(serial), _rx(rx), _tx(tx) {}

	const char *name() const override { return "NextPM"; }
	int apiPin() const override { return 1; }
	bool begin() override;
	void update(unsigned long ms_since_start) override;
	void collect(std::vector<Reading> &out) override;

private:
	enum class Cmd { State, Change, Concentration };
	// "available >= N" thresholds from the original firmware
	enum Wait16 : uint8_t { HEADER_16 = 16, STATE_16 = 14, BODY_16 = 13, CHECKSUM_16 = 1 };

	void sendCmd(Cmd c);
	bool startStop();               // toggles the sensor, returns the new running state
	static bool checksumValid16(const uint8_t (&d)[16]);
	bool waitAvailable(uint8_t n, unsigned long timeout_ms = 500);
	void resetAccumulators();

	SensorSerial &_serial;
	int8_t _rx, _tx;
	bool   _running = false;
	Wait16 _waiting_for = HEADER_16;

	long _pm1_sum = 0, _pm10_sum = 0, _pm25_sum = 0;
	long _pm1_min = 20000, _pm10_min = 20000, _pm25_min = 20000;
	long _pm1_max = 0, _pm10_max = 0, _pm25_max = 0;
	long _n1_sum = 0, _n10_sum = 0, _n25_sum = 0;
	long _n1_min = 60000, _n10_min = 60000, _n25_min = 60000;
	long _n1_max = 0, _n10_max = 0, _n25_max = 0;
	int  _val_count = 0;
};

#endif // FEATURE_SENSOR_NPM