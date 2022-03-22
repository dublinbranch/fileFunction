#include "qdatetimev2.h"

TimevalV2 QDateTimeV2::getMs() {
	timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	TimevalV2 v2;
	v2.sec = tp.tv_sec;
	v2.ms  = tp.tv_nsec / 1E6;
	return v2;
}
