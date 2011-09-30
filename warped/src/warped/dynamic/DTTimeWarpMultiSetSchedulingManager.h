#ifndef DTTIMEWARPMULTISETSCHEDULINGMANAGER_H_
#define DTTIMEWARPMULTISETSCHEDULINGMANAGER_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "SchedulingManager.h"
#include "DTTimeWarpMultiSet.h"

class SimulationObject;
class DTTimeWarpSimulationManager;

/** This scheduling manager is to be used with the TimeWarpMultiSet. */
class DTTimeWarpMultiSetSchedulingManager: public SchedulingManager {
public:
	DTTimeWarpMultiSetSchedulingManager(
			DTTimeWarpSimulationManager *initSimulationManager);
	~DTTimeWarpMultiSetSchedulingManager();

	const Event *peekNextEvent();
	const Event *peekNextEvent(int threadId);

	const VTime &getLastEventScheduledTime();

	void configure(SimulationConfiguration &) { }

	void getExecuteLock(int threadId);
	void releaseExecuteLock(int threadId);
	void updateExecuteQueue(int threadId, const Event* updateEvent);
	void clearExecuteQueue(int threadId);

private:
	void setLastScheduledEventTime(const VTime &newTime);

	DTTimeWarpSimulationManager *mySimulationManager;
	const VTime *lastScheduledTime;

	AtomicState* executeQueueLock;
	vector <const Event*> executeQueue;
	vector<const Event*>::iterator executeQueueIterator;


};

#endif /* DTTIMEWARPMULTISETSCHEDULINGMANAGER_H_ */
