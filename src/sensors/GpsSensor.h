#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_GPS
// GPS receiver (UART, NMEA via TinyGPS++). Ported from fetchSensorGPS().
// Provides GPS_lat, GPS_lon, GPS_height, GPS_timestamp.

#include "Sensor.h"
#include "../Board.h"
#include <TinyGPS++.h>

class GpsSensor : public ISensor {
public:
	GpsSensor(SensorSerial &serial, int8_t rx, int8_t tx)
		: _serial(serial), _rx(rx), _tx(tx) {}

	const char *name() const override { return "GPS"; }
	bool canReadInstant() const override { return true; }
	int apiPin() const override { return 9; }
	bool begin() override;
	void update(unsigned long ms_since_start) override;
	void collect(std::vector<Reading> &out) override;

private:
	SensorSerial &_serial;
	int8_t _rx, _tx;
	TinyGPSPlus _gps;

	double _lat = -200, _lon = -200, _alt = -1000;
	String _timestamp = "1970-01-01T00:00:00.000";
};

#endif // FEATURE_SENSOR_GPS