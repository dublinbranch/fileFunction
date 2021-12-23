#include "UaDecoder.h"
#include "JSONReader/jsonreader.h"
#include "fileFunction/filefunction.h"
#include "mapExtensor/rwguard.h"
#include "minCurl/mincurl.h"
#include "minMysql/const.h"
#include <QDateTime>
#include <shared_mutex>

static std::shared_mutex         lock;
static mapV2<QString, UaDecoder> cache;

bool UaDecoder::decode(const QString& userAgent, const QString& decoderUrl) {
	RWGuard scoped(&lock);
	scoped.lockShared();
	if (auto v = cache.get(userAgent); v) {
		*this = *v.val;
		return true;
	}
	scoped.unlock();

	auto       url = decoderUrl + "?ua=" + userAgent.toUtf8().toPercentEncoding();
	CurlKeeper curl;
	//should be on same machine and heavily used, normal time is around 1ms
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 10);

	auto res = urlGetContent2(url, true);
	if (res.httpCode != 200) {
		if (res.httpCode != 200) {
			filePutContents(QSL("\n------\n %1 error %2 for decoding ua %3 ")
			                        .arg(QDateTime::currentDateTime().toString(mysqlDateTimeFormat), res.result.data()) +
			                    userAgent,
			                "log/uaDecoder.log");
		}
		return false;
	}

	JSONReader reader;
	if (!reader.parse(res.result)) {
		static auto refreshAfter = QDateTime::currentSecsSinceEpoch() + 120;
		if (refreshAfter < QDateTime::currentSecsSinceEpoch()) {
			qCritical() << "matomo json decoder read error while decoding " << userAgent << QStacker16Light();
			return false;
		}
	}
	reader.getta("/enabled", ok);
	if (!ok) {
		static auto refreshAfter = QDateTime::currentSecsSinceEpoch() + 120;
		if (refreshAfter < QDateTime::currentSecsSinceEpoch() + 120) {
			qCritical() << "json decoder not enabled" << QStacker16Light();
			return false;
		}
	}

	reader.getta("/osName", osName);
	reader.getta("/osVersion", osVersion);
	reader.getta("/browserVersion", browserVersion);
	reader.getta("/browserName", browserName);
	reader.getta("/device", device);
	reader.getta("/brand", brand);
	reader.getta("/bot", bot);

	scoped.lock();
	cache.insert({userAgent, *this});
	scoped.unlock();
	return true;
}
