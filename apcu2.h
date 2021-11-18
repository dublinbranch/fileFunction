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

#define QSL(str) QStringLiteral(str)
void throwTypeError(const std::type_info* found, const std::type_info* expected);

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
	 * @brief store will OVERWRITE IF IS FOUND
	 * @param key
	 * @param obj
	 * @param ttl
	 */
	//	template <class T>
	//	void store(const QString& key, T& obj, int ttl = 60, bool lock = true) {
	//		auto copy = make_shared<T>(obj);
	//		store(key, copy, ttl, lock);
	//	}

	///void store(const QString& key, shared_ptr<void>& obj, const std::type_info* type, void* _dtor, int ttl = 60, bool lock = true);

	template <class T>
	std::shared_ptr<T> fetch(const QString& key) {
		//		auto res = fetch(key, &typeid(T));
		//		if (res) {
		//			return static_pointer_cast<T>(res);
		//		}
		//		return nullptr;

		CacheType::iterator iter;
		if (exists(key, true, iter)) {
			auto& ref = iter->second;
			return any_cast<shared_ptr<T>>(ref.obj2);
		}
		return nullptr;
	}

	//If you pass a shared ptr, it will not deep copy it
	//passing a * is wild and will not be tollerated instead
	template <class T>
	void store(const QString& key, shared_ptr<T>& obj, int ttl = 60, bool lock = true) {

		auto        v = Value(obj, ttl);
		LockGuardV2 scoped(&innerLock, lock);

		if (auto iter = cache.find(key); iter != cache.end()) {
			iter->second = move(v);
		} else {
			cache[key] = move(v);
		}

		//store(key, copy, type, ttl, lock);
	}

	std::shared_ptr<void> fetch(const QString& key, const std::type_info* type) {
		CacheType::iterator iter;
		if (exists(key, true, iter)) {
			auto& ref = iter->second;
			if (ref.isSameType(type)) {
				return ref.obj;
			} else {
				throwTypeError(ref.type, type);
			}
		}
		return nullptr;
	}

	void clear();

      private:
	std::mutex innerLock;

	struct Value {
		any              obj2;
		shared_ptr<void> obj = nullptr;
		//I could have used std::any but at this point is just cumbersome
		//https://en.cppreference.com/w/cpp/language/typeid
		const std::type_info* type     = nullptr;
		qint64                expireAt = 0;
		//Ptr to the obj destructor, as we can not share easily the type AFAIK
		void* dtor = nullptr;
		~Value() {
		}
		//Value()                        = delete;
		Value() {
			int x = 0;
		}
		//Value(shared_ptr<void>& _obj, int ttl, const std::type_info* _type, void* _dtor);

		template <class T>
		Value(shared_ptr<T>& obj, int ttl) {
			//this->obj = static_pointer_cast<void>(obj);
			obj2     = obj;
			type     = &typeid(T);
			expireAt = QDateTime::currentSecsSinceEpoch() + ttl;
		}

		template <class T>
		Value(T& obj, int ttl) {
			this->obj = static_pointer_cast<void>(make_shared<T>(obj));
			type      = &typeid(T);
			expireAt  = QDateTime::currentSecsSinceEpoch() + ttl;
		}
		bool expired() const {
			return QDateTime::currentSecsSinceEpoch() > expireAt;
		}
		template <class T>
		bool isSameType() const {
			return type->hash_code() == typeid(T).hash_code();
		}
		bool isSameType(const std::type_info* target) const {
			return type->hash_code() == target->hash_code();
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
