#pragma once
#include "../Features.h"
#if FEATURE_SENSOR_IPS
// Piera Systems IPS-7100 particulate matter sensor (UART, ASCII CSV protocol).
// Port of fetchSensorIPS() + IPS_cmd(). Provides 7 mass values (IPS_P*) and
// 7 count values (IPS_N*).

#include "Sensor.h"
#include "../Board.h"

class IpsSensor : public ISensor {
public:
	IpsSensor(SensorSerial &serial, int8_t rx, int8_t tx)
		: _serial(serial), _rx(rx), _tx(tx) {}

	const char *name() const override { return "IPS-7100"; }
	int apiPin() const override { return 1; }
	bool begin() override;
	void update(unsigned long ms_since_start) override;
	void collect(std::vector<Reading> &out) override;

private:
	enum class Cmd { Start, Stop, Get };
	void sendCmd(Cmd c);
	void resetAccumulators();

	SensorSerial &_serial;
	int8_t _rx, _tx;
	bool   _running = false;

	// 7 size classes: 0.1, 0.3, 0.5, 1.0, 2.5, 5.0, 10
	static constexpr int N = 7;
	float         _pm_sum[N]  = {0};
	float         _pm_min[N];
	float         _pm_max[N];
	unsigned long _pcs_sum[N] = {0};
	unsigned long _pcs_min[N];
	unsigned long _pcs_max[N];
	int           _val_count = 0;
};

#endif // FEATURE_SENSOR_IPS