#ifndef FILEFUNCTION_H
#define FILEFUNCTION_H
#include <QFile>

class QFileXT : public QFile {
      public:
	bool open(OpenMode flags) override;
	bool open(OpenMode flags, bool quiet);
};
bool       filePutContents(const QByteArray& pay, const QString& fileName);
QByteArray fileGetContents(const QString& fileName, bool quiet = true);
bool       fileAppendContents(const QByteArray& pay, const QString& fileName);

QByteArray sha1(const QByteArray& original, bool urlSafe = true);
QByteArray sha1(const QString& original, bool urlSafe = true);
#endif // FILEFUNCTION_H
