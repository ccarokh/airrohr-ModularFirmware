#include "../Features.h"
#if FEATURE_SENSOR_DNMS
#include "DnmsSensor.h"
#include "../Debug.h"
#include "../Config.h"
#include "../util/Correction.h"

#include <dnms_i2c.h>

bool DnmsSensor::begin()
{
	char version[DNMS_MAX_VERSION_LEN + 1];
	dnms_reset();
	delay(1000);
	if (dnms_read_version(version) != 0) {
		LOG_ERROR(F("DNMS nicht gefunden (Verkabelung prüfen)"));
		_ok = false;
		return false;
	}
	_ok = true;
	return true;
}

void DnmsSensor::collect(std::vector<Reading> &out)
{
	bool error = false;

	if (dnms_calculate_leq() != 0) {
		error = true;
	}

	uint16_t data_ready = 0;
	error = true;
	for (unsigned i = 0; i < 20; i++) {
		delay(2);
		if (dnms_read_data_ready(&data_ready) == 0 && data_ready != 0) {
			error = false;
			break;
		}
	}

	struct dnms_measurements m;
	if (!error && dnms_read_leq(&m) == 0) {
		const float corr = readCorrectionOffset(cfg.dnms_correction);
		out.emplace_back(F("DNMS_noise_LAeq"), m.leq_a + corr, 2);
		out.emplace_back(F("DNMS_noise_LA_min"), m.leq_a_min + corr, 2);
		out.emplace_back(F("DNMS_noise_LA_max"), m.leq_a_max + corr, 2);
	} else {
		dnms_reset();
		LOG_ERROR(F("DNMS: Lesefehler"));
	}
}
#endif // FEATURE_SENSOR_DNMS
