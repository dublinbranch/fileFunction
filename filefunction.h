#pragma once

#include "folder.h"
#include <QFile>
#include <QSaveFile>
#include "ffCommon.h"

inline const QString FSDateTimeFormat = "yyyy-MM-dd_HH:mm:ss";

class QFileXT : public QFile {
      public:
	QFileXT() = default;
	QFileXT(const QString& file);
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
bool fileAppendContents(const std::string& pay, const QString& fileName);

QByteArray sha1(const QByteArray& original, bool urlSafe = true);
QByteArray sha1(const QString& original, bool urlSafe = true);
QString    sha1QS(const QString& original, bool urlSafe = true);

/**
 * @brief unzippaFile
 * @param folder need to ONLY have the file to be extracted
 * @return
 */
QStringList unzippaFile(const QString& folder);

/**
  The parameter line MUST be kept alive, so the QStringRef can point to something valid
*/
//Much slower but more flexible, is that ever used ?
std::vector<QStringRef> readCSVRowFlexySlow(const QString& line, const QStringList& separator = {","}, const QStringList& escape = {"\""});
//Quite fast expecially if optimizer is on
std::vector<QStringRef> readCSVRowRef(const QStringRef& line, const QChar& separator = ',', const QChar& escape = 0x0);
std::vector<QStringRef> readCSVRow(const QString& line, const QChar& separator = ',', const QChar& escape = 0x0);

QVector<QByteArray> csvExploder(QByteArray line, const char separator = 0);

void checkFileLock(QString path, uint minDelay = 5);

namespace std {
class thread;
}
std::thread* deleter(const QString& folder, uint day, uint ms = 1000, bool useThread = false);

bool softlink(const QString& source, const QString& dest, bool quiet = false);


//using namespace magic_enum::bitwise_operators;
// HLParam::quiet | HLParam::eraseOld
QString hardlink(const QString& source, const QString& dest, HLParam param = HLParam::eraseOld);
