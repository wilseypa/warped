#include <string.h>                     // for NULL, memset
#include <iostream>                     // for operator<<, basic_ostream, etc

#include "Event.h"                      // for Event, operator<<
#include "EventId.h"                    // for EventId
#include "LockState.h"                  // for LockState
#include "NegativeEvent.h"              // for NegativeEvent
#include "ObjectID.h"                   // for ObjectID
#include "SimulationObject.h"           // for SimulationObject
#include "StragglerEvent.h"             // for StragglerEvent
#include "ThreadedStateManager.h"       // for ThreadedStateManager
#include "ThreadedTimeWarpMultiSet.h"
#include "ThreadedTimeWarpMultiSetLTSF.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "VTime.h"                      // for VTime
#include "WarpedDebug.h"                // for debugout
#include "WorkerInformation.h"          // for __sync_fetch_and_add_4
#include "warped.h"                     // for ASSERT

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::multiset;

#define MAX_MIGRATION_CNT 50


ThreadedTimeWarpMultiSet::ThreadedTimeWarpMultiSet(
                        ThreadedTimeWarpSimulationManager *initSimulationManager) :
        mySimulationManager(initSimulationManager) {

    //Input queues
    objectCount = initSimulationManager->getNumberOfSimulationObjects();

    //synchronization mechanism
    syncMechanism = initSimulationManager->getSyncMechanism();

    int threadCount = initSimulationManager->getNumberofThreads();
    scheduleQScheme = initSimulationManager->getScheduleQScheme();
    causalityType   = initSimulationManager->getCausalityType();

    //Worker thread migration
    workerThreadMigration = initSimulationManager->getWorkerThreadMigration();

    //Create object locks for the schedule queues
    vector<LockState*> *objectStatusLock = new vector<LockState*>;
    vector<const Event*> *lowestObjPos = new vector<const Event*>;

    for (int i = 0; i < objectCount; i++) {
        LockState *lockState = new LockState();
        objectStatusLock->push_back(lockState);
        lowestObjPos->push_back(NULL);
    }

    unprocessedQueueLockState = new LockState *[objectCount];
    processedQueueLockState = new LockState *[objectCount];
    removedQueueLockState = new LockState *[objectCount];

    LTSFCount = initSimulationManager->getScheduleQCount();

    LTSF = new ThreadedTimeWarpMultiSetLTSF *[LTSFCount];

    //Assert whether object count greater than or equal to LTSF queue count
    ASSERT(objectCount >= LTSFCount);

    //Assert whether thread count greater than or equal  to LTSF queue count
    //only for static thread assignment to LTSF queues
    if (workerThreadMigration) {
        ASSERT(threadCount >= LTSFCount);
    }

    LTSFByThread = new unsigned int[threadCount];
    LTSFByObj = new unsigned int[objectCount];
    MigrateCntPerThread = new unsigned int[threadCount];

    //Initialize schedule queues
    for (int i=0; i < LTSFCount; i++) {
        LTSF[i] = new ThreadedTimeWarpMultiSetLTSF( 
                            syncMechanism, scheduleQScheme, causalityType, 
                            lowestObjPos, objectStatusLock );
    }

    //Assign threads to LTSF queues
    for (int i=0; i < threadCount; i++) {
        LTSFByThread[i] = i % LTSFCount;
        MigrateCntPerThread[i] = 0;
    }

    //Warning message if uneven distribution of threads to LTSF queues
    if ((threadCount % LTSFCount) != 0) {
        cout << "Threads unevenly distributed amongst LTSF queues" << endl;
    }

    //Iterators for each threads
    vectorIterator = new vIterate[threadCount];
    multisetIterator = new mIterate[threadCount];

    //Initializing Unprocessed Event Queue
    for (int i = 0; i < objectCount; i++) {
        multiset<const Event*, receiveTimeLessThanEventIdLessThan>* objSet =
                    new multiset<const Event*, receiveTimeLessThanEventIdLessThan> ;
        unProcessedQueue.push_back(objSet);
        processedQueue.push_back(new vector<const Event*>);
        removedEventQueue.push_back(new vector<const Event*>);

        //Initialzing Locks for each Object
        unprocessedQueueLockState[i] = new LockState();
        processedQueueLockState[i] = new LockState();
        removedQueueLockState[i] = new LockState();

        //Create lookup table to associate between an unprocessed queue id
        // and the appropriate LTSF queue
        LTSFByObj[i] = i % LTSFCount;
    }
}

ThreadedTimeWarpMultiSet::~ThreadedTimeWarpMultiSet() {
    //freeing Unprocessed Event Queue
    multiset<const Event*, receiveTimeLessThanEventIdLessThan>* deleteMultiSet =
        NULL;
    while (!unProcessedQueue.empty()) {
        deleteMultiSet = unProcessedQueue.back();
        delete deleteMultiSet;
    }

    vector<const Event*>* toDeleteVector = NULL;
    while (!processedQueue.empty()) {
        toDeleteVector = processedQueue.back();
        processedQueue.pop_back();
        delete toDeleteVector;
    }
    delete unprocessedQueueLockState;
    delete processedQueueLockState;
    delete removedQueueLockState;

    //  deleting each Threads Iterator
    delete vectorIterator;
    delete multisetIterator;
}

bool ThreadedTimeWarpMultiSet::threadHasUnprocessedQueueLock(int threadId,
                                                             int objId) {
    return (unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism));
}

void ThreadedTimeWarpMultiSet::getunProcessedLock(int threadId, int objId) {
#if USETSX_RTM
    unsigned status;
    int retries = 0;

    if (tsxCommits == 0 && tsxAborts > TSXRTM_RETRIES * 100) { }
    else if (tsxRtmRetries > 1 && tsxAborts > tsxCommits << 1) {
        tsxRtmRetries--;
    } else {
        do {
            status = _xbegin();
            if (status == _XBEGIN_STARTED) {
                if (!unprocessedQueueLockState[objId]->isLocked()) {
                   return;
                }
                _xabort(_ABORT_LOCK_BUSY);
            }
            ABORT_COUNT(_XA_RETRY, status);
            ABORT_COUNT(_XA_EXPLICIT, status);
            ABORT_COUNT(_XA_CONFLICT, status);
            ABORT_COUNT(_XA_CAPACITY, status);
            if (!(status & _XABORT_RETRY) ||
                ((status & _XABORT_EXPLICIT) && _XABORT_CODE(status) != _ABORT_LOCK_BUSY))
            {
                break;
            } else if ((status & _XABORT_EXPLICIT) && _XABORT_CODE(status) == _ABORT_LOCK_BUSY) {
                while (unprocessedQueueLockState[objId]->isLocked());
            } else if (status & _XABORT_CONFLICT) {
                _mm_pause();
            }
        } while (retries++ < tsxRtmRetries);
        tsxAborts++;
    }
#endif
    if (syncMechanism == "HleAtomicLock") {
        unprocessedQueueLockState[objId]->setHleLock(threadId);
    } else {
        unprocessedQueueLockState[objId]->setLock(threadId, syncMechanism);
        ASSERT(unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism));
    }
}

void ThreadedTimeWarpMultiSet::releaseunProcessedLock(int threadId, int objId) {
#if USETSX_RTM
    if (!unprocessedQueueLockState[objId]->isLocked()) {
        _xend();
        tsxCommits++;
        return;
    }
#endif
    if(!unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism)) return;
    if (syncMechanism == "HleAtomicLock") {
        unprocessedQueueLockState[objId]->releaseHleLock(threadId);
    } else {
        ASSERT(unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism));
        unprocessedQueueLockState[objId]->releaseLock(threadId, syncMechanism);
    }
}

