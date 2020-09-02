#pragma once

#include <QFile>

class QFileXT : public QFile {
      public:
	bool open(OpenMode flags) override;
	bool open(OpenMode flags, bool quiet);
};

struct FileGetRes {
	operator bool() {
		return exist;
	}
	QByteArray content;
	bool       exist = false;
};

bool       filePutContents(const QString& pay, const QString& fileName);
bool       filePutContents(const QByteArray& pay, const QString& fileName);
QByteArray fileGetContents(const QString& fileName, bool quiet = true);
FileGetRes fileGetContents2(const QString& fileName, bool quiet = true);
QByteArray fileGetContents(const QString& fileName, bool quiet, bool& success);
bool       fileAppendContents(const QByteArray& pay, const QString& fileName);

QByteArray sha1(const QByteArray& original, bool urlSafe = true);
QByteArray sha1(const QString& original, bool urlSafe = true);
QString    sha1QS(const QString& original, bool urlSafe = true);

bool mkdir(const QString& dirName);
void cleanFolder(const QString& folder);

/**
 * @brief unzippaFile
 * @param folder need to ONLY have the file to be extracted
 * @return
 */
QStringList unzippaFile(const QString& folder);

QString getMostRecent(const QString pathDir, const QString& filter);

class QTextStream;
bool readCSVRow(QTextStream& line, QList<QString>& part, const QString separator = ",");
QVector<QByteArray> csvExploder(QByteArray line, const char separator = 0);

void checkFileLock(QString path);
