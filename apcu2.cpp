#include "apcu2.h"

using namespace std;



//only used internally
void APCU::throwTypeError(const type_info *found, const type_info *expected) {
	throw ExceptionV2(QSL("Wrong type!! Found %1, expected %2, recheck where this key is used, maybe you have a collision").arg(found->name()).arg(expected->name()));
}

bool APCU::exists(const QString& key, bool lock, CacheType::iterator& iter) {
	if (lock) {
		std::lock_guard<std::mutex> scoped(innerLock);
	}
	
	if (iter = cache.find(key); iter != cache.end()) {
		if (iter->second.expired()) {
			cache.erase(iter);
			return false;
		} else {
			return true;
		}
	} else {
		return false;
	}
}

//public one
bool APCU::exists(const QString& key, bool lock) {
	CacheType::iterator iter;
	return exists(key, lock, iter);
}

bool APCU::tryStore(const QString& key, std::any& obj, int ttl) {
	//do not call apcuExists to avoid useless double lock unlock
	std::lock_guard<std::mutex> scoped(innerLock);
	if (exists(key, false)) {
		return true;
	} else {
		auto copy  = make_shared<any>(obj);
		cache[key] = Value(copy, ttl);
		return false;
	}
}