void ThreadedTimeWarpMultiSet::getProcessedLock(int threadId, int objId) {
#if USETSX_RTM
    unsigned status;
    int retries = 0;

    if (tsxCommits == 0 && tsxAborts > TSXRTM_RETRIES * 100) { }
    else if (tsxRtmRetries > 1 && tsxAborts > tsxCommits << 1) {
        tsxRtmRetries--;
    } else {
        do {
            status = _xbegin();
            if (status == _XBEGIN_STARTED) {
                if (!processedQueueLockState[objId]->isLocked()) {
                   return;
                }
                _xabort(_ABORT_LOCK_BUSY);
            }
            ABORT_COUNT(_XA_RETRY, status);
            ABORT_COUNT(_XA_EXPLICIT, status);
            ABORT_COUNT(_XA_CONFLICT, status);
            ABORT_COUNT(_XA_CAPACITY, status);
            if (!(status & _XABORT_RETRY) ||
                ((status & _XABORT_EXPLICIT) && _XABORT_CODE(status) != _ABORT_LOCK_BUSY))
            {
                break;
            } else if ((status & _XABORT_EXPLICIT) && _XABORT_CODE(status) == _ABORT_LOCK_BUSY) {
                while (processedQueueLockState[objId]->isLocked());
            } else if (status & _XABORT_CONFLICT) {
                _mm_pause();
            }
        } while (retries++ < tsxRtmRetries);
        tsxAborts++;
    }
#endif
    if (syncMechanism == "HleAtomicLock") {
        processedQueueLockState[objId]->setHleLock(threadId);
    } else {
        processedQueueLockState[objId]->setLock(threadId, syncMechanism);
        ASSERT(processedQueueLockState[objId]->hasLock(threadId, syncMechanism));
    }
}

void ThreadedTimeWarpMultiSet::releaseProcessedLock(int threadId, int objId) {
#if USETSX_RTM
    if (!processedQueueLockState[objId]->isLocked()) {
        _xend();
        tsxCommits++;
        return;
    }
#endif
    if(!processedQueueLockState[objId]->hasLock(threadId, syncMechanism)) return;
    if (syncMechanism == "HleAtomicLock") {
        processedQueueLockState[objId]->releaseHleLock(threadId);
    } else {
        ASSERT(processedQueueLockState[objId]->hasLock(threadId, syncMechanism));
        processedQueueLockState[objId]->releaseLock(threadId, syncMechanism);
    }
}

void ThreadedTimeWarpMultiSet::getremovedLock(int threadId, int objId) {
    removedQueueLockState[objId]->setLock(threadId, syncMechanism);
    if (!_xtest()) {
        ASSERT(removedQueueLockState[objId]->hasLock(threadId, syncMechanism));
    }
}

void ThreadedTimeWarpMultiSet::releaseremovedLock(int threadId, int objId) {
    if (!_xtest()) {
        ASSERT(removedQueueLockState[objId]->hasLock(threadId, syncMechanism));
    }
    removedQueueLockState[objId]->releaseLock(threadId, syncMechanism);
}

bool ThreadedTimeWarpMultiSet::isObjectScheduled(int objId) {
    return LTSF[LTSFByObj[objId]]->isObjectScheduled(objId);
}

bool ThreadedTimeWarpMultiSet::isObjectScheduledBy(int threadId, int objId) {
    return LTSF[LTSFByObj[objId]]->isObjectScheduledBy(threadId, objId);
}

//not thread Safe
int ThreadedTimeWarpMultiSet::getQueueEventCount(int objId) {
    int size;
    size = unProcessedQueue[objId]->size();
    return size;
}

//This Function will be called by the worker when the object has been scheduled, so no need to update schedule queue(need to verify this)
const Event* ThreadedTimeWarpMultiSet::getEvent(SimulationObject* simObj,
                                                int threadId) {
    const Event* ret = NULL;
    unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
    if (!this->unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism))
    { this->getunProcessedLock(threadId, objId); }
    if (getQueueEventCount(objId) > 0) {
        //Remove from Unprocessed Queue
        ret = *(unProcessedQueue[objId]->begin());
        //Return NULL if ret is a Straggler/Negative
        if (dynamic_cast<const StragglerEvent*>(ret) || ret->getReceiveTime()
                < simObj->getSimulationTime()) {
            this->releaseunProcessedLock(threadId, objId);
            return NULL;
        }
        unProcessedQueue[objId]->erase(unProcessedQueue[objId]->begin());
        this->releaseunProcessedLock(threadId, objId);
        //Insert into Processed Queue
        if (dynamic_cast<const StragglerEvent*>(ret))
        { ASSERT(false); }
        this->getProcessedLock(threadId, objId);
        processedQueue[objId]->push_back(ret);
        this->releaseProcessedLock(threadId, objId);
    } else {
        this->releaseunProcessedLock(threadId, objId);
    }
    //ASSERT(this->isObjectScheduledBy(threadId, objId));
    return ret;
}

