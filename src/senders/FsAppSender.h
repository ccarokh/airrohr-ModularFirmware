#pragma once
#include "../Features.h"
#if FEATURE_SENDER_FSAPP
// Particulate-matter app sender (chillibits). POSTs the payload JSON to
// server.chillibits.com/data.php.

#include "DataSender.h"

class FsAppSender : public IDataSender {
public:
	const char *name() const override { return "Feinstaub-App"; }
	bool enabled() const override;
	bool send(const std::vector<SensorReadings> &groups, const String &json) override;
};

#endif // FEATURE_SENDER_FSAPP