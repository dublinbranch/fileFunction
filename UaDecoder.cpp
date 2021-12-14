#include "UaDecoder.h"
#include "JSONReader/jsonreader.h"
#include "minCurl/mincurl.h"
#include <QDateTime>

bool UaDecoder::decode(const QString& userAgent, const QString& decoderUrl) {
	auto       url = decoderUrl + "?ua=" + userAgent;
	CurlKeeper curl;
	//should be on same machine and heavily used, normal time is around 1ms
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 10);

	auto res = urlGetContent2(url, true);
	if (!res.ok) {
		return false;
	}

	JSONReader reader;
	if (!reader.parse(res.result)) {
		static auto refreshAfter = QDateTime::currentSecsSinceEpoch() + 120;
		if (refreshAfter < QDateTime::currentSecsSinceEpoch()) {
			qCritical() << "matomo json decoder read error" << QStacker16Light();
			return false;
		}
	}
	reader.getta("enabled", ok);
	if (!ok) {
		static auto refreshAfter = QDateTime::currentSecsSinceEpoch() + 120;
		if (refreshAfter < QDateTime::currentSecsSinceEpoch() + 120) {
			qCritical() << "json decoder not enabled" << QStacker16Light();
			return false;
		}
	}

	reader.getta("osName", osName);
	reader.getta("osVersion", osVersion);
	reader.getta("browserVersion", browserVersion);
	reader.getta("browserName", browserName);
	reader.getta("device", device);
	reader.getta("brand", brand);
	reader.getta("bot", bot);
	reader.getta("bot", bot);
	return true;
}