const Event* ThreadedTimeWarpMultiSet::getEventWhileRollback(
    SimulationObject* simObj, int threadId) {
    const Event* ret = NULL;
    unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
    if (getQueueEventCount(objId) > 0) {
        //Remove from Unprocessed Queue
        ret = *(unProcessedQueue[objId]->begin());
        //Return NULL if ret is a Straggler/Negative
        if (dynamic_cast<const StragglerEvent*>(ret) || ret->getReceiveTime()
                < simObj->getSimulationTime()) {
            this->releaseunProcessedLock(threadId, objId);
            return NULL;
        }
        unProcessedQueue[objId]->erase(unProcessedQueue[objId]->begin());
        //Insert into Processed Queue
        if (dynamic_cast<const StragglerEvent*>(ret))
        { ASSERT(false); }
        this->getProcessedLock(threadId, objId);
        processedQueue[objId]->push_back(ret);
        this->releaseProcessedLock(threadId, objId);
    }
    return ret;
}

const Event* ThreadedTimeWarpMultiSet::getEventIfStraggler(
    SimulationObject* simObj, int threadId) {
    const Event* ret = NULL;
    unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
    this->getunProcessedLock(threadId, objId);
    if (getQueueEventCount(objId) > 0) {
        //Remove from Unprocessed Queue
        ret = (*unProcessedQueue[objId]->begin());
        if (dynamic_cast<const StragglerEvent*>(ret)) {
            unProcessedQueue[objId]->erase(unProcessedQueue[objId]->begin());
        } else if (ret->getReceiveTime() >= simObj->getSimulationTime()) {
            this->releaseunProcessedLock(threadId, objId);
            ret = NULL;
        }
    } else {
        this->releaseunProcessedLock(threadId, objId);
    }
    return ret;
}

const Event* ThreadedTimeWarpMultiSet::getEvent(SimulationObject* simObj,
                                                const VTime& minimumTime, int threadId) {
    const Event* retval = NULL;
    ASSERT(simObj != NULL);
    ASSERT( minimumTime.getApproximateIntTime() );
    const Event* peeked = peekEvent(simObj, threadId);
    if (peeked != NULL) {
        retval = getEventWhileRollback(simObj, threadId);
        ASSERT(peeked == retval);
    }
    return retval;
}

const VTime* ThreadedTimeWarpMultiSet::nextEventToBeScheduledTime(int threadId) {
    const VTime* minimum = NULL;
    const VTime* temp;
    // Iterate through all schedule queues, and find lowest item
    for (int i=0; i<LTSFCount; i++) {
        temp = (LTSF[i]->nextEventToBeScheduledTime(threadId));
        if ((minimum == NULL) || ((temp != NULL) && (*temp < *minimum))) {
            minimum = temp;
        }
    }
    return minimum;
}

const Event* ThreadedTimeWarpMultiSet::peekEvent(SimulationObject* simObj,
                                                 int threadId) {
    const Event* ret = NULL;

    bool releaseWhileReturn = true;
    if (simObj == NULL) {
        ret = LTSF[LTSFByThread[threadId-1]]->peek(threadId);
    } else {
        unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
        if (!this->unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism)) {
            this->getunProcessedLock(threadId, objId);
            releaseWhileReturn = false;
        }
        ret = *(unProcessedQueue[objId]->begin());
        if (dynamic_cast<const StragglerEvent*>(ret)) {
            if (!releaseWhileReturn)
            { this->releaseunProcessedLock(threadId, objId); }
            return NULL;
        }
        if (!releaseWhileReturn) {
            this->releaseunProcessedLock(threadId, objId);
        }
    }
    return ret;
}

const Event* ThreadedTimeWarpMultiSet::peekEventCoastForward(
    SimulationObject* simObj, int threadId) {
    const Event* ret = NULL;
    unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
    this->getunProcessedLock(threadId, objId);
    if (getQueueEventCount(objId) > 0) {
        ret = *(unProcessedQueue[objId]->begin());
        if (dynamic_cast<const StragglerEvent*>(ret) || ret->getReceiveTime()
                < simObj->getSimulationTime()) {
            this->releaseunProcessedLock(threadId, objId);
            return NULL;
        }
    } else {
        this->releaseunProcessedLock(threadId, objId);
    }
    return ret;
}

