#ifndef QDATETIMEUTIL_H
#define QDATETIMEUTIL_H

#include <QDateTime>
#include <QTimeZone>

//--------------------------------------------------------------------------------------
const QTimeZone UTC = QTimeZone("UTC");
//this should not be here!
const QDateTime unixMidnight = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

//--------------------------------------------------------------------------------------
//This will keep the timestamp
[[nodiscard]] [[deprecated("use QdateTime.toTimeZone")]] QDateTime alterTz(const QDateTime& old, const QTimeZone& tz);

//This will alter the timestamp!
[[nodiscard]] QDateTime setTz(const QDateTime& old, const QTimeZone& tz);

//Floor to the begin of the hour
[[nodiscard]] QDateTime hourlyFloor(QDateTime time);

//--------------------------------------------------------------------------------------
//small fuction for computin the seconds till the midnight if the current day
qint64 getSecondsUntilMidnight(const QTimeZone& time_zone);

class QDateTime2 : public QDateTime {
      public:
	QDateTime2(const QDateTime& val)
	    : QDateTime(val){};
	QDateTime2() = default;
	QDateTime getNextMidnight() const;
	qint64    secToNextMidnight() const;
};

//--------------------------------------------------------------------------------------
//Wrrapper class around unix timestamp.

struct timespec;
class TimespecV2 {
      public:
	//This can easily handle microsecond precision data 53bit / 15.95digit, a unix ts is 10 digit
	double time;

	double sec() const;
	//This is what you want 99.9% of the time
	double fractional() const;

	//no idea why anyone will use those but let's leave them
	uint ms() const;
	uint ns() const;

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

#endif // QDATETIMEUTIL_H
