#ifndef THREADEThreadedIMEWARPMULTISETSCHEDULINGMANAGER_H_
#define THREADEThreadedIMEWARPMULTISETSCHEDULINGMANAGER_H_


#include <vector>                       // for vector, vector<>::iterator

#include "SchedulingManager.h"          // for SchedulingManager
#include "ThreadedTimeWarpMultiSet.h"
#include "warped.h"

class AtomicState;
class Event;
class SimulationConfiguration;
class SimulationObject;
class ThreadedTimeWarpSimulationManager;
class VTime;

/** This scheduling manager is to be used with the TimeWarpMultiSet. */
class ThreadedTimeWarpMultiSetSchedulingManager: public SchedulingManager {
public:
    ThreadedTimeWarpMultiSetSchedulingManager(
        ThreadedTimeWarpSimulationManager* initSimulationManager);
    ~ThreadedTimeWarpMultiSetSchedulingManager();

    const Event* peekNextEvent();
    const Event* peekNextEvent(int threadId);

    const VTime& getLastEventScheduledTime();

    void configure(SimulationConfiguration&) { }

    void getExecuteLock(int threadId);
    void releaseExecuteLock(int threadId);
    void updateExecuteQueue(int threadId, const Event* updateEvent);
    void clearExecuteQueue(int threadId);

private:
    void setLastScheduledEventTime(const VTime& newTime);

    ThreadedTimeWarpSimulationManager* mySimulationManager;
    const VTime* lastScheduledTime;

    AtomicState* executeQueueLock;
    vector <const Event*> executeQueue;
    vector<const Event*>::iterator executeQueueIterator;


};

#endif /* ThreadedTIMEWARPMULTISETSCHEDULINGMANAGER_H_ */