const Event* ThreadedTimeWarpMultiSet::peekEvent(SimulationObject* simObj,
                                                 const VTime& minimumTime, int threadId) {
    const Event* retval = peekEvent(simObj, threadId);
    if (retval != NULL) {
        if (!(retval->getReceiveTime() < minimumTime)) {
            retval = NULL;
        }
    }
    return retval;
}

const Event* ThreadedTimeWarpMultiSet::peekEventLockUnprocessed(
    SimulationObject* simObj, const VTime& minimumTime, int threadId) {
    const Event* retval = peekEventLockUnprocessed(simObj, threadId);
    if (retval != NULL) {
        if (!(retval->getReceiveTime() < minimumTime)) {
            retval = NULL;
            unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
            releaseunProcessedLock(threadId, objId);
        }
    }
    return retval;
}

bool ThreadedTimeWarpMultiSet::insert(const Event* receivedEvent, int threadId) {

    unsigned int objId = receivedEvent->getReceiver().getSimulationObjectID();
    this->getunProcessedLock(threadId, objId);
    unProcessedQueue[objId]->insert(receivedEvent);
    debug::debugout << "( " << mySimulationManager->getSimulationManagerID()
                    << " ) " << mySimulationManager->getObjectHandle(
                        receivedEvent->getReceiver())->getName() << " has received ::::"
                    << *receivedEvent << " - " << threadId << "\n";
    multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator itee;
    itee = unProcessedQueue[objId]->begin();
    // The event was just inserted at the beginning, so update the Schedule Queue
    if (receivedEvent == *(itee)) {
        LTSF[LTSFByObj[objId]]->getScheduleQueueLock(threadId);
        if (!this->isObjectScheduled(objId)) {
            LTSF[LTSFByObj[objId]]->eraseSkipFirst(objId);
            LTSF[LTSFByObj[objId]]->insertEvent(objId, receivedEvent);
        }
        LTSF[LTSFByObj[objId]]->releaseScheduleQueueLock(threadId);
    }
    this->releaseunProcessedLock(threadId, objId);
    return true;
}

bool ThreadedTimeWarpMultiSet::isScheduleQueueEmpty(int ltsfIndex) {

    if (ltsfIndex < LTSFCount) {
        return LTSF[ltsfIndex]->isScheduleQueueEmpty();

    } else if (ltsfIndex == LTSFCount) { // simulation termination check condition

        bool isEmpty = true;
        for (int index = 0; (index < LTSFCount) && (isEmpty); index++) { // check all schedule queues
            isEmpty &= LTSF[index]->isScheduleQueueEmpty();
        }
        return isEmpty;

    } else {
        ASSERT(false);
    }
}

bool ThreadedTimeWarpMultiSet::handleAntiMessage(SimulationObject* simObj,
                                                 const NegativeEvent* negativeEvent, int threadId) {
    bool eventWasRemoved = false;
    unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
    if (!this->unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism))
    { this->getunProcessedLock(threadId, objId); }
    multisetIterator[threadId] = unProcessedQueue[objId]->begin();

    while (multisetIterator[threadId] != unProcessedQueue[objId]->end()
            && !eventWasRemoved) {
        if ((*(multisetIterator[threadId]))->getSender()
                == negativeEvent->getSender()
                && ((*(multisetIterator[threadId]))->getEventId()
                    == negativeEvent->getEventId())) {
            const Event* eventToRemove = *multisetIterator[threadId];
            if (dynamic_cast<const StragglerEvent*>(*(multisetIterator[threadId]))) {
                debug::debugout
                        << "Negative Message Found in Handling Anti-Message .."
                        << endl;
                multisetIterator[threadId]++;
                continue;
            }
            unProcessedQueue[objId]->erase(multisetIterator[threadId]);
            // Put the removed event here in case it needs to be used for comparisons in
            // lazy cancellation.
            this->getremovedLock(threadId, objId);
            removedEventQueue[objId]->push_back(eventToRemove);
            this->releaseremovedLock(threadId, objId);
            eventWasRemoved = true;
        } else {
            multisetIterator[threadId]++;
        }
    }
    this->releaseunProcessedLock(threadId, objId);

    return eventWasRemoved;
}

