#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <stdexcept>                    // for runtime_error

#include "Event.h"                      // for Event
#include "LadderQRelaxed.h"             // for LadderQueueRelaxed
#include "LadderQStrict.h"              // for LadderQueueStrict
#include "LockState.h"                  // for LockState
#include "ObjectID.h"                   // for ObjectID
#include "SplayTree.h"                  // for SplayTree, string
#include "ThreadedTimeWarpMultiSetLTSF.h"
#include "VTime.h"                      // for VTime
#include "WarpedDebug.h"                // for debugout
#include "warped.h"                     // for ASSERT

ThreadedTimeWarpMultiSetLTSF::ThreadedTimeWarpMultiSetLTSF( 
            const string syncMech,
            const string scheQScheme,
            const string causalityType,
            std::vector<const Event*> *lowestObjPos,
            std::vector<LockState*> *objStatusLock ) :
        syncMechanism(syncMech),
        scheduleQScheme(scheQScheme),
        eventCausality(causalityType),
        lowestObjectPosition(lowestObjPos),
        objectStatusLock(objStatusLock) {

    ASSERT(lowestObjectPosition);
    ASSERT(objectStatusLock);

    // Set up scheduleQueue (LTSF queue)
    if (scheduleQScheme == "MultiSet") {
        scheduleQueue = new multiset<const Event*,receiveTimeLessThanEventIdLessThan> ;
    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            ladderQRelaxed = new LadderQueueRelaxed();
        } else if (eventCausality == "Strict") {
            ladderQStrict = new LadderQueueStrict();
        } else {
            cerr << "Invalid ladder causality type." << endl;
            abort();
        }
    } else if (scheduleQScheme == "SplayTree") {
        splayTree = new SplayTree;
    } else {
        cerr << "Invalid schedule queue scheme" << endl;
        abort();
    }

    //Create the schedule queue lock
    scheduleQueueLock = new LockState();
}

ThreadedTimeWarpMultiSetLTSF::~ThreadedTimeWarpMultiSetLTSF() {
    delete scheduleQueueLock;
    if (scheduleQScheme == "MultiSet") {
        delete scheduleQueue;
    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            delete ladderQRelaxed;
        } else {
            delete ladderQStrict;
        }
    } else { //SplayTree
        delete splayTree;
    }
}

void ThreadedTimeWarpMultiSetLTSF::getScheduleQueueLock(int threadId) {
    scheduleQueueLock->setLock(threadId, syncMechanism);
    ASSERT(scheduleQueueLock->hasLock(threadId, syncMechanism));
}

void ThreadedTimeWarpMultiSetLTSF::releaseScheduleQueueLock(int threadId) {
    ASSERT(scheduleQueueLock->hasLock(threadId, syncMechanism));
    scheduleQueueLock->releaseLock(threadId, syncMechanism);
}

const VTime* ThreadedTimeWarpMultiSetLTSF::nextEventToBeScheduledTime(int threadID) {
    const VTime* ret = NULL;
    if (scheduleQScheme == "MultiSet") {
        this->getScheduleQueueLock(threadID);
        if (scheduleQueue->size() > 0) {
            ret = &((*scheduleQueue->begin())->getReceiveTime());
        }
        this->releaseScheduleQueueLock(threadID);

    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            this->getScheduleQueueLock(threadID);
            if (!ladderQRelaxed->empty()) {
                ret = &(ladderQRelaxed->begin(false)->getReceiveTime());
            }
            this->releaseScheduleQueueLock(threadID);
        } else {
            this->getScheduleQueueLock(threadID);
            if (!ladderQStrict->empty()) {
                ret = &(ladderQStrict->begin()->getReceiveTime());
            }
            this->releaseScheduleQueueLock(threadID);
        }

    } else { //SplayTree
        this->getScheduleQueueLock(threadID);
        if (splayTree->peekEvent()) {
            ret = &(splayTree->peekEvent()->getReceiveTime());
        }
        this->releaseScheduleQueueLock(threadID);
    }
    return (ret);
}

bool ThreadedTimeWarpMultiSetLTSF::isScheduleQueueEmpty() {
    if (scheduleQScheme == "MultiSet") {
        return scheduleQueue->empty();
    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            return ladderQRelaxed->empty();
        } else {
            return ladderQStrict->empty();
        }
    } else { //SplayTree
        return (splayTree->size() == 0) ? true : false;
    }
}

void ThreadedTimeWarpMultiSetLTSF::releaseAllScheduleQueueLocks() {
    if (scheduleQueueLock->isLocked()) {
        scheduleQueueLock->releaseLock(scheduleQueueLock->whoHasLock(), syncMechanism);
        debug::debugout << "Releasing Schedule Queue during recovery." << endl;
    }
}

void ThreadedTimeWarpMultiSetLTSF::clearScheduleQueue(int threadId) {
    if (scheduleQScheme == "MultiSet") {
        this->getScheduleQueueLock(threadId);
        scheduleQueue->clear();
        this->releaseScheduleQueueLock(threadId);

    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            ladderQRelaxed->clear();
        } else {
            this->getScheduleQueueLock(threadId);
            ladderQStrict->clear();
            this->releaseScheduleQueueLock(threadId);
        }
    } else { //SplayTree
        this->getScheduleQueueLock(threadId);
        splayTree->clear();
        this->releaseScheduleQueueLock(threadId);
    }
}

void ThreadedTimeWarpMultiSetLTSF::setLowestObjectPosition(int threadId, int index) {
    if (scheduleQScheme == "MultiSet") {
        this->getScheduleQueueLock(threadId);
        lowestObjectPosition->at(index) = NULL;
        this->releaseScheduleQueueLock(threadId);

    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            lowestObjectPosition->at(index) = NULL;
        } else {
            this->getScheduleQueueLock(threadId);
            lowestObjectPosition->at(index) = NULL;
            this->releaseScheduleQueueLock(threadId);
        }

    } else { //SplayTree
        this->getScheduleQueueLock(threadId);
        lowestObjectPosition->at(index) = NULL;
        this->releaseScheduleQueueLock(threadId);
    }
}

void ThreadedTimeWarpMultiSetLTSF::insertEvent(int objId, const Event* newEvent) {
    if (scheduleQScheme == "MultiSet") {
        ASSERT(newEvent);
        lowestObjectPosition->at(objId) = *(scheduleQueue->insert(newEvent));
    } else if (scheduleQScheme == "LadderQueue") {
        ASSERT(newEvent);
        if(eventCausality == "Relaxed") {
            lowestObjectPosition->at(objId) = ladderQRelaxed->insert(newEvent);
        } else {
            debug::debugout<<"Inserting " << newEvent << "." <<endl;
            lowestObjectPosition->at(objId) = ladderQStrict->insert(newEvent);
        }
    } else { //SplayTree
        ASSERT(newEvent);
        splayTree->insert(newEvent);
        lowestObjectPosition->at(objId) = newEvent;
    }
}

void ThreadedTimeWarpMultiSetLTSF::insertEmptyEvent(int objId) {
    if (scheduleQScheme == "MultiSet") {
        lowestObjectPosition->at(objId) = NULL;
    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            lowestObjectPosition->at(objId) = NULL;
        } else {
            lowestObjectPosition->at(objId) = NULL;
        }
    } else { //SplayTree
        lowestObjectPosition->at(objId) = NULL;
    }
}

void ThreadedTimeWarpMultiSetLTSF::eraseSkipFirst(int objId) {
    // Do not erase the first time.
    ASSERT(lowestObjectPosition->size() > objId);
    if (scheduleQScheme == "MultiSet") {
        if (lowestObjectPosition->at(objId) != NULL) {
            scheduleQueue->erase(lowestObjectPosition->at(objId));
        }
    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            if (lowestObjectPosition->at(objId) != NULL) {
                ladderQRelaxed->erase(lowestObjectPosition->at(objId));
            }
        } else {
            if (lowestObjectPosition->at(objId) != NULL) {
                ladderQStrict->erase(lowestObjectPosition->at(objId));
            }
        }
    } else { //SplayTree
        if (lowestObjectPosition->at(objId) != NULL) {
            splayTree->erase(lowestObjectPosition->at(objId));
        }
    }
}

const Event* ThreadedTimeWarpMultiSetLTSF::peek(int threadId) {

    const Event* ret = NULL;
    if (scheduleQScheme == "MultiSet") {
        this->getScheduleQueueLock(threadId);
        if (scheduleQueue->size() > 0) {
            debug::debugout << "(" << threadId << "T) Peeking from Schedule Queue" << endl;
            ret = *(scheduleQueue->begin());
            unsigned int objId = ret->getReceiver().getSimulationObjectID();
            debug::debugout << "(" << threadId << ") Locking the Object " << objId << endl;
            getObjectLock(threadId, objId);
            scheduleQueue->erase(scheduleQueue->begin());
            lowestObjectPosition->at(objId) = NULL;
        }
        this->releaseScheduleQueueLock(threadId);

    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            this->getScheduleQueueLock(threadId);
            if (!ladderQRelaxed->empty()) {
                debug::debugout << "(" << threadId << "T) Peeking from Schedule Queue" << endl;
                ret = ladderQRelaxed->dequeue();
                if (ret == NULL) {
                    cout << "dequeue() func returned NULL" << endl;
                    this->releaseScheduleQueueLock(threadId);
                    return ret;
                }
                unsigned int objId = ret->getReceiver().getSimulationObjectID();
                debug::debugout << "(" << threadId << ") Locking the Object " << objId << endl;
                this->getObjectLock(threadId, objId);
                lowestObjectPosition->at(objId) = NULL;
            }
            this->releaseScheduleQueueLock(threadId);
        } else {
            this->getScheduleQueueLock(threadId);
            if (!ladderQStrict->empty()) {
                debug::debugout << "(" << threadId << "T) Peeking from Schedule Queue" << endl;
                ret = ladderQStrict->dequeue();
                if (ret == NULL) {
                    cout << "dequeue() func returned NULL" << endl;
                    this->releaseScheduleQueueLock(threadId);
                    return ret;
                }
                unsigned int objId = ret->getReceiver().getSimulationObjectID();
                debug::debugout << "(" << threadId << ") Locking the Object " << objId << endl;
                this->getObjectLock(threadId, objId);
                lowestObjectPosition->at(objId) = NULL;
            }
            this->releaseScheduleQueueLock(threadId);
        }
    } else { //SplayTree
        this->getScheduleQueueLock(threadId);
        if (splayTree->peekEvent()) {
            debug::debugout << "(" << threadId << "T) Peeking from Schedule Queue" << endl;
            if (NULL == (ret = splayTree->getEvent())) {
                cout << "getEvent() func returned NULL" << endl;
                this->releaseScheduleQueueLock(threadId);
                return ret;
            }
            unsigned int objId = ret->getReceiver().getSimulationObjectID();
            debug::debugout << "(" << threadId << ") Locking the Object " << objId << endl;
            this->getObjectLock(threadId, objId);
            lowestObjectPosition->at(objId) = NULL;
        }
        this->releaseScheduleQueueLock(threadId);
    }
    return ret;
}

void ThreadedTimeWarpMultiSetLTSF::getObjectLock(int threadId, int objId) {
    objectStatusLock->at(objId)->setLock(threadId, syncMechanism);
    ASSERT(objectStatusLock->at(objId)->hasLock(threadId, syncMechanism));
}

void ThreadedTimeWarpMultiSetLTSF::releaseObjectLock(int threadId, int objId) {
    ASSERT(objectStatusLock->at(objId)->hasLock(threadId, syncMechanism));
    objectStatusLock->at(objId)->releaseLock(threadId, syncMechanism);
}

bool ThreadedTimeWarpMultiSetLTSF::isObjectScheduled(int objId) {
    return objectStatusLock->at(objId)->isLocked();
}

// Returns owner of object lock - used when transferring
int ThreadedTimeWarpMultiSetLTSF::whoHasObjectLock(int objId) {
    return objectStatusLock->at(objId)->whoHasLock();
}

bool ThreadedTimeWarpMultiSetLTSF::isObjectScheduledBy(int threadId, int objId) {
    return (objectStatusLock->at(objId)->whoHasLock() == threadId) ? true : false;
}

void ThreadedTimeWarpMultiSetLTSF::releaseObjectLocksRecovery(int objNum) {
    if (objectStatusLock->at(objNum)->isLocked()) {
        objectStatusLock->at(objNum)->releaseLock( objectStatusLock->at(objNum)->whoHasLock(), syncMechanism);
        debug::debugout << "Releasing Object " << objNum << " during recovery." << endl;
    }
}

