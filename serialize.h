#pragma once
#include "filefunction.h"
#include <QDataStream>
#include <QDateTime>
#include <QFileInfo>

template <typename T>
uint fileSerialize(QString fileName, const T& t) {
	QFileXT file;
	file.setFileName(fileName);
	if (!file.open(QIODevice::QIODevice::Truncate | QIODevice::WriteOnly, false)) {
		return 0;
	}

	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_DefaultCompiledVersion);
	out << t;
	return file.size();
}

struct UnserializeResult {
	qint64    size       = 0;
	bool      fileExists = false;
	QDateTime age;
	bool      valid = false;
};
template <typename T>
UnserializeResult fileUnSerialize(QString fileName, T& t, uint maxAge = 0) {

	QFileXT file;
	file.setFileName(fileName);
	if (!file.open(QIODevice::ReadOnly, true)) {
		return {0, false, QDateTime()};
	}

	//AFAIK birtTime is a bit broken in linux http://moiseevigor.github.io/software/2015/01/30/get-file-creation-time-on-linux-with-ext4/
	auto lastEdit = QFileInfo(file).metadataChangeTime();
	//is the file fresh enought ?
	if (lastEdit.toSecsSinceEpoch() < QDateTime::currentSecsSinceEpoch() - maxAge) {
		return {0, true, QDateTime(), false};
	}

	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_DefaultCompiledVersion);

	in >> t;
	return {file.size(), true, lastEdit, true};
}
