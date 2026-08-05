#include "Correction.h"

float readCorrectionOffset(const char *correction)
{
	char *pEnd = nullptr;
	// No atof() (saves ~9 kB of code size).
	float r = float(strtol(correction, &pEnd, 10));
	if (pEnd && pEnd[0] == '.' && pEnd[1] >= '0' && pEnd[1] <= '9') {
		bool isNegative = correction[0] == '-';
		for (int i = 0; correction[i] == ' '; i++) {
			isNegative = correction[i + 1] == '-';
		}
		r += (isNegative ? -1.0f : 1.0f) * ((pEnd[1] - '0') / 10.0f);
	}
	return r;
}
