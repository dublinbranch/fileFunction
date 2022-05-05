#include "b64.h"
#include "stringDefine.h"
#include <QRegExp>

QString base64this(const char* param) {
	// no alloc o.O
	QByteArray cheap;
	cheap.setRawData(param, strlen(param));
	return base64this(cheap);
}

QString toBase64(const QString& url, bool urlSafe) {
	auto b = QByteArray::Base64Option::Base64UrlEncoding | QByteArray::Base64Option::OmitTrailingEquals;
	if (urlSafe) {
		return url.toUtf8().toBase64(b);
	}
	return url.toUtf8().toBase64();
}

QByteArray fromBase64(const QByteArray& url64, bool urlSafe) {
	auto b = QByteArray::Base64Option::Base64UrlEncoding;
	if (urlSafe) {
		return QByteArray::fromBase64(url64, b);
	}
	return QByteArray::fromBase64(url64);
}

QString fromBase64(const QString& url64, bool urlSafe) {
	return fromBase64(url64.toUtf8(), urlSafe);
}

// https://stackoverflow.com/questions/12094280/qt-decode-binary-sequence-from-base64
//bool isB64Valid(QString input, bool checkLength) {
//	if (checkLength and (input.length() % 4 != 0))
//		return false;

//	auto found1 = QRegExp("^[A-Za-z0-9+/]+$").indexIn(input, QRegExp::CaretAtZero);
//	auto found2 = QRegExp("^[A-Za-z0-9+/]+=$").indexIn(input, QRegExp::CaretAtZero);
//	auto found3 = QRegExp("^[A-Za-z0-9+/]+==$").indexIn(input, QRegExp::CaretAtZero);

//	auto cond1 = found1 == -1;
//	auto cond2 = found2 == -1;
//	auto cond3 = found3 == -1;

//	if (cond1 && cond2 && cond3)
//		return false;
//	return true;
//}

bool isB64ValidV2(const QString& input) {
	auto decoded = QByteArray::fromBase64Encoding(input.toUtf8(), QByteArray::Base64Option::AbortOnBase64DecodingErrors);
	auto ok      = decoded.decodingStatus == QByteArray::Base64DecodingStatus::Ok;
	return ok;
}

QString base64this(const QByteArray& param) {
	return QBL("FROM_BASE64('") + param.toBase64() + QBL("')");
}

QString base64this(const QString& param) {
	auto a = param.toUtf8().toBase64();
	return QBL("FROM_BASE64('") + a + QBL("')");
}

QString base64this(const std::string_view& param) {
	QByteArray cheap;
	cheap.setRawData(param.data(), param.size());
	return base64this(cheap);
}

QString base64this(const std::string& param) {
	return base64this(std::string_view(param));
}

QString mayBeBase64(const QString& original, bool emptyAsNull) {
	if (original == SQL_NULL) {
		return original;
	} else if (original.isEmpty()) {
		if (emptyAsNull) {
			return SQL_NULL;
		}
		return QSL("''");
	} else {
		return base64this(original);
	}
}

QString base64Nullable(const QString& param, bool emptyAsNull) {
	return mayBeBase64(param, emptyAsNull);
}
QString base64Nullable4Where(const QString& param, bool emptyAsNull) {
	auto val = mayBeBase64(param, emptyAsNull);
	if (val == SQL_NULL) {
		return " IS NULL ";
	}
	return " = " + val;
}
