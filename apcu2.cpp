#include "apcu2.h"
using namespace std;

//only used internally
void throwTypeError(const type_info* found, const type_info* expected) {
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

//void APCU::store(const QString& key, shared_ptr<void>& obj, const type_info* type, void* _dtor, int ttl, bool lock) {
//	auto        v = Value(obj, ttl, type, _dtor);
//	LockGuardV2 scoped(&innerLock, lock);

//	if (auto iter = cache.find(key); iter != cache.end()) {
//		iter->second = move(v);
//	} else {
//		cache[key] = move(v);
//	}
//}

void APCU::clear() {
	LockGuardV2 scoped(&innerLock);
}

//APCU::Value::Value(shared_ptr<void>& _obj, int ttl, const type_info* _type, void* _dtor) {
//	this->obj = _obj;
//	type      = _type;
//	expireAt  = QDateTime::currentSecsSinceEpoch() + ttl;
//	dtor      = _dtor;
//}
