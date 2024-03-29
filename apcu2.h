#pragma once

#include "QStacker/exceptionv2.h"
#include "fileFunction/mixin.h"
#include <QDateTime>
#include <QString>
#include <any>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#define QSL(str) QStringLiteral(str)
void throwTypeError(const std::type_info* found, const std::type_info* expected);

class APCU : private NoCopy {
      public:
	APCU();
	static APCU* getInstance();

	struct Row {
		//Corpus munus
		Row() = delete;
		Row(const std::string& _key, const std::any& _value, int ttl);

		//Member
		std::string key;
		std::any    value;
		uint        expireAt = 0;

		bool expired() const;
		bool expired(qint64 ts) const;
	};

	/**
	 * We hide the implementation as multi index will kill compile time
	 */
	std::any fetchInner(const std::string& key);
	void     storeInner(const std::string& _key, const std::any& _value, bool overwrite = false, int ttl = 60);

	template <class T>
	std::shared_ptr<T> fetch(const std::string& key) {
		(void)key;
		auto res = fetchInner(key);
		if (res.has_value()) {
			return any_cast<std::shared_ptr<T>>(res);
		} else {
			return nullptr;
		}
	}

	std::string info() const;

	/**
	 * @brief store will OVERWRITE IF IS FOUND
	 * @param key
	 * @param obj
	 * @param ttl
	 */
	template <class T>
	void store(const std::string& key, std::shared_ptr<T>& obj, int ttl = 60) {
		std::any value = obj;
		storeInner(key, value, true, ttl);
	}

	void clear();

	//1 overwrite will NOT trigger 1 delete and 1 inserted
	std::atomic<uint64_t> overwrite;
	std::atomic<uint64_t> insert;
	std::atomic<uint64_t> deleted;
	std::atomic<uint64_t> hits;
	std::atomic<uint64_t> miss;

      private:
	void              garbageCollector_F2();
	std::shared_mutex innerLock;
	qint64            startedAt = 0;

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
void apcuStore(const std::string& key, std::shared_ptr<T>& obj, int ttl = 60) {
	auto a = APCU::getInstance();
	a->store(key, obj, ttl);
}

template <class T>
void apcuStore(const QString& key, std::shared_ptr<T>& obj, int ttl = 60) {
	apcuStore(key.toStdString(), obj, ttl);
}

template <class T>
void apcuStore(const std::string& key, const T& obj, int ttl = 60) {
    auto copy = std::make_shared<T>(obj);
	apcuStore(key, copy, ttl);
}

template <class T>
void apcuStore(const QString& key, const T& obj, int ttl = 60) {
	apcuStore(key.toStdString(), obj, ttl);
}

template <class T>
std::shared_ptr<T> apcuFetch(const std::string& key) {
	auto a   = APCU::getInstance();
	auto res = a->fetch<T>(key);
	return res;
}

template <class T>
std::shared_ptr<T> apcuFetch(const QString& key) {
	return apcuFetch<T>(key.toStdString());
}

void apcuClear();
int  apcuTest();
