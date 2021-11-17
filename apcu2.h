//You are not supposed to include this header! NO!

#include "QStacker/exceptionv2.h"
#include "mapExtensor/hmap.h"
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
	APCU* get() {
		static APCU* instance = nullptr;
		if (!instance) {
			instance = new APCU();
		}
		return instance;
	}

      private:
	std::mutex innerLock;

	void throwTypeError(const std::type_info* found, const std::type_info* expected);
	struct Value {
		shared_ptr<void*> obj;
		//I could have used std::any but at this point is just cumbersome
		const std::type_info* type     = nullptr;
		int                   expireAt = 0;
		Value()                        = delete;

		template <class T>
		Value(shared_ptr<T>& obj, int ttl) {
			this->obj = obj;
			type      = &typeid(T);
			expireAt  = QDateTime::currentSecsSinceEpoch() + ttl;
		}
		bool expired() const {
			return QDateTime::currentSecsSinceEpoch() > expireAt;
		}
		template <class T>
		bool isSameType(T) const {
			return type->hash_code() == typeid(T).hash_code();
		}
	};

	using CacheType = hmap<QString, Value>;
	CacheType cache;

	bool exists(const QString& key, bool lock, CacheType::iterator& iter);
	bool exists(const QString& key, bool lock);

	/**
	 * @brief apcuStore will OVERWRITE IF IS FOUND
	 * @param key
	 * @param obj
	 * @param ttl
	 */
	template <class T>
	void store(const QString& key, T& obj, int ttl = 60) {
		auto                        copy = make_shared<any>(obj);
		std::lock_guard<std::mutex> scoped(innerLock);
		if (auto iter = cache.find(key); iter != cache.end()) {
			auto& ref = iter->second;
			ref.type  = &typeid(T);
			ref.obj.reset();
			ref.obj      = copy;
			ref.expireAt = QDateTime::currentSecsSinceEpoch() + ttl;
		} else {
			cache[key] = {copy, ttl};
		}
	}
	/**
	 * @brief apcuTryStore
	 * @param key
	 * @param obj
	 * @param ttl
	 * @return if we inserted (there was NONE or EXPIRED) or not
	 */
	bool tryStore(const QString& key, std::any& obj, int ttl = 60);

	QString apcuFetch(const QString& key);

	template <class T>
	std::shared_ptr<T> apcuFetch(const QString& key) {
		CacheType::iterator iter;
		if (exists(key, false, iter)) {
			auto& ref = iter->second;
			//https://en.cppreference.com/w/cpp/language/typeid
			if (ref.isSameType<T>()) {
				return (shared_ptr<T>)ref.obj;
			} else {
				throwTypeError(ref.type, &typeid(T));
			}
		} else {
			return nullptr;
		}
	}
};
