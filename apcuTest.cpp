#include "fileFunction/apcu2.h"

using namespace std;
struct M {
	QString k;
	double  v = 0;
	//mutex   rowLock;
	~M() {
		int x = 0;
	}
};
struct T {
	QString k;
	double  v = 0;
	mutex   rowLock;
};

int apcuTest() {
	{

		any m2 = M({"ciao", 3});
	}
	auto a = APCU::get();

	auto m = make_shared<M>();
	m->v   = 123.456;
	m->k   = "ciao";

	for (int i = 0; i < 1000; i++) {
		QString key = QString("k1 %1").arg(i);
		a->store(key, m);

		//apcuStore(key, m, 3600);
	}

	auto c1  = a->fetch<M>("k1 1");
	c1->v    = 4;
	c1->k    = "miao";
	auto c11 = a->fetch<M>("k1 1");
	auto c2  = a->fetch<M>("k1 2");
	//	auto c2 = apcuFetch<M>(key);
	//	c2->k   = "prova1";
	//	auto c3 = apcuFetch<M>(key);

	return 0;
}
