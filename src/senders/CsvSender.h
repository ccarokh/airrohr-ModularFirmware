#pragma once
#include "../Features.h"
#if FEATURE_SENDER_CSV
// CSV output over the serial interface (USB). Prints a header line the first
// time, then one value row per cycle. Port of send_csv().

#include "DataSender.h"

class CsvSender : public IDataSender {
public:
	const char *name() const override { return "CSV (USB)"; }
	bool enabled() const override;
	bool send(const std::vector<SensorReadings> &groups, const String &json) override;

private:
	bool _first_line = true;
};

#endif // FEATURE_SENDER_CSV