#include "apcu2.h"

using namespace std;

//only used internally
void throwTypeError(const type_info* found, const type_info* expected) {
	throw ExceptionV2(QSL("Wrong type!! Found %1, expected %2, recheck where this key is used, maybe you have a collision").arg(found->name()).arg(expected->name()));
}

bool Value::expired() const {
	return QDateTime::currentSecsSinceEpoch() > expireAt;
}

bool Value::expired(qint64 ts) const {
	return ts > expireAt;
}

/*
 * void APCU::garbageCollector_F2() {
        while (true) {
                sleep(1);
                quint32 scanned = 0;
                quint32 erased  = 0;
                auto    now     = QDateTime::currentSecsSinceEpoch();

                LockGuardV2 scoped(&innerLock);

                while (true) {
                        scanned++;
                        //Redis alike old gen gc collector
                        if (scanned % 100 == 0) {
                                scoped.unlock();
                                if (erased < 25) {
                                        break;
                                }
                                erased = 0;
                                //this will deschedule this thread. So if someone else has job to do it can
                                std::this_thread::yield();
                                scoped.lock();
                        }
                        //take a random bucket
                        auto bucket = random() % (cache.bucket_count() - 1);
                        if (auto size = cache.bucket_size(bucket); size) { //if there is something inside
                                //hashmap iterator can not jump, is only incremental, so instead of doing a long jump, we will do a small one now!
                                auto skip = 0;
                                if (size > 1) {
                                        skip = random() % (size - 1);
                                }

                                auto element = std::next(cache.begin(bucket), skip);
                                if (element->second.expired(now)) {
                                        //Local iterator is , no idea but you have to search again to get the full one
                                        cache.erase(element->first);
                                        deleted++;
                                        continue;
                                }
                        }
                }
        }
}
*/
