#include "folder.h"
#include <mutex>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

bool mkdir(const QString& dirName) {
	static std::mutex            lock;
	std::scoped_lock<std::mutex> scoped(lock);
	QDir                         dir = QDir(dirName);
	if (!dir.mkpath(".")) {
		qWarning().noquote() << "impossible to create working dir" << dirName << "\n"
		                     << "maybe" << QCoreApplication::applicationName() << "is running without the necessary privileges";
		return false;
	}
	return true;
}

void cleanFolder(const QString& folder) {
	auto dir = QDir(folder);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	auto files = dir.entryList();
	for (auto file : files) {
		dir.remove(file);
	}
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
