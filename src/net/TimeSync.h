#pragma once
// NTP time synchronization. Replaces setupNetworkTime() of the original firmware.

namespace TimeSync {
// Fetch time via NTP (started non-blocking).
void begin();
// true as soon as a plausible time has been set.
bool synced();
} // namespace TimeSync
