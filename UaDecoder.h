#ifndef UADECODER_H
#define UADECODER_H

#include <QString>
/*
 Use like that
 
 //init the semaphore in case you need concurrency control
 std::counting_semaphore<16> uaSem(0);
 
 ....
 
	UaDecoder ua;
	if (uaSem.try_acquire()) {
		ua.decode(dk.userAgent, conf().uaDecoder.path);
		uaSem.release();
	}
	
*/ 
class UaDecoder {
      public:
	bool decode(const QString& userAgent, const QString& decoderUrl);

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
