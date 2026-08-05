#include "TimeSync.h"
#include "../Debug.h"

#include <time.h>

namespace TimeSync {

void begin()
{
	// UTC; NTP pools as in the original firmware.
	configTime(0, 0, "0.pool.ntp.org", "1.pool.ntp.org", "2.pool.ntp.org");
	LOG_INFO(F("NTP: Zeitabgleich gestartet"));
}

bool synced()
{
	time_t now = time(nullptr);
	// Plausible if after 2021-01-01.
	return now > 1609459200;
}

} // namespace TimeSync
