#pragma once
#include "../Features.h"
#if FEATURE_SENDER_SENSORCOMMUNITY
// Sender for sensor.community (formerly luftdaten.info). Sends one POST PER
// sensor with the X-Pin header of the respective sensor type and without the
// value_type prefix (e.g. "SDS_P1" -> "P1"). Port of
// sendSensorCommunity() + the per-sensor dispatch loop.

#include "DataSender.h"

class SensorCommunitySender : public IDataSender {
public:
	const char *name() const override { return "sensor.community"; }
	bool enabled() const override;
	bool send(const std::vector<SensorReadings> &groups, const String &json) override;
};

#endif // FEATURE_SENDER_SENSORCOMMUNITY