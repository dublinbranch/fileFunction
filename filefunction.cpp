#include "filefunction.h"
#include <QDebug>
#include <QCryptographicHash>

QString QStacker() {
	return QString();
}
bool QFileXT::open(QIODevice::OpenMode flags) {
	return open(flags, false);
}

bool QFileXT::open(QIODevice::OpenMode flags, bool quiet) {
	if (!QFile::open(flags)) {
		if (!quiet) {
			qCritical().noquote() << errorString() << "opening" << fileName() << "\n"
			                      << QStacker();
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
	QFileXT file;
	file.setFileName(fileName);
	if (!file.open(QIODevice::ReadOnly, quiet)) {
		return QByteArray();
	}
	return file.readAll();
}

bool fileAppendContents(const QByteArray& pay, const QString& fileName) {
	QFileXT file;
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
