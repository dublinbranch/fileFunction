#ifndef QDATETIMEV2_H
#define QDATETIMEV2_H

#include <QDateTime>

class TimespecV2 {
      public:
	uint32_t sec;
	uint32_t ns;
	auto     operator<=>(const TimespecV2& t) const = default;
};

class TimevalV2 {
      public:
	uint32_t sec;
	uint16_t ms;
	auto     operator<=>(const TimevalV2& t) const = default;
};

class QDateTimeV2 : QDateTime {
      public:
	TimevalV2 static getMs();
};

#endif // QDATETIMEV2_H
