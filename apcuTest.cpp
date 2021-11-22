#include "fileFunction/apcu2.h"
#include <QDebug>
#include <thread>
using namespace std;
auto cache1 = APCU<uint32_t>::create();
//auto cache1 = APCU<QString>::create();
struct M {
	//QString k;
	double v = 0;
	//mutex   rowLock;
};
struct T {
	QString k;
	double  v = 0;
	mutex   rowLock;
};

void spammer1() {

	//	m->k   = "ciao";

	for (int i = 0; i < 500000; i++) {
		auto m   = make_shared<M>();
		auto key = random() % (1024 * 1024 * 1024);
		m->v     = key;
		//cache1->store(QSL("k1 %1").arg(random() % (1024 * 1024 * 1024)), m, 1);
		cache1->store(key, m, 1);
	}
}

uint found = 0;

void reader() {

	for (int i = 0; i < 500000; i++) {
		//auto v = cache1->fetch<M>(QSL("k1 %1").arg(random() % 1024 * 1024 * 1024));
		auto key = random() % 1024 * 1024 * 1024;
		auto v   = cache1->fetch<M>(key);
		if (v) {
			found++;
			if (v->v != key) {
				qCritical() << "error in the cache, value is the expected one for " << key << "found" << v->v;
			}
		}
	}
}

int apcuTest() {

	while (true) {
		thread t1(spammer1);
		thread t2(spammer1);
		//thread t6(spammer1);
		//thread t8(spammer1);
		thread r1(reader);
		thread r2(reader);
		thread r3(reader);
		thread r4(reader);
		thread r5(reader);
		thread r6(reader);

		qDebug().noquote() << cache1->info();
		t1.join();
		t2.join();

		//t6.join();
		//t8.join();

		r1.join();
		r2.join();
		r3.join();
		r4.join();
		r5.join();
		r6.join();

		qDebug() << found;
	}
	sleep(1000);
	return 0;
}
