#include "threadstatush.h"

ThreadStatus                       threadStatus;
//Just a default for the program who do not uses this technology
ThreadStatus::Status               fakeLocalThreadStatus;
thread_local ThreadStatus::Status* localThreadStatus = &fakeLocalThreadStatus;

void ThreadStatus::Timing::reset() {
	*this = Timing();
	timer.start();
}
