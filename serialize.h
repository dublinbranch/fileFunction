#pragma once
#include "filefunction.h"
#include <QDataStream>
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

template <typename T>
uint fileUnSerialize(QString fileName, T& t) {

	QFileXT file;
	file.setFileName(fileName);
	if (!file.open(QIODevice::ReadOnly, true)) {
		return 0;
	}

	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_DefaultCompiledVersion);

	in >> t;
	return file.size();
}
