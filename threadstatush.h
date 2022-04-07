#ifndef THREADSTATUSH_H
#define THREADSTATUSH_H

#include "mapExtensor/hmap.h"

#include <QString>
#include <thread>

#include <QElapsedTimer>

enum class ThreadState {
	Idle,
	Beast,
	Immediate,
	Deferred,
	MyQuery,
	MyCache,
	ClickHouse
};

class ElapsedTimerV2 {
      public:
	void start() {
		timer.restart();
	}
	qint64 pause() {
		total += timer.nsecsElapsed();
		return total;
	}
	qint64 nsecsElapsed() {
		return total + timer.nsecsElapsed();
	}

      private:
	QElapsedTimer timer;
	qint64        total = 0;
};

class ThreadStatus {
      public:
	struct Timing {
		QElapsedTimer timer;

		//Total execution time including log writing, clickhouse, sql, whatever
		qint64 total;
		//time spent in sql, this is computed only until the delivery, all sql after the http is sent are irrelevant
		qint64 sql;
		ElapsedTimerV2 clickHouse;
		//Time spent doing IO, mostly reading the disk cache
		ElapsedTimerV2 IO;
		//Once data is fully sent to browser
		qint64 flush;
		//The actual time spend executing code
		qint64 execution;
		void   reset();
	};

	struct Status {
		ThreadState state;
		int         tid = 0;
		Timing      time;
		QString     info;
	};

	Status* newStatus() const {
		return new Status();
	}

	hmap<std::thread::id, Status*> pool;
};

#endif // THREADSTATUSH_H
