#include "../Features.h"
#if FEATURE_SENSOR_GPS
#include "GpsSensor.h"
#include "../Debug.h"

bool GpsSensor::begin()
{
	_serial.begin(9600, SENSOR_SERIAL_8N1, _rx, _tx);
	return true;
}

void GpsSensor::update(unsigned long)
{
	while (_serial.available() > 0) {
		_gps.encode(_serial.read());
	}

	if (_gps.location.isUpdated()) {
		if (_gps.location.isValid()) {
			_lat = _gps.location.lat();
			_lon = _gps.location.lng();
		} else {
			_lat = -200;
			_lon = -200;
		}
		_alt = _gps.altitude.isValid() ? _gps.altitude.meters() : -1000;

		if (_gps.date.isValid() && _gps.time.isValid()) {
			char buf[37];
			snprintf_P(buf, sizeof(buf), PSTR("%04d-%02d-%02dT%02d:%02d:%02d.%03d"),
					   _gps.date.year(), _gps.date.month(), _gps.date.day(),
					   _gps.time.hour(), _gps.time.minute(), _gps.time.second(),
					   _gps.time.centisecond());
			_timestamp = buf;
		} else {
			_timestamp = F("1970-01-01T00:00:00.000");
		}
	}
}

void GpsSensor::collect(std::vector<Reading> &out)
{
	out.emplace_back(F("GPS_lat"), String(_lat, 6));
	out.emplace_back(F("GPS_lon"), String(_lon, 6));
	out.emplace_back(F("GPS_height"), (float)_alt, 1);
	out.emplace_back(F("GPS_timestamp"), _timestamp);
}
#endif // FEATURE_SENSOR_GPS
