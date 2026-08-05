#include "../Features.h"
#if FEATURE_SENDER_CSV
#include "CsvSender.h"
#include "../Config.h"

bool CsvSender::enabled() const { return cfg.send2csv; }

bool CsvSender::send(const std::vector<SensorReadings> &groups, const String & /*json*/)
{
	const std::vector<Reading> readings = flattenReadings(groups);

	if (_first_line) {
		String head = F("Timestamp_ms;");
		for (const Reading &r : readings) {
			head += r.value_type;
			head += ';';
		}
		if (head.endsWith(";")) head.remove(head.length() - 1);
		Serial.println(head);
		_first_line = false;
	}

	String line(millis());
	line += ';';
	for (const Reading &r : readings) {
		line += r.value;
		line += ';';
	}
	if (line.endsWith(";")) line.remove(line.length() - 1);
	Serial.println(line);
	return true;
}
#endif // FEATURE_SENDER_CSV
