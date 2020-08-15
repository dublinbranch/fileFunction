#include "filefunction.h"
#include "QStacker/qstacker.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <boost/tokenizer.hpp>
#include <mutex>
#include <sys/file.h>

#define QBL(str) QByteArrayLiteral(str)
#define QSL(str) QStringLiteral(str)

bool QFileXT::open(QIODevice::OpenMode flags) {
	return open(flags, false);
}

bool QFileXT::open(QIODevice::OpenMode flags, bool quiet) {
	if (!QFile::open(flags)) {
		if (!quiet) {
			qWarning().noquote() << errorString() << "opening" << fileName() << "\n"
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
	auto written = file.write(pay);
	if (written != pay.size()) {
		return false;
	}
	file.close();
	return true;
}

QByteArray fileGetContents(const QString& fileName, bool quiet) {
	bool ok;
	auto x = fileGetContents(fileName, quiet, ok);
	return x;
}

FileGetRes fileGetContents2(const QString& fileName, bool quiet) {
	bool ok;

	FileGetRes res;
	res.content = fileGetContents(fileName, quiet, ok);
	res.exist   = ok;

	return res;
}

QByteArray fileGetContents(const QString& fileName, bool quiet, bool& success) {
	if (fileName.isEmpty()) {
		return QByteArray();
	}
	QFileXT file;
	file.setFileName(fileName);
	if (!file.open(QIODevice::ReadOnly, quiet)) {
		return QByteArray();
	}
	success = true;
	auto x  = file.readAll();
	return x;
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

bool mkdir(const QString& dirName) {
	static std::mutex            lock;
	std::scoped_lock<std::mutex> scoped(lock);
	QDir                         dir = QDir(dirName);
	if (!dir.mkpath(".")) {
		qWarning().noquote() << "impossible to create working dir" << dirName << "\n"
		                                                                         "maybe swapTronic is running without the necessary privileges";
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
		qWarning().noquote() << "impossible spostare";
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

QVector<QByteArray> csvExploder(QByteArray line, const char separator) {
	//The csv we receive is trash sometimes
	line.replace(QBL("\r"), QByteArray());
	line.replace(QBL("\n"), QByteArray());
	//Not only we can receive a new line symbol, we can receive the two character \r or \n o.O
	line.replace(QBL("\\r"), QByteArray());
	line.replace(QBL("\\n"), QByteArray());

	//https://www.boost.org/doc/libs/1_71_0/libs/tokenizer/doc/char_separator.htm
	typedef boost::tokenizer<boost::escaped_list_separator<char>> Tokenizer;
	boost::escaped_list_separator<char>                           sep('\\', ',', '\"');
	if (separator) {
		sep = boost::escaped_list_separator<char>('\\', separator, '\"');
	}
	QVector<QByteArray>      final;
	std::vector<std::string> vec2;
	auto                     cry = line.toStdString();
	try {
		Tokenizer tok(cry, sep);
		vec2.assign(tok.begin(), tok.end());
	} catch (...) {
		qWarning().noquote() << "error decoding csv line " << line;
	}
	//Cry
	for (auto&& l : vec2) {
		final.append(QByteArray::fromStdString(l));
	}
	return final;
}

bool readCSVRow(QTextStream& line, QList<QString>& part, const QString separator) {

	static const int delta[][5] = {
	    //  ,    "   \n    ?  eof
	    {1, 2, -1, 0, -1}, // 0: parsing (store char)
	    {1, 2, -1, 0, -1}, // 1: parsing (store column)
	    {3, 4, 3, 3, -2},  // 2: quote entered (no-op)
	    {3, 4, 3, 3, -2},  // 3: parsing inside quotes (store char)
	    {1, 3, -1, 0, -1}, // 4: quote exited (no-op)
	                       // -1: end of row, store column, success
	                       // -2: eof inside quotes
	};
	part.clear();
	if (line.atEnd()) {
		return false;
	}

	int     state = 0, t;
	QString cell;
	QChar   ch;

	while (state >= 0) {

		if (line.atEnd())
			t = 4;
		else {
			line >> ch;
			if (ch == separator)
				t = 0;
			else if (ch == QSL("\""))
				t = 1;
			else if (ch == QSL("\n"))
				t = 2;
			else
				t = 3;
		}

		state = delta[state][t];

		switch (state) {
		case 0:
		case 3:
			cell += ch;
			break;
		case -1:
		case 1:
			part.append(cell);
			cell.clear();
			break;
		}
	}
	if (state == -2)
		throw std::runtime_error("End-of-file found while inside quotes.");

	return true;
}

void checkFileLock(QString path) {
	//check if there is another instance running...

	int fd = open(path.toUtf8().data(), O_CREAT | O_RDWR, 0666);
	if (fd == -1) {
		qWarning() << path << "error opening";
		exit(1);
	}

	if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
		qInfo().noquote() << path << "is already locked, I refuse to start.\n"
		                                "(The application is already running.)";
		exit(1);
	}
}

