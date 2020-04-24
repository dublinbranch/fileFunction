#include "filefunction.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <mutex>
#include "QStacker/qstacker.h"

bool QFileXT::open(QIODevice::OpenMode flags) {
	return open(flags, false);
}

bool QFileXT::open(QIODevice::OpenMode flags, bool quiet) {
	if (!QFile::open(flags)) {
		if (!quiet) {
			qCritical().noquote() << errorString() << "opening" << fileName() << "\n"
			                      << QStacker16();
		}
		return false;
	}
	return true;
}

bool filePutContents(const QByteArray& pay, const QString& fileName) {
	QFileXT file;
	file.setFileName(fileName);
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
		return false;
	}
	file.write(pay);
	file.close();
	return true;
}

QByteArray fileGetContents(const QString& fileName, bool quiet) {
	if (fileName.isEmpty()) {
		return QByteArray();
	}
	QFileXT file;
	file.setFileName(fileName);
	if (!file.open(QIODevice::ReadOnly, quiet)) {
		return QByteArray();
	}
	return file.readAll();
}

bool fileAppendContents(const QByteArray& pay, const QString& fileName) {
	static std::mutex           mutex;
	std::lock_guard<std::mutex> lock(mutex);
	QFileXT                     file;
	file.setFileName(fileName);
	if (!file.open(QIODevice::Append | QIODevice::WriteOnly)) {
		return false;
	}
	file.write(pay);
	file.write("\n");
	file.close();
	return true;
}

QByteArray sha1(const QByteArray& original, bool urlSafe) {
	auto sha1 = QCryptographicHash::hash(original, QCryptographicHash::Algorithm::Sha1);
	if (urlSafe) {
		return sha1.toBase64(QByteArray::Base64Option::Base64UrlEncoding | QByteArray::Base64Option::OmitTrailingEquals);
	}
	return sha1;
}
QByteArray sha1(const QString& original, bool urlSafe) {
	return sha1(original.toUtf8(), urlSafe);
}

void mkdir(const QString& dirName) {
	static std::mutex            lock;
	std::scoped_lock<std::mutex> scoped(lock);
	QDir                         dir = QDir(dirName);
	if (!dir.mkpath(".")) {
		qCritical().noquote() << "impossible to create working dir" << dirName << "\n"
		                                                                "maybe swapTronic is running without the necessary privileges";
		exit(1);
	}
}

void cleanFolder(const QString& folder) {
	auto dir = QDir(folder);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	auto files = dir.entryList();
	for (auto file : files) {
		dir.remove(file);
	}
}

QStringList unzippaFile(const QString& folder) {
	auto processedFolder = folder + "/processed";
	auto extractedFolder = folder + "/extracted";
	mkdir(processedFolder);
	mkdir(extractedFolder);

	//verify there are only zip file in this folder
	auto dir   = QDir(folder);
	auto files = dir.entryList(QStringList("*"), QDir::Files | QDir::NoDotAndDotDot);
	if (files.size() > 1) {
		throw QString("the folder %1 has more than 1 file!").arg(folder);
	}

	const QString program = "unzip";
	//const QStringList arguments = QStringList() << file;
	QProcess process;
	process.setWorkingDirectory(folder);
	process.start(program, files);
	process.waitForFinished(10000); //10 sec

	//move away the zip
	auto old = folder + "/" + files.at(0);
	auto neu = processedFolder + "/" + files.at(0);
	if (QFile::exists(neu)) {
		QFile::remove(neu);
	}

	if (!QFile::rename(old, neu)) {
		qCritical().noquote() << "impossible spostare";
	}

	//rescan directory for extracted file
	files = dir.entryList(QStringList("*"), QDir::Files | QDir::NoDotAndDotDot);
	for (auto&& file : files) {
		//move in extracted and update path
		auto old = folder + "/" + file;
		auto neu = file = extractedFolder + "/" + file;
		if (QFile::exists(neu)) {
			QFile::remove(neu);
		}
		QFile::rename(old, neu);
	}
	return files;
}

QString getMostRecent(const QString pathDir, const QString& filter) {
	auto dir = QDir(pathDir);
	if (!filter.isEmpty()) {
		QStringList filters;
		filters << filter;
		dir.setNameFilters(filters);
	}

	dir.setSorting(QDir::Time);
	auto files = dir.entryList();
	if (!files.isEmpty()) {
		return pathDir + "/" + files.at(0);
	}
	return QString();
}

QString sha1QS(const QString& original, bool urlSafe) {
	return sha1(original, urlSafe);
}
