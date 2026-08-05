#pragma once
#include "../Features.h"
#if FEATURE_SENDER_MADAVI
// Sender for Madavi.de (graphing backend). Sends the complete
// sensordatavalues payload as a JSON POST. Port of the send2madavi branch.

#include "DataSender.h"

class MadaviSender : public IDataSender {
public:
	const char *name() const override { return "Madavi"; } // -> status/madavi
	bool enabled() const override;
	bool send(const std::vector<SensorReadings> &groups, const String &json) override;
};

#endif // FEATURE_SENDER_MADAVI