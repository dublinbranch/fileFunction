#ifndef APCUINCLUDE_H
#define APCUINCLUDE_H

#include <QString>
#include <memory>

void apcuStore(const QString& key, std::shared_ptr<void>& obj, const std::type_info* type, void* _dtor, int ttl = 60);

template <class T>
void apcuStore(const QString& key, std::shared_ptr<T>& obj, int ttl = 60) {
	auto copy = std::shared_ptr<void>(obj);
	//T::~T is to acces the destructor
	apcuStore(key, copy, &typeid(T), (void*)&T::dtor, ttl);
}

template <class T>
void apcuStore(const QString& key, T& obj, int ttl = 60) {
	auto copy = make_shared<T>(obj);
	apcuStore(key, copy, ttl);
}

std::shared_ptr<void> apcuFetch(const QString& key, const std::type_info* type);

template <class T>
std::shared_ptr<T> apcuFetch(const QString& key) {
	auto res = apcuFetch(key, &typeid(T));
	if (res) {
		return static_pointer_cast<T>(res);
	}
	return nullptr;
}
void apcuClear();
#endif // APCUINCLUDE_H
