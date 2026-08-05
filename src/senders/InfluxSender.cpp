#include "../Features.h"
#if FEATURE_SENDER_INFLUX
#include "InfluxSender.h"
#include "HttpPost.h"
#include "../Board.h"
#include "../Config.h"
#include "../Debug.h"

bool InfluxSender::enabled() const { return cfg.send2influx; }

bool InfluxSender::isNumeric(const String &s)
{
	if (s.length() == 0) return false;
	char *end = nullptr;
	strtod(s.c_str(), &end);
	return end && *end == '\0';
}

bool InfluxSender::send(const std::vector<SensorReadings> &groups, const String & /*json*/)
{
	const std::vector<Reading> readings = flattenReadings(groups);

	// Line protocol: <measurement>,node=<node-id> field=val,field="str" ...
	String line = cfg.measurement_name_influx;
	line += ",node=";
	line += boardNodeId();
	line += ' ';

	bool first = true;
	for (const Reading &r : readings) {
		if (!first) line += ',';
		first = false;
		line += r.value_type;
		line += '=';
		if (isNumeric(r.value)) {
			line += r.value;
		} else {
			line += '"';
			line += r.value;
			line += '"';
		}
	}
	line += '\n';

	return httpPost(cfg.host_influx, cfg.port_influx, cfg.url_influx, cfg.ssl_influx,
					"text/plain; charset=utf-8", line,
					cfg.user_influx[0] ? cfg.user_influx : nullptr,
					cfg.pwd_influx);
}
#endif // FEATURE_SENDER_INFLUX
