#ifndef THREADED_TIMEWARP_MULTISET_H_
#define THREADED_TIMEWARP_MULTISET_H_

#include <list>                         // for list
#include <set>                          // for multiset, etc
#include <string>                       // for string
#include <vector>                       // for vector, vector<>::iterator

#include "EventFunctors.h"
#include "LockState.h"
#include "NegativeEvent.h"              // for string, etc
#include "ThreadedTimeWarpEventSet.h"   // for ThreadedTimeWarpEventSet
#include "ThreadedTimeWarpMultiSetLTSF.h"
#include "ThreadedTimeWarpSimulationManager.h"

class LockState;
class SimulationConfiguration;
class SimulationObject;
class VTime;

using std::multiset;
using std::list;
using std::string;

class Event;
class NegativeEvent;
class ThreadedTimeWarpMultiSetLTSF;
class ThreadedTimeWarpSimulationManager;

class ThreadedTimeWarpMultiSet: public ThreadedTimeWarpEventSet {
public:
    ThreadedTimeWarpMultiSet(ThreadedTimeWarpSimulationManager* initSimulationManager);
    ~ThreadedTimeWarpMultiSet();

    /**
     * rollback a particular objects Event Queue
     * @param
     * @return nothing.
     */
    void rollback(SimulationObject* simObj, const VTime& rollbackTime,
                  int threadId);

    /** To get total pending message in the InputEventQueue for all Objects
     * @return Pending EventCount
     */
    ////int getMessageCount(int threadId);
    bool isScheduleQueueEmpty(int ltsfIndex);

    /** Remove and return the next event in the event set.
     *  @return The removed event.
     */
    const Event* getEvent(SimulationObject* simObj, int threadId);

    /** Remove and return the next event in the event set.
     *  @return The removed event.
     */
    const Event* getEventWhileRollback(SimulationObject* simObj, int threadId);

    /** Remove and return the next event in the event set.
     *  @return The removed event.
     */
    const Event* getEventIfStraggler(SimulationObject* simObj, int threadId);

    /** Remove and return the next event in the event set.
     *  @return The removed event.
     */
    const Event* getEvent(SimulationObject* simObj, const VTime& minimumTime,
                          int threadId);

    /** Return a reference to the next event in the event set.
     *  @return A reference to the next event in the event set.
     */
    const Event* peekEvent(SimulationObject* simObj, int threadId);

    const Event* peekEventCoastForward(SimulationObject* simObj, int threadId);

    /** Return a reference to the next event in the event set.
     *  @return A reference to the next event in the event set.
     */
    const Event* peekEvent(SimulationObject* simObj, const VTime& minimumTime,
                           int threadId);

    /** Insert an event into the event set.
     *  @return The Insert Result
     */
    bool insert(const Event*, int threadId);

    /** To get total pending message in the InputEventQueue for all Objects
     *  @return Pending EventCount
     */
    int getQueueEventCount(int objId);

    /** To print the complete Input event Queue
     * @return nothing
     */
    void printAll();

    bool threadHasUnprocessedQueueLock(int threadId, int objId);

    void getunProcessedLock(int threadId, int objId);

    void releaseunProcessedLock(int threadId, int objId);

    void getProcessedLock(int threadId, int objId);

    void releaseProcessedLock(int threadId, int objId);

    void getremovedLock(int threadId, int objId);

    void releaseremovedLock(int threadId, int objId);

    bool handleAntiMessage(SimulationObject* simObj,
                           const NegativeEvent* negativeEvent, int threadId);

    // Might need to polymorp this function with different Time parameter
    void fossilCollect(SimulationObject* simObj,
                       const VTime& fossilCollectTime, int threadId);
    void fossilCollect(SimulationObject* simObj, int fossilCollectTime,
                       int threadId);
    void fossilCollect(const Event*, int threadId);

    bool isObjectScheduled(int objId);

    bool isObjectScheduledBy(int threadId, int objId);

    void updateScheduleQueueAfterExecute(int objId, int threadId);

    /**
     Remove all events from the event set. Used to restore state after
     a catastrophic rollback while using optimistic fossil collection.
     */
    void ofcPurge(int threadId);

    /** Overriden from Configurable */
    void configure(SimulationConfiguration& configure) {
    }

    //A Temp Function to find min of Schedule Queue, will be replaced by GVT calc Function
    const VTime* nextEventToBeScheduledTime(int threadId);

    //Function to get Minimum of all Unprocessed and executing Events
    const VTime& getMinimumOfAll(int threadId);

    //Peek the first Event and Lock the Unprocessed Queue for that Object
    const Event* peekEventLockUnprocessed(SimulationObject* simObj,
                                          int threadId);
    //Peek the first Event and Lock the Unprocessed Queue for that Object while Coasting
    const Event* peekEventLockUnprocessed(SimulationObject* simObj,
                                          const VTime& minimumTime, int threadId);
    const VTime* getMinEventTime(unsigned int threadId, unsigned int objId);

    //Release all the object locks during a catastrophic rollback.
    void releaseObjectLocksRecovery();

private:

    LockState** unprocessedQueueLockState;
    LockState** processedQueueLockState;
    LockState** removedQueueLockState;

    //Queues to hold the unprocessed Events for each simObj.
    std::vector<multiset<const Event*, receiveTimeLessThanEventIdLessThan>*> unProcessedQueue;

    //Iterator for the Events in Multiset.
    multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator unProcessedQueueIterator;

    //Queues to hold the processed Events for each simObj.
    std::vector<std::vector<const Event*>*> processedQueue;

    //Queues to hold the removed events for each simObj.
    std::vector<std::vector<const Event*>*> removedEventQueue;

    //Iterators for each thread
    typedef std::vector<const Event*>::iterator vIterate;
    vIterate *vectorIterator;
    typedef multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator mIterate;
    mIterate *multisetIterator;

    //The handle to the simulation manager.
    ThreadedTimeWarpSimulationManager* mySimulationManager;

    //number of objects associated with the manager
    int objectCount;

    //Specfiy the synchronization mechanism in the config
    string syncMechanism;

    //Specify the workerThread migration status
    bool workerThreadMigration;

    //ScheduleQueues (LTSF)
    ThreadedTimeWarpMultiSetLTSF** LTSF;
    string scheduleQScheme;
    string causalityType;
    int LTSFCount;

    // ScheduleQueue (LTSF) Lookup Tables
    ThreadedTimeWarpMultiSetLTSF** LTSFByObj;
    ThreadedTimeWarpMultiSetLTSF** LTSFByThread;
};

#endif /* THREADED_TIMEWARP_MULTISET_H_ */
