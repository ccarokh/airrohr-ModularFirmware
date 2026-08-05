#pragma once
#include "../Features.h"
#if FEATURE_SENDER_AIRCMS
// aircms.online sender (doiot.ru). Custom format L=<id>&t=<ts>&airrohr=<json>
// with an HMAC-SHA1 signature in the URL. Port of the send2aircms branch.

#include "DataSender.h"

class AircmsSender : public IDataSender {
public:
	const char *name() const override { return "aircms.online"; }
	bool enabled() const override;
	bool send(const std::vector<SensorReadings> &groups, const String &json) override;
};

#endif // FEATURE_SENDER_AIRCMS