#ifndef QDATETIMEV2_H
#define QDATETIMEV2_H

#include <QDateTime>

struct timespec;
class TimespecV2 {
      public:
	//This can easily handle microsecond precision data 53bit / 15.95digit, a unix ts is 10 digit
	double time;

	double sec() const;
	//This is what you want 99.9% of the time
	double fractional() const;
	
	//no idea why anyone will use those but let's leave them
	uint   ms() const;
	uint   ns() const;
	

	auto operator<=>(const TimespecV2& t) const = default;

	TimespecV2() = default;
	TimespecV2(double ts);
	TimespecV2        operator-(const TimespecV2& rhs);
	TimespecV2        operator-(const double& rhs);
	timespec          toTimespec() const;
	static TimespecV2 now();
	void              setNow();
	double            toDouble() const;

	operator double() const;
};

#endif // QDATETIMEV2_H
