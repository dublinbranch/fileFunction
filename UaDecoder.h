#ifndef UADECODER_H
#define UADECODER_H

#include <QString>

class UaDecoder {
      public:
	UaDecoder(const QString &userAgent, const QString& decoderUrl);

	bool    ok = false;
	QString osName;
	QString osVersion;
	QString browserVersion;
	QString browserName;
	QString device;
	QString brand;
	QString bot;
	QString enabled;
};

#endif // UADECODER_H
