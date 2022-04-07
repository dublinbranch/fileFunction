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
	void   start();
	qint64 pause();

	qint64 nsecsElapsed() const;

      private:
	bool          paused = true;
	QElapsedTimer timer;
	qint64        total = 0;
};

class ThreadStatus {
      public:
	struct Timing {
		ElapsedTimerV2 timer;

		//Total execution time including log writing, clickhouse, sql, whatever until NOW (or is called pause)
		qint64 total() const;
		ElapsedTimerV2 clickHouse;
		//Time spent doing IO, mostly reading the disk cache
		ElapsedTimerV2 IO;
		//Once data is fully sent to browser
		qint64 flush = 0;
		//The actual time spend executing code
		qint64 execution() const;

		//time spent in sql, this is computed only until the delivery, all sql after the http is sent are irrelevant
		qint64 sqlImmediate;
		qint64 sqlDeferred;

		void reset();
		void addSqlTime(qint64 addMe);
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
