#include "apcu2.h"
using namespace std;

//only used internally
void APCU::throwTypeError(const type_info* found, const type_info* expected) {
	throw ExceptionV2(QSL("Wrong type!! Found %1, expected %2, recheck where this key is used, maybe you have a collision").arg(found->name()).arg(expected->name()));
}

bool APCU::exists(const QString& key, bool lock, CacheType::iterator& iter) {
	LockGuardV2 scoped(&innerLock, lock);

	if (iter = cache.find(key); iter != cache.end()) {
		if (iter->second.expired()) {
			cache.erase(iter);
			return false;
		}
		return true;
	}
	return false;
}

//public one
bool APCU::exists(const QString& key, bool lock) {
	CacheType::iterator iter;
	return exists(key, lock, iter);
}
