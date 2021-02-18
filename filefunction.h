#pragma once

#include <QFile>
#include <QSaveFile>

inline const QString FSDateTimeFormat = "yyyy-MM-dd_HH:mm:ss";

class QFileXT : public QFile {
      public:
	bool open(OpenMode flags) override;
	bool open(OpenMode flags, bool quiet);
};

class QSaveV2 : public QFile {
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

bool filePutContents(const QString& pay, const QString& fileName);
bool filePutContents(const QByteArray& pay, const QString& fileName);
bool filePutContents(const std::string& pay, const QString& fileName);

QByteArray fileGetContents(const QString& fileName, bool quiet = true);
QByteArray fileGetContents(const QString& fileName, bool quiet, bool& success);

FileGetRes fileGetContents2(const QString& fileName, bool quiet = true, uint maxAge = 0);

bool fileAppendContents(const QByteArray& pay, const QString& fileName);

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

/**
  The parameter line MUST be kept alive, so the QStringRef can point to something valid
*/
std::vector<QStringRef> readCSVRow(const QString& line, const QStringList& separator = {","}, const QStringList& escape = {"\""});
QVector<QByteArray>     csvExploder(QByteArray line, const char separator = 0);

void checkFileLock(QString path);

namespace std {
class thread;
}
std::thread* deleter(const QString& folder, uint day, uint ms = 1000, bool useThread = false);
