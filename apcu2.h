#pragma once

#include "QStacker/exceptionv2.h"
#include "mapExtensor/rwguard.h"
#include <QDateTime>
#include <QHash>
#include <QString>
#include <any>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

#define QSL(str) QStringLiteral(str)
void throwTypeError(const std::type_info* found, const std::type_info* expected);

class NonCopyable {
      protected:
	NonCopyable()  = default;
	~NonCopyable() = default;

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};

struct Value {
	std::any obj;
	qint64   expireAt = 0;
	Value()           = delete;
	template <class T>
	Value(std::shared_ptr<T>& _obj, int ttl) {
		obj      = _obj;
		expireAt = QDateTime::currentSecsSinceEpoch() + ttl;
	}
	bool expired() const;
	bool expired(qint64 ts) const;
};

template <class Key>
class APCU : private NonCopyable {
      public:
	static APCU* create() {
		return new APCU();
	}

	template <class T>
	std::shared_ptr<T> fetch(const Key& key) {
		typename CacheType::iterator iter;
		//we need to keep the lock, so we can copy the shared, to avoid it goes out scope while in our hands!
		RWGuard scoped(&innerLock);
		scoped.lockShared();

		if (iter = cache.find(key); iter != cache.end()) {
			if (iter->second.expired()) {
				//unlock and just relock is bad, as will leave a GAP!
				//you should unlock, restart the operation under full lock, and than erase...
				//who cares, in a few second the GC will remove the record anyways
				return nullptr;
			}
			hits++;
			return any_cast<std::shared_ptr<T>>(iter->second.obj);
		}
		miss++;
		return nullptr;
	}
	QString info() const {
		//Poor man APCU page -.-
		double delta = QDateTime::currentSecsSinceEpoch() - startedAt;
		auto   msg   = QSL(R"(
	Cache size: %1
	Hits:       %2 / %3s
	Miss:       %4 / %5s
	Insert:     %6 / %7s
	Overwrite:  %8 / %9s
	Delete:     %10 / %11s
	)")
		               .arg(cache.size())
		               .arg(hits)
		               .arg(hits / delta)
		               .arg(miss)
		               .arg(miss / delta) // 5
		               .arg(insert)
		               .arg(insert / delta)
		               .arg(overwite)
		               .arg(overwite / delta)
		               .arg(deleted)
		               .arg(deleted / delta);
		return msg;
	}

	/**
	 * @brief store will OVERWRITE IF IS FOUND
	 * @param key
	 * @param obj
	 * @param ttl
	 */
	template <class T>
	void store(const Key& key, std::shared_ptr<T>& obj, int ttl = 60) {
		auto    v = Value(obj, ttl);
		RWGuard scoped(&innerLock);
		scoped.lock();
		if (auto iter = cache.find(key); iter != cache.end()) {
			overwite++;
			iter->second = std::move(v);
		} else {
			insert++;
			cache.emplace(key, std::move(v));
		}
	}

	void clear();

	//1 overwrite will NOT trigger 1 delete and 1 inserted
	std::atomic<uint64_t> overwite;
	std::atomic<uint64_t> insert;
	std::atomic<uint64_t> deleted;
	std::atomic<uint64_t> hits;
	std::atomic<uint64_t> miss;

      private:
	void garbageCollector_F2() {
		while (true) {
			sleep(1);
			quint32 scanned = 0;
			auto    now     = QDateTime::currentSecsSinceEpoch();

			RWGuard scoped(&innerLock);
			scoped.lock();

			auto iter = cache.begin();
			auto end  = cache.end();
			while (iter != end) {
				scanned++;
				if (scanned % 500 == 0) {
					scoped.unlock();
					//this will deschedule this thread. So if someone else has job to do it can
					std::this_thread::yield();
					scoped.lock();
				}

				if (iter->second.expired(now)) {
					iter = cache.erase(iter);
					deleted++;
					continue;
				} else {
					iter++;
				}
			}
		}
	}
	std::shared_mutex innerLock;
	qint64            startedAt = 0;

	APCU() {
		startedAt = QDateTime::currentSecsSinceEpoch();
		new std::thread(&APCU::garbageCollector_F2, this);
	}

	using CacheType = std::unordered_map<Key, Value>;
	CacheType cache;

	//	/**
	//	 * @brief apcuTryStore
	//	 * @param key
	//	 * @param obj
	//	 * @param ttl
	//	 * @return if we inserted (there was NONE or EXPIRED) or not
	//	 */
	//	template <class T>
	//	bool tryStore(const QString& key, T& obj, int ttl = 60) {
	//		CacheType::iterator         iter;
	//		std::lock_guard<std::mutex> scoped(innerLock);
	//		if (exists(key, false, iter)) {
	//			return true;
	//		} else {
	//			iter->second = Value(obj, ttl);
	//			return false;
	//		}
	//	}
};

template <class T>
void apcuStore(const QString& key, std::shared_ptr<T>& obj, int ttl = 60) {
	//	auto a = APCU::create();
	//	a->store(key, obj, ttl);
}

template <class T>
void apcuStore(const QString& key, T& obj, int ttl = 60) {
	auto copy = make_shared<T>(obj);
	apcuStore(key, copy, ttl);
}

template <class T>
std::shared_ptr<T> apcuFetch(const QString& key) {
	//	auto a   = APCU::create();
	//	auto res = a->fetch<T>(key);
	//	if (res) {
	//		return static_pointer_cast<T>(res);
	//	}
	return nullptr;
}
void apcuClear();
int  apcuTest();
