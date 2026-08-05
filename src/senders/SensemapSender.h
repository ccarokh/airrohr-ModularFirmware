#pragma once
#include "../Features.h"
#if FEATURE_SENDER_SENSEMAP
// OpenSenseMap sender. POSTs the payload JSON to ingress.opensensemap.org
// (HTTPS) at /boxes/<senseboxid>/data?luftdaten=1.

#include "DataSender.h"

class SensemapSender : public IDataSender {
public:
	const char *name() const override { return "OpenSenseMap"; }
	bool enabled() const override;
	bool send(const std::vector<SensorReadings> &groups, const String &json) override;
};

#endif // FEATURE_SENDER_SENSEMAP