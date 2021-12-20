#include "timezone.h"
#include <QTimeZone>

[[nodiscard]] QDateTime alterTz(const QDateTime& old, const QTimeZone& tz) {
       QDateTime neu;
       neu.setTimeZone(tz);
       neu.setMSecsSinceEpoch(old.toMSecsSinceEpoch());
       return neu;
}

[[nodiscard]] QDateTime setTz(const QDateTime& old, const QTimeZone& tz) {
       QDateTime neu = old;
       neu.setTimeZone(tz);
       return neu;
}

QDateTime hourlyFloor(const QDateTime& time) {
       return QDateTime::fromSecsSinceEpoch( (time.toSecsSinceEpoch() / 3600) * 3600);
}
