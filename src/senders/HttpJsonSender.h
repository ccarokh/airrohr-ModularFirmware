#pragma once
#include "../Features.h"
#if FEATURE_SENDER_CUSTOM
// Sender for the custom HTTP API ("Own API"). Sends the sensordatavalues JSON
// (augmented with the node ID) via POST to a freely configurable endpoint.
// Port of the send2custom branch.

#include "DataSender.h"

class HttpJsonSender : public IDataSender {
public:
	const char *name() const override { return "Custom-API"; }
	bool enabled() const override;
	bool send(const std::vector<SensorReadings> &groups, const String &json) override;
};

#endif // FEATURE_SENDER_CUSTOM