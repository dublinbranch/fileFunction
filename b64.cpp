#include "b64.h"
#include "stringDefine.h"

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

QString fromBase64(const QString& url64, bool urlSafe) {
	auto b = QByteArray::Base64Option::Base64UrlEncoding | QByteArray::Base64Option::OmitTrailingEquals;
	if (urlSafe) {
		return QByteArray::fromBase64(url64.toUtf8(), b);
	}
	return QByteArray::fromBase64(url64.toUtf8());
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
