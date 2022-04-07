#include "threadstatush.h"

ThreadStatus threadStatus;
//Just a default for the program who do not uses this technology
ThreadStatus::Status               fakeLocalThreadStatus;
thread_local ThreadStatus::Status* localThreadStatus = &fakeLocalThreadStatus;

void ThreadStatus::Timing::addSqlTime(qint64 addMe) {
	if (flush) {
		sqlDeferred += addMe;
	} else {
		sqlImmediate += addMe;
	}
}

qint64 ThreadStatus::Timing::total() const {
	return timer.nsecsElapsed();
}

qint64 ThreadStatus::Timing::execution() const {
	return total() - (sqlDeferred + sqlImmediate + IO.nsecsElapsed() + clickHouse.nsecsElapsed());
}

void ThreadStatus::Timing::reset() {
	*this = Timing();
	timer.start();
}

void ElapsedTimerV2::start() {
	paused = false;
	timer.restart();
}

qint64 ElapsedTimerV2::pause() {
	paused = true;
	total += timer.nsecsElapsed();
	return total;
}

qint64 ElapsedTimerV2::nsecsElapsed() const {
	if (paused) {
		return total;
	}
	return total + timer.nsecsElapsed();
}
