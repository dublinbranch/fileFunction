#include "apcuinclude.h"
#include "fileFunction/apcu2.h"

void apcuStore(const QString& key, std::shared_ptr<void>& obj, const std::type_info* type, void* _dtor, int ttl) {
	auto a = APCU::get();
	//a->store(key, obj, type, _dtor, ttl);
}

std::shared_ptr<void> apcuFetch(const QString& key, const std::type_info* type) {
	auto a = APCU::get();
	return a->fetch(key, type);
}

void apcuClear() {
	auto a = APCU::get();
	a->clear();
}
