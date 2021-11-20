#include "fileFunction/apcu2.h"
#include <QDebug>
#include <thread>
using namespace std;
struct M {
	QString k;
	double  v = 0;
	//mutex   rowLock;
};
struct T {
	QString k;
	double  v = 0;
	mutex   rowLock;
};

void spammer1() {
	auto a = APCU::get();

	auto m = make_shared<M>();
	m->v   = 123.456;
	m->k   = "ciao";

	for (int i = 0; i < 500000; i++) {
		a->store(QSL("k1 %1").arg(random() % (1024 * 1024 * 1024)), m, 1);
	}
}

uint found = 0;

void reader() {
	auto a = APCU::get();

	for (int i = 0; i < 500000; i++) {
		auto v = a->fetch<M>(QSL("k1 %1").arg(random() % 1024 * 1024 * 1024));
		if (v) {
			found++;
		}
	}
}

int apcuTest() {
	auto a = APCU::get();

	auto m = make_shared<M>();
	m->v   = 123.456;
	m->k   = "ciao";

	while (true) {
		//thread t1(spammer1);
		//thread t2(spammer1);
		thread t6(spammer1);
		//thread t8(spammer1);
		thread r1(reader);
		thread r2(reader);
		thread r3(reader);
		thread r4(reader);
		thread r5(reader);
		thread r6(reader);

		sleep(1);
		qDebug().noquote() << a->info();
		//t1.join();
		//t2.join();

		t6.join();
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