void ThreadedTimeWarpMultiSet::rollback(SimulationObject* simObj,
                                        const VTime& rollbackTime, int threadId) {
    // Go through the entire processed events queue and put any events with
    // a receive time greater than or equal to the rollback time back in the
    // unprocessed queue.
    unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
    this->getProcessedLock(threadId, objId);
    vectorIterator[threadId] = processedQueue[objId]->begin();
    int tempCount = 0;
    if (rollbackTime.getApproximateIntTime() == 0) {
        tempCount = processedQueue[objId]->size();
    } else {
        vectorIterator[threadId] = processedQueue[objId]->begin();
        while (vectorIterator[threadId] != processedQueue[objId]->end()
                && (*(vectorIterator[threadId]))->getReceiveTime()
                < rollbackTime) {
            (vectorIterator[threadId])++;
            tempCount++;
        }
        const unsigned int eventIdRollback =
                mySimulationManager->getStateManagerNew()->getEventIdForRollback(threadId, objId);
        const unsigned int senderObjectId =
                mySimulationManager->getStateManagerNew()->getSenderObjectIdForRollback(threadId, objId);
        while (vectorIterator[threadId] != processedQueue[objId]->end()) {
            EventId tempEventId = (*(vectorIterator[threadId]))->getEventId();
            unsigned int tempSenderObjectId =
                        (*(vectorIterator[threadId]))->getSender().getSimulationObjectID();
            if( (tempEventId.getEventNum() != eventIdRollback) || 
                            (tempSenderObjectId != senderObjectId) ) {
                (vectorIterator[threadId])++;
                tempCount++;
            } else {
                break;
            }
        }
        tempCount = processedQueue[objId]->size() - tempCount;
    }

    debug::debugout << "( " << mySimulationManager->getSimulationManagerID()
                    << " ) Object - " << objId << " Rollback returns : " << tempCount
                    << " events back to Unprocessed Queue - " << threadId << endl;
    unProcessedQueue[objId]->insert(vectorIterator[threadId],
                                    processedQueue[objId]->end());
    processedQueue[objId]->erase(vectorIterator[threadId],
                                 processedQueue[objId]->end());
    this->releaseProcessedLock(threadId, objId);
}

void ThreadedTimeWarpMultiSet::fossilCollect(SimulationObject* simObj,
                                             const VTime& fossilCollectTime, int threadId) {
    unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
    // Removed the processed events with time less than the collect time.
    this->getProcessedLock(threadId, objId);
    vectorIterator[threadId] = processedQueue[objId]->begin();
    while (vectorIterator[threadId] != processedQueue[objId]->end()
            && (*(vectorIterator[threadId]))->getReceiveTime()
            < fossilCollectTime) {
        simObj->reclaimEvent(*(vectorIterator[threadId]));
        //delete *(vectorIterator[threadId]);//Replace this by a reclaim function from object
        vectorIterator[threadId]++;
    }
    processedQueue[objId]->erase(processedQueue[objId]->begin(),
                                 vectorIterator[threadId]);
    this->releaseProcessedLock(threadId, objId);

    // Also remove the processed events that have been removed.
    this->getremovedLock(threadId, objId);
    vectorIterator[threadId] = removedEventQueue[objId]->begin();
    while (vectorIterator[threadId] != removedEventQueue[objId]->end()) {
        if ((*(vectorIterator[threadId]))->getReceiveTime() < fossilCollectTime) {
            const Event* eventToReclaim = *(vectorIterator[threadId]);
            vectorIterator[threadId] = removedEventQueue[objId]->erase(
                                           vectorIterator[threadId]);
            simObj->reclaimEvent(eventToReclaim);
            //  delete eventToReclaim;//Replace this by a reclaim function from object
        } else {
            vectorIterator[threadId]++;
        }
    }
    this->releaseremovedLock(threadId, objId);
}

