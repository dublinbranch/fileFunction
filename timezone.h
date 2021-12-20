#ifndef TIMEZONE_H
#define TIMEZONE_H
#include <QTimeZone>

const QTimeZone UTC = QTimeZone("UTC");
//this should not be here!
const QDateTime unixMidnight = QDateTime::fromSecsSinceEpoch(0, Qt::UTC);

//This will keep the timestamp
[[nodiscard]] QDateTime alterTz(const QDateTime& old, const QTimeZone& tz);
//This will alter the timestamp!
[[nodiscard]] QDateTime setTz(const QDateTime& old, const QTimeZone& tz);
//Floor to the begin of the hour
[[nodiscard]] QDateTime hourlyFloor(QDateTime time);

#endif // TIMEZONE_H
