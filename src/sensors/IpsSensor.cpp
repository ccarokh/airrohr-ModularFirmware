#include "../Features.h"
#if FEATURE_SENSOR_IPS
#include "IpsSensor.h"
#include "../Debug.h"
#include "../Config.h"

namespace {
constexpr unsigned long WARMUPTIME_MS = 15000;
constexpr unsigned long READINGTIME_MS = 15000;
}

void IpsSensor::sendCmd(Cmd c)
{
	switch (c) {
	case Cmd::Start: _serial.print("$Wpsm=0\r\n"); break;
	case Cmd::Stop:  _serial.print("$Wpsm=1\r\n"); break;
	case Cmd::Get:   _serial.print("$Rget=\r\n"); break;
	}
}

void IpsSensor::resetAccumulators()
{
	for (int i = 0; i < N; ++i) {
		_pm_sum[i] = 0;   _pm_min[i] = 200.0f;      _pm_max[i] = 0;
		_pcs_sum[i] = 0;  _pcs_min[i] = 4000000000UL; _pcs_max[i] = 0;
	}
	_val_count = 0;
}

bool IpsSensor::begin()
{
	_serial.begin(9600, SENSOR_SERIAL_8N1, _rx, _tx);
	_serial.setTimeout(1000);
	resetAccumulators();
	return true;
}

void IpsSensor::update(unsigned long ms_since_start)
{
	const unsigned long interval = cfg.sending_intervall_ms;

	// Flush the input buffer.
	while (_serial.available() > 0) _serial.read();

	if (interval > (WARMUPTIME_MS + READINGTIME_MS) &&
		ms_since_start < (interval - (WARMUPTIME_MS + READINGTIME_MS))) {
		if (_running) { sendCmd(Cmd::Stop); _running = false; }
		return;
	}
	if (!_running) { sendCmd(Cmd::Start); _running = true; }

	if (ms_since_start <= (interval - READINGTIME_MS)) {
		return;
	}

	sendCmd(Cmd::Get);
	String d;
	if (_serial.available() > 0) {
		d = _serial.readString();
	}
	if (d.length() == 0) return;

	// Search for field markers in the CSV string (order as in the device).
	int iPC01 = d.indexOf("PC0.1,");
	int iPC03 = d.indexOf(",PC0.3,");
	int iPC05 = d.indexOf(",PC0.5,");
	int iPC1  = d.indexOf(",PC1.0,");
	int iPC25 = d.indexOf(",PC2.5,");
	int iPC5  = d.indexOf(",PC5.0,");
	int iPC10 = d.indexOf(",PC10,");
	int iPM01 = d.indexOf(",PM0.1,");
	int iPM03 = d.indexOf(",PM0.3,");
	int iPM05 = d.indexOf(",PM0.5,");
	int iPM1  = d.indexOf(",PM1.0,");
	int iPM25 = d.indexOf(",PM2.5,");
	int iPM5  = d.indexOf(",PM5.0,");
	int iPM10 = d.indexOf(",PM10,");
	int iEnd  = d.indexOf(",IPS");
	if (iPC01 < 0 || iEnd < 0 || iPM10 < 0) return; // incomplete

	String Ncnt[N] = {
		d.substring(iPC01 + 6, iPC03),
		d.substring(iPC03 + 7, iPC05),
		d.substring(iPC05 + 7, iPC1),
		d.substring(iPC1 + 7, iPC25),
		d.substring(iPC25 + 7, iPC5),
		d.substring(iPC5 + 7, iPC10),
		d.substring(iPC10 + 6, iPM01),
	};
	String Nmass[N] = {
		d.substring(iPM01 + 7, iPM03 - 6),
		d.substring(iPM03 + 7, iPM05 - 6),
		d.substring(iPM05 + 7, iPM1 - 6),
		d.substring(iPM1 + 7, iPM25 - 6),
		d.substring(iPM25 + 7, iPM5 - 6),
		d.substring(iPM5 + 7, iPM10 - 6),
		d.substring(iPM10 + 6, iEnd - 6),
	};

	for (int i = 0; i < N; ++i) {
		float mass = Nmass[i].toFloat();
		unsigned long pcs = strtoul(Ncnt[i].c_str(), nullptr, 10);
		_pm_sum[i] += mass;
		_pcs_sum[i] += pcs;
		if (mass < _pm_min[i]) _pm_min[i] = mass;
		if (mass > _pm_max[i]) _pm_max[i] = mass;
		if (pcs < _pcs_min[i]) _pcs_min[i] = pcs;
		if (pcs > _pcs_max[i]) _pcs_max[i] = pcs;
	}
	_val_count++;
}

void IpsSensor::collect(std::vector<Reading> &out)
{
	float pm[N];
	double pcs[N];
	int count = _val_count;
	for (int i = 0; i < N; ++i) { pm[i] = _pm_sum[i]; pcs[i] = (double)_pcs_sum[i]; }
	if (count > 2) {
		for (int i = 0; i < N; ++i) {
			pm[i]  -= _pm_min[i] + _pm_max[i];
			pcs[i] -= (double)_pcs_min[i] + (double)_pcs_max[i];
		}
		count -= 2;
	}

	if (count > 0) {
		// value_type mapping as in the original (indices: 0=0.1,1=0.3,2=0.5,3=1.0,4=2.5,5=5.0,6=10)
		out.emplace_back(F("IPS_P0"),  pm[3] / count, 2); // PM1
		out.emplace_back(F("IPS_P1"),  pm[6] / count, 2); // PM10
		out.emplace_back(F("IPS_P2"),  pm[4] / count, 2); // PM2.5
		out.emplace_back(F("IPS_P01"), pm[0] / count, 2);
		out.emplace_back(F("IPS_P03"), pm[1] / count, 2);
		out.emplace_back(F("IPS_P05"), pm[2] / count, 2);
		out.emplace_back(F("IPS_P5"),  pm[5] / count, 2);

		out.emplace_back(F("IPS_N1"),  (float)(pcs[3] / (count * 1000.0)), 2);
		out.emplace_back(F("IPS_N10"), (float)(pcs[6] / (count * 1000.0)), 2);
		out.emplace_back(F("IPS_N25"), (float)(pcs[4] / (count * 1000.0)), 2);
		out.emplace_back(F("IPS_N01"), (float)(pcs[0] / (count * 1000.0)), 2);
		out.emplace_back(F("IPS_N03"), (float)(pcs[1] / (count * 1000.0)), 2);
		out.emplace_back(F("IPS_N05"), (float)(pcs[2] / (count * 1000.0)), 2);
		out.emplace_back(F("IPS_N5"),  (float)(pcs[5] / (count * 1000.0)), 2);
	} else {
		LOG_ERROR(F("IPS-7100: keine gültigen Werte im Zyklus"));
	}
	resetAccumulators();

	if (cfg.sending_intervall_ms > (WARMUPTIME_MS + READINGTIME_MS) && _running) {
		sendCmd(Cmd::Stop);
		_running = false;
	}
}
#endif // FEATURE_SENSOR_IPS
