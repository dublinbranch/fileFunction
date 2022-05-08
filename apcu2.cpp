#include "apcu2.h"
#include "unistd.h"

#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>

#include "fmt/includeMe.h"

using namespace std;
static APCU theAPCU;

struct ByExpire {};
struct ByKey {};

using boost::multi_index_container;
using namespace boost::multi_index;

struct ApcuCache_index : indexed_by<
                             hashed_unique<
                                 tag<ByKey>, BOOST_MULTI_INDEX_MEMBER(APCU::Row, std::string, key)>,
                             ordered_non_unique<
                                 tag<ByExpire>, BOOST_MULTI_INDEX_MEMBER(APCU::Row, uint, expireAt)>> {};

typedef multi_index_container<APCU::Row, ApcuCache_index> ApcuCache;
ApcuCache                                                 cache;
APCU::APCU() {
	startedAt = QDateTime::currentSecsSinceEpoch();
	new std::thread(&APCU::garbageCollector_F2, this);
}

APCU* APCU::getInstance() {
	return &theAPCU;
}

std::any APCU::fetchInner(const std::string& key) {
	auto& byKey = cache.get<ByKey>();

	RWGuard scoped(&innerLock);
	scoped.lockShared();

	if (auto iter = byKey.find(key); iter != cache.end()) {
		if (!iter->expired()) {
			hits++;
			return iter->value;
		}
		//unlock and just relock is bad, as will leave a GAP!
		//you should unlock, restart the operation under full lock, and than erase...
		//who cares, in a few second the GC will remove the record anyways
	}
	miss++;
	return std::any();
}

void APCU::storeInner(const std::string& _key, const std::any& _value, bool _overwrite, int ttl) {
	auto& byKey = cache.get<ByKey>();

	RWGuard scoped(&innerLock);
	scoped.lock();

	if (auto iter = byKey.find(_key); iter != cache.end()) {
		if (_overwrite) {
			overwrite++;

			auto old     = *iter;
			old.value    = _value;
			old.expireAt = QDateTime::currentSecsSinceEpoch() + ttl;
			byKey.replace(iter, Row(_key, _value, ttl));
		}
	} else {
		insert++;
		cache.emplace(Row(_key, _value, ttl));
	}
}

std::string APCU::info() const {
	//Poor man APCU page -.-
	double delta = QDateTime::currentSecsSinceEpoch() - startedAt;
	auto   msg   = fmt::format(R"(
		Cache size: {}
		Hits:       {} / {}s
		Miss:       {} / {}s
		Insert:     {} / {}s
		Overwrite:  {} / {}s
		Delete:     {} / {}s
		)",
	                           cache.size(), hits, hits / delta, miss, miss / delta, // 5
	                           insert, insert / delta, overwrite, overwrite / delta, deleted, deleted / delta);
	return msg;
}

//only used internally
void throwTypeError(const type_info* found, const type_info* expected) {
	throw ExceptionV2(QSL("Wrong type!! Found %1, expected %2, recheck where this key is used, maybe you have a collision").arg(found->name()).arg(expected->name()));
}

APCU::Row::Row(const std::string& _key, const std::any& _value, int ttl) {
	key      = _key;
	value    = _value;
	expireAt = QDateTime::currentSecsSinceEpoch() + ttl;
}

bool APCU::Row::expired() const {
	return QDateTime::currentSecsSinceEpoch() > expireAt;
}

bool APCU::Row::expired(qint64 ts) const {
	return ts > expireAt;
}

void APCU::garbageCollector_F2() {
	auto& byExpire = cache.get<ByExpire>();
	while (true) {
		sleep(1);
		auto now = QDateTime::currentSecsSinceEpoch();

		RWGuard scoped(&innerLock);
		scoped.lock();

		auto upper = byExpire.upper_bound(now);
		auto iter  = byExpire.begin();

		while (true) {
			//You can not have an OR condition in the for ?
			auto b = iter == byExpire.end();
			auto c = iter == upper;
			if (b || c) {
				break;
			}
			auto& row = *iter;
			(void)row;
			if (iter->expired(now)) {
				iter = byExpire.erase(iter);
				deleted++;
				continue;
			} else {
				iter++;
			}
		}
	}
}
