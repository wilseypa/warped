#ifndef THREADEThreadedTIMEWARPMULTISETLTSF_H_
#define THREADEThreadedTIMEWARPMULTISETLTSF_H_

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
#include "SplayTree.h"
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
    // Creates an LTSF queue with 'objectCount' input queues
    ThreadedTimeWarpMultiSetLTSF(int objectCount, int LTSFCountVal, const string syncMechanism,
                                 const string scheduleQScheme, const string causalityType, int** inLTSFObjId);
    ~ThreadedTimeWarpMultiSetLTSF();

    void getScheduleQueueLock(int threadId);

    void releaseScheduleQueueLock(int threadId);

    //A Temp Function to find min of Schedule Queue, will be replaced by GVT calc Function
    const VTime* nextEventToBeScheduledTime(int threadID);

    /** To get total pending message in the InputEventQueue for all Objects
     * @return Pending EventCount
     */
    int getMessageCount(int threadId);
    bool isScheduleQueueEmpty();

    //Release all schedule queue locks for releaseObjectLocksRecovery.
    // There might be a better way to do this.
    void releaseAllScheduleQueueLocks();

    // Clears the scheduleQueue
    void clearScheduleQueue(int threadId);

    // Sets lowest object positions
    void setLowestObjectPosition(int threadId, int index);

    const Event* removeLP(int objId);
    int addLP(int oldLockOwner);

    // Inserts new event into scheduleQueue and updates lowestObjectPosition
    void insertEvent(int objId, const Event* newEvent);
    // Inserts a blank event
    void insertEmptyEvent(int objId);

    // Erases the given event from the given objId, skipping the first time ??
    void eraseSkipFirst(int objId);

    int getScheduleQueueSize();

    // ??
    const Event* peek(int threadId);

    void getObjectLock(int threadId, int objId);

    void releaseObjectLock(int threadId, int objId);

    bool isObjectScheduled(int objId);

    bool isObjectScheduledBy(int threadId, int objId);

    // Release all the object locks during a catastrophic rollback.
    void releaseObjectLocksRecovery(int objNum);

    int whoHasObjectLock(int objId);
private:
    //Lowest event position pointer for MULTILTSF
    vector<multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator>
    lowestObjectPosition;

    ///Schedule Queue - MULTILTSF
    multiset<const Event*, receiveTimeLessThanEventIdLessThan>* scheduleQueue;

    //Lowest event position pointer for LadderQ. Also used by Splay Tree.
    vector<const Event*> lowestLadderObjectPosition;

    ///Schedule Queue - LadderQ (Strict and Relaxed)
    LadderQueueStrict  *ladderQStrict;
    LadderQueueRelaxed *ladderQRelaxed;

    ///Schedule Queue - SplayTree
    SplayTree* splayTree;

    ///Schedule Queue Lock
    LockState* scheduleQueueLock;

    ///Object Status Lock
    vector<LockState*> objectStatusLock;

    //Specfiy the synchronization mechanism in the config
    string syncMechanism;

    //Specify the scheduleQ scheme in the config
    string scheduleQScheme;

    //Specify the event causality type
    string eventCausality;

    // Number of LTSF Queues in use
    int LTSFCount;

    int objectCount;

    int** LTSFObjId;

    unsigned int minReceiveTime;
};
#endif /* ThreadedTIMEWARPMULTISETLTSF_H_ */