void ThreadedTimeWarpMultiSet::fossilCollect(SimulationObject* object,
                                             int fossilCollectTime, int threadId) {
    unsigned int objId = object->getObjectID()->getSimulationObjectID();

    // Removed the processed events with time less than the collect time.
    this->getProcessedLock(threadId, objId);
    vectorIterator[threadId] = processedQueue[objId]->begin();
    while (vectorIterator[threadId] != processedQueue[objId]->end()
            && ((*(vectorIterator[threadId]))->getReceiveTime()).getApproximateIntTime()
            < fossilCollectTime) {
        object->reclaimEvent(*(vectorIterator[threadId]));
        vectorIterator[threadId]++;
    }
    processedQueue[objId]->erase(processedQueue[objId]->begin(),
                                 vectorIterator[threadId]);
    this->releaseProcessedLock(threadId, objId);

    // Also remove the processed events that have been removed.
    this->getremovedLock(threadId, objId);
    vectorIterator[threadId] = removedEventQueue[objId]->begin();
    while (vectorIterator[threadId] != removedEventQueue[objId]->end()) {
        if ((*(vectorIterator[threadId]))->getReceiveTime().getApproximateIntTime()
                < fossilCollectTime) {
            const Event* eventToReclaim = *(vectorIterator[threadId]);
            vectorIterator[threadId] = removedEventQueue[objId]->erase(
                                           vectorIterator[threadId]);
            object->reclaimEvent(eventToReclaim);
        } else {
            vectorIterator[threadId]++;
        }
    }
    this->releaseremovedLock(threadId, objId);
}

void ThreadedTimeWarpMultiSet::fossilCollect(const Event* toRemove,
                                             int threadId) {
    unsigned int objId = toRemove->getReceiver().getSimulationObjectID();
    bool foundMatch = false;

    this->getProcessedLock(threadId, objId);
    vectorIterator[threadId] = processedQueue[objId]->begin();
    while (vectorIterator[threadId] != processedQueue[objId]->end()) {
        if ((*(vectorIterator[threadId]))->getEventId()
                == toRemove->getEventId()
                && (*(vectorIterator[threadId]))->getSender()
                == toRemove->getSender()) {
            processedQueue[objId]->erase(vectorIterator[threadId]);
            foundMatch = true;
            break;
        } else {
            vectorIterator[threadId]++;
        }
    }
    this->releaseProcessedLock(threadId, objId);

    if (!foundMatch) {
        this->getremovedLock(threadId, objId);
        vectorIterator[threadId] = removedEventQueue[objId]->begin();
        while (vectorIterator[threadId] != removedEventQueue[objId]->end()) {
            if ((*(vectorIterator[threadId]))->getEventId()
                    == toRemove->getEventId()
                    && (*(vectorIterator[threadId]))->getSender()
                    == toRemove->getSender()) {
                vectorIterator[threadId] = removedEventQueue[objId]->erase(
                                               vectorIterator[threadId]);
                foundMatch = true;
                break;
            } else {
                vectorIterator[threadId]++;
            }
        }
        this->releaseremovedLock(threadId, objId);
    }
}

void ThreadedTimeWarpMultiSet::updateScheduleQueueAfterExecute(int objId, int threadId) {

    const Event* firstEvent = NULL;

    if (!this->unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism)) { 
        this->getunProcessedLock(threadId, objId);
    }

    /* If worker thread migration requested */
    if( workerThreadMigration && (MigrateCntPerThread[threadId-1] < MAX_MIGRATION_CNT) ) {
        ASSERT( LTSFByObj[objId] == LTSFByThread[threadId-1] );
        LTSFByObj[objId] = (LTSFByObj[objId]+1) % LTSFCount;
        LTSFByThread[threadId-1] = LTSFByObj[objId];
        MigrateCntPerThread[threadId-1]++;
    }

    LTSF[LTSFByObj[objId]]->getScheduleQueueLock(threadId);
    if (unProcessedQueue[objId]->size() > 0) {
        firstEvent = *(unProcessedQueue[objId]->begin());
    }

    // Check that lowest object position for this objId is scheduleQueue->end
    if (firstEvent != NULL) {
        LTSF[LTSFByObj[objId]]->insertEvent(objId, firstEvent);
    } else {
        LTSF[LTSFByObj[objId]]->insertEmptyEvent(objId);
    }

    debug::debugout <<" ( "<< threadId << ") Returning object " <<objId <<" back to SCheQ"<<endl;

    LTSF[LTSFByObj[objId]]->releaseObjectLock(threadId, objId);
    LTSF[LTSFByObj[objId]]->releaseScheduleQueueLock(threadId);
    this->releaseunProcessedLock(threadId, objId);
}

void ThreadedTimeWarpMultiSet::ofcPurge(int threadId) {
    multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator msit;

    for (int index = 0; index < LTSFCount; index++) {
        LTSF[index]->clearScheduleQueue(threadId);
    }
    for (int i = 0; i < objectCount; i++) {
        this->getunProcessedLock(threadId, i);
        msit = unProcessedQueue[i]->begin();
        while (msit != unProcessedQueue[i]->end()) {
            (*msit)->~Event();
            ::operator delete((void*)(*msit));
            unProcessedQueue[i]->erase(msit++);
        }
        this->releaseunProcessedLock(threadId, i);
        LTSF[LTSFByObj[i]]->setLowestObjectPosition(threadId, i);
    }

    for (int i = 0; i < objectCount; i++) {
        this->getProcessedLock(threadId, i);
        vector<const Event*>::iterator ip = processedQueue[i]->begin();
        while (ip != processedQueue[i]->end()) {
            (*ip)->~Event();
            ::operator delete((void*)(*ip));
            ip++;
        }
        processedQueue[i]->clear();
        this->releaseProcessedLock(threadId, i);
    }

    for (int i = 0; i < objectCount; i++) {
        vector<const Event*>::iterator ir = removedEventQueue[i]->begin();
        while (ir != removedEventQueue[i]->end()) {
            (*ir)->~Event();
            ::operator delete((void*)(*ir));
            ir++;
        }
        removedEventQueue[i]->clear();
    }
}

const Event* ThreadedTimeWarpMultiSet::peekEventLockUnprocessed(
    SimulationObject* simObj, int threadId) {
    const Event* ret = NULL;
    ASSERT(simObj != NULL);
    unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
    this->getunProcessedLock(threadId, objId);
    if (getQueueEventCount(objId) > 0) {
        ret = *(unProcessedQueue[objId]->begin());
        if (dynamic_cast<const StragglerEvent*>(ret) || ret->getReceiveTime()
                < simObj->getSimulationTime()) {
            this->releaseunProcessedLock(threadId, objId);
            return NULL;
        }
    }
    return ret;
}

const VTime* ThreadedTimeWarpMultiSet::getMinEventTime(unsigned int threadId,
                                                       unsigned objId) {
    VTime* ret = NULL;
    bool haslock = (threadHasUnprocessedQueueLock(threadId, objId));
    if (haslock) {
        if (unProcessedQueue[objId]->size() > 0) {
            ret = (*unProcessedQueue[objId]->begin())->getReceiveTime().clone();
        }
        return ret;
    } else {
        getunProcessedLock(threadId, objId);
        if (unProcessedQueue[objId]->size() > 0) {
            ret = (*unProcessedQueue[objId]->begin())->getReceiveTime().clone();
        }

        releaseunProcessedLock(threadId, objId);
        return ret;
    }
}

void ThreadedTimeWarpMultiSet::releaseObjectLocksRecovery() {
    for (int objNum = 0; objNum < objectCount; objNum++) {
        for (int i = 0; i<LTSFCount; i++) {
            LTSF[i]->releaseObjectLocksRecovery(objNum);
        }
        if (unprocessedQueueLockState[objNum]->isLocked()) {
            unprocessedQueueLockState[objNum]->releaseLock(
                unprocessedQueueLockState[objNum]->whoHasLock(),
                syncMechanism);
            debug::debugout << "Releasing Unprocessed Queue " << objNum
                            << " during recovery." << endl;
        }
        if (processedQueueLockState[objNum]->isLocked()) {
            processedQueueLockState[objNum]->releaseLock(
                processedQueueLockState[objNum]->whoHasLock(),
                syncMechanism);
            debug::debugout << "Releasing Processed Queue " << objNum
                            << " during recovery." << endl;
        }
        if (removedQueueLockState[objNum]->isLocked()) {
            removedQueueLockState[objNum]->releaseLock(
                removedQueueLockState[objNum]->whoHasLock(),
                syncMechanism);
            debug::debugout << "Releasing Removed Queue " << objNum
                            << " during recovery." << endl;
        }
    }
    for (int i = 0; i<LTSFCount; i++) {
        LTSF[i]->releaseAllScheduleQueueLocks();
    }
}

#if USETSX_RTM
void ThreadedTimeWarpMultiSet::reportTSXstats() {
    for (int i = 0; i < LTSFCount; i++) {
        std::cout << "LTSF[" << i << "]:" << std::endl;
        LTSF[i]->reportTSXstats();
    }
}
#endif
