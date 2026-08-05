#pragma once
#include "../Features.h"
#if FEATURE_SENDER_INFLUX
// InfluxDB sender. Builds the line protocol from the readings and sends it
// via HTTP POST. Port of create_influxdb_string_from_data() + dispatch.

#include "DataSender.h"

class InfluxSender : public IDataSender {
public:
	const char *name() const override { return "InfluxDB"; }
	bool enabled() const override;
	bool send(const std::vector<SensorReadings> &groups, const String &json) override;

private:
	static bool isNumeric(const String &s);
};

#endif // FEATURE_SENDER_INFLUX