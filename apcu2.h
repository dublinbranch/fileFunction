//You are not supposed to include this header! NO!

#include "QStacker/exceptionv2.h"
#include "mapExtensor/hmap.h"
#include "mapExtensor/lockguardv2.h"
#include <QDateTime>
#include <QHash>
#include <QString>
#include <any>
#include <memory>
#include <mutex>
#include <typeinfo>

#define QSL(str) QStringLiteral(str)

using namespace std;

class NonCopyable {
      protected:
	NonCopyable()  = default;
	~NonCopyable() = default;

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};

class APCU : private NonCopyable {
      public:
	static APCU* get() {
		static APCU* instance = nullptr;
		if (!instance) {
			instance = new APCU();
		}
		return instance;
	}

	bool exists(const QString& key, bool lock = true);

	/**
	 * @brief apcuStore will OVERWRITE IF IS FOUND
	 * @param key
	 * @param obj
	 * @param ttl
	 */
	template <class T>
	void store(const QString& key, T& obj, int ttl = 60, bool lock = true) {
		auto copy = make_shared<T>(obj);
		store(key, copy, ttl, lock);
	}

	//If you pass a shared ptr, it will not deep copy it
	//passing a * is wild and will not be tollerated instead
	template <class T>
	void store(const QString& key, shared_ptr<T>& obj, int ttl = 60, bool lock = true) {
		LockGuardV2 scoped(&innerLock, lock);

		if (auto iter = cache.find(key); iter != cache.end()) {
			iter->second = Value(obj, ttl);
		} else {
			cache[key] = {obj, ttl};
		}
	}

	template <class T>
	std::shared_ptr<T> apcuFetch(const QString& key) {
		CacheType::iterator iter;
		if (exists(key, false, iter)) {
			auto& ref = iter->second;
			if (ref.isSameType<T>()) {
				return static_pointer_cast<T>(ref.obj);
			} else {
				throwTypeError(ref.type, &typeid(T));
			}
		} else {
			return nullptr;
		}
	}

      private:
	std::mutex innerLock;

	void throwTypeError(const std::type_info* found, const std::type_info* expected);
	struct Value {
		shared_ptr<void> obj = nullptr;
		//I could have used std::any but at this point is just cumbersome
		//https://en.cppreference.com/w/cpp/language/typeid
		const std::type_info* type     = nullptr;
		int                   expireAt = 0;
		//Value()                        = delete;
		Value() {
			int x = 0;
		}
		template <class T>
		Value(shared_ptr<T>& obj, int ttl) {
			this->obj = (shared_ptr<void>)obj;
			type      = &typeid(T);
			expireAt  = QDateTime::currentSecsSinceEpoch() + ttl;
		}
		template <class T>
		Value(T& obj, int ttl) {
			auto             copy = make_shared<T>(obj);
			shared_ptr<void> obj2 = (shared_ptr<void>)copy;
			this->obj             = obj2;
			type                  = &typeid(T);
			expireAt              = QDateTime::currentSecsSinceEpoch() + ttl;
		}
		bool expired() const {
			return QDateTime::currentSecsSinceEpoch() > expireAt;
		}
		template <class T>
		bool isSameType() const {
			return type->hash_code() == typeid(T).hash_code();
		}
	};

	using CacheType = hmap<QString, Value>;
	CacheType cache;

	bool exists(const QString& key, bool lock, CacheType::iterator& iter);

	/**
	 * @brief apcuTryStore
	 * @param key
	 * @param obj
	 * @param ttl
	 * @return if we inserted (there was NONE or EXPIRED) or not
	 */
	template <class T>
	bool tryStore(const QString& key, T& obj, int ttl = 60) {
		CacheType::iterator         iter;
		std::lock_guard<std::mutex> scoped(innerLock);
		if (exists(key, false, iter)) {
			return true;
		} else {
			iter->second = Value(obj, ttl);
			return false;
		}
	}
};
