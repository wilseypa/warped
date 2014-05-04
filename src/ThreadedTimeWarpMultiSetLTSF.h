#ifndef THREADED_TIMEWARP_MULTISET_LTSF_H_
#define THREADED_TIMEWARP_MULTISET_LTSF_H_

#include <list>                         // for list
#include <set>                          // for multiset, etc
#include <string>                       // for string
#include <vector>                       // for vector

#include "AtomicState.h"
#include "EventFunctors.h"
#include "LadderQRelaxed.h"
#include "LadderQStrict.h"
#include "LockState.h"
#include "NegativeEvent.h"
#include "SplayTree.h"                  // for string, etc
#include "ThreadedTimeWarpEventSet.h"
#include "ThreadedTimeWarpSimulationManager.h"

class LadderQueueRelaxed;
class LadderQueueStrict;
class LockState;
class SplayTree;
class VTime;

using std::multiset;
using std::list;
using std::string;

class Event;
class NegativeEvent;
class ThreadedTimeWarpSimulationManager;

class ThreadedTimeWarpMultiSetLTSF {
public:
    // Creates an LTSF queue
    ThreadedTimeWarpMultiSetLTSF(
            const string syncMechanism, const string scheduleQScheme, const string causalityType,
            std::vector<const Event*> *lowestObjPos, std::vector<LockState*> *objStatusLock );

    ~ThreadedTimeWarpMultiSetLTSF();

    void getScheduleQueueLock(int threadId);

    void releaseScheduleQueueLock(int threadId);

    const VTime* nextEventToBeScheduledTime(int threadID);

    bool isScheduleQueueEmpty();

    //Release all schedule queue locks for releaseObjectLocksRecovery.
    void releaseAllScheduleQueueLocks();

    // Clears the scheduleQueue
    void clearScheduleQueue(int threadId);

    // Sets lowest object positions
    void setLowestObjectPosition(int threadId, int index);

    // Inserts new event into scheduleQueue and updates lowestObjectPosition
    void insertEvent(int objId, const Event* newEvent);

    // Inserts a blank event
    void insertEmptyEvent(int objId);

    // Erases the given event from the given objId, skipping the first time ??
    void eraseSkipFirst(int objId);

    //Peek lowest event in the schedule queue
    const Event* peek(int threadId);

    void getObjectLock(int threadId, int objId);

    void releaseObjectLock(int threadId, int objId);

    bool isObjectScheduled(int objId);

    int whoHasObjectLock(int objId);

    bool isObjectScheduledBy(int threadId, int objId);

    // Release all the object locks during a catastrophic rollback.
    void releaseObjectLocksRecovery(int objNum);


private:

    //Specfiy the synchronization mechanism in the config
    string syncMechanism;

    //Specify the scheduleQ scheme in the config
    string scheduleQScheme;

    //Specify the event causality type
    string eventCausality;

    //Lowest event position pointer
    std::vector<const Event*> *lowestObjectPosition;

    //Schedule Queue
    multiset<const Event*, receiveTimeLessThanEventIdLessThan>* scheduleQueue;

    //Schedule Queue - LadderQ (Strict and Relaxed)
    LadderQueueStrict  *ladderQStrict;
    LadderQueueRelaxed *ladderQRelaxed;

    //Schedule Queue - SplayTree
    SplayTree* splayTree;

    //Schedule Queue Lock
    LockState* scheduleQueueLock;

    //Object Status Lock
    std::vector<LockState*> *objectStatusLock;

    // Number of LTSF Queues in use
    int LTSFCount;

    int objectCount;

    unsigned int minReceiveTime;
};

#endif /* THREADED_TIMEWARP_MULTISET_LTSF_H_ */
