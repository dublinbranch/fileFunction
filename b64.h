#ifndef B64_H
#define B64_H

#include <QString>

QString base64this(const char* param);
QString base64this(const QByteArray& param);
QString base64this(const QString& param);
QString base64this(const std::string& param);
QString base64this(const std::string_view& param);
QString mayBeBase64(const QString& original, bool emptyAsNull = false);
QString base64Nullable(const QString& param, bool emptyAsNull = false);
QString base64Nullable(const QString* param, bool emptyAsNull = false);
QString base64Nullable4Where(const QString& param, bool emptyAsNull = false);

#endif // B64_H