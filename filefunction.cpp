#include "filefunction.h"
#include "QStacker/qstacker.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <boost/tokenizer.hpp>
#include <mutex>
#include <sys/file.h>
#include <thread>

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
	using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;
	boost::escaped_list_separator<char> sep('\\', ',', '\"');
	if (separator) {
		sep = boost::escaped_list_separator<char>('\\', separator, '\"');
	}
	QVector<QByteArray>      final;
	std::vector<std::string> vec2;
	auto                     cry = line.toStdString();
	try {
		cxaLevel = CxaLevel::none;
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

std::vector<QStringRef> readCSVRow(const QString& line, const QStringList& separator, const QStringList& escape) {
	std::vector<QStringRef> part;
	//Quando si ESCE dal quote, non avanzare di pos, altrimenti diventa "ciao" -> ciao"
	//questo innesca il problema che se vi è ad esempio ciao,"miao""bau",altro
	//invece di avere ciao, miaobau, altro non funge perché il range NON é CONTIGUO -.-, ma viene ritornato ad esempio ciao, miao, bau, altro

	// newState = delta[currentState][event]

	// rows are states

	// columns are events:
	// 0 = read separator
	// 1 = read escape
	// 2 = read new line
	// 3 = read "normal" character
	// 4 = reached the end of line (eof if line is a file content)
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
	part.clear(); //clear del contenitore dei pezzi in cui scomporre la riga
	if (line.isEmpty()) {
		return part;
	}

	int actualState = 0;
	int pos         = 0;

	// initial invalid value
	int                  event             = -1;
	int                  currentBlockStart = -1;
	int                  blockEnd          = 0;
	static const QString newline           = "\n";
	static const QString empty;

	while (actualState >= 0) {
		if (pos >= line.length())
			event = 4;
		else {
			QStringRef ch = line.midRef(pos, 1);
			pos++;
			if (separator.contains(ch, Qt::CaseInsensitive))
				event = 0;
			else if (escape.contains(ch, Qt::CaseInsensitive)) {
				event = 1;
			} else if (ch == newline)
				event = 2;
			else
				event = 3;
		}

		actualState = delta[actualState][event];

		switch (actualState) {
		case 4:
			blockEnd = pos - 1;
			break;
		case 0:
		case 3:
			if (currentBlockStart == -1) {
				currentBlockStart = pos - 1;
			}

			break;
		case -1:
		case 1:

			if (!blockEnd) {
				blockEnd = pos - 1;
			}
			if (currentBlockStart == -1) { //a new block has never started, we have two separator in a row
				part.push_back(empty.midRef(0, 0));
			} else {
				QStringRef v = line.midRef(currentBlockStart, (blockEnd - currentBlockStart));
				blockEnd     = 0;
				part.push_back(v);
				currentBlockStart = -1;
				//curentBlock.clear();
			}
			break;
		}
	}
	if (actualState == -2)
		throw std::runtime_error("End-of-file found while inside quotes.");

	return part;
}

using namespace std::literals;
void checkFileLock(QString path) {
	//check if there is another instance running...

	int fd = open(path.toUtf8().data(), O_CREAT | O_RDWR, 0666);
	if (fd == -1) {
		qWarning() << path << "error opening";
		exit(1);
	}

	if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
		auto msg = path.toStdString() + " is already locked, I refuse to start.\n (The application is already running.) "s;
		std::puts(msg.c_str());
		exit(1);
	}
}

bool filePutContents(const QString& pay, const QString& fileName) {
	return filePutContents(pay.toUtf8(), fileName);
}
/**
 * @brief deleter
 * @param folder
 * @param filter
 * @param day
 */
std::thread* deleter(const QString& folder, uint day, uint ms, bool useThread) {
	//wrap in a lambda, copy parameter to avoid they go out of scope
	auto task = [=]() {
		QProcess process;

		process.start("find", {folder, "-mtime", QString::number(day), "-delete"});
		auto finished = process.waitForFinished(ms);

		//auto exitStatus = process.exitStatus();
		auto error = process.error();
		//auto state      = process.state();
		if (error != QProcess::UnknownError) {
			qCritical().noquote() << "error launching find process" << error << QStacker16Light();
			process.kill();
			process.waitForFinished(10); //quick wait only to dispatch the kill signal
			return;
		}

		QByteArray errorMsg = process.readAllStandardError();
		if (!errorMsg.isEmpty()) {
			qWarning().noquote() << QSL("Error deleting old files in folder %1  error: %2 msg:").arg(folder).arg(error) + errorMsg + QStacker16Light();
			return;
		}

		if (!finished) {
			qDebug() << "Still deleting for " << folder << " after" << ms;
			process.kill();
			process.waitForFinished(10); //quick wait only to dispatch the kill signal
		}
	};
	if (useThread) {
		return new std::thread(task);
	}

	task();
	return nullptr;
}
