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

ThreadedTimeWarpMultiSetLTSF::ThreadedTimeWarpMultiSetLTSF(int inObjectCount, int LTSFCountVal,
                                                           const string syncMech, const string scheQScheme, const string causalityType, int** inLTSFObjId) {
    objectCount = inObjectCount;
    LTSFObjId = inLTSFObjId;

    //scheduleQ scheme
    scheduleQScheme = scheQScheme;

    //Event causality type
    eventCausality = causalityType;

    // Set up scheduleQueue (LTSF queue)
    if (scheduleQScheme == "MultiSet") {
        scheduleQueue = new multiset<const Event*,receiveTimeLessThanEventIdLessThan> ;
    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            ladderQRelaxed = new LadderQueueRelaxed();
        } else {
            ladderQStrict = new LadderQueueStrict();
        }
    } else if (scheduleQScheme == "SplayTree") {
        splayTree = new SplayTree;
    } else {
        cout << "Invalid schedule queue scheme" << endl;
    }

    //objectStatusLock = new LockState *[objectCount];
    scheduleQueueLock = new LockState();

    //synchronization mechanism
    syncMechanism = syncMech;

    LTSFCount = LTSFCountVal;

    minReceiveTime = 0;

    //Initialize LTSF Event Queue
    for (int i = 0; i < objectCount; i++) {
        objectStatusLock.push_back(new LockState());
        //Schedule queue
        if (scheduleQScheme == "MultiSet") {
            lowestObjectPosition.push_back(scheduleQueue->end());
        } else if (scheduleQScheme == "LadderQueue") {
            if(eventCausality == "Relaxed") {
                lowestLadderObjectPosition.push_back(ladderQRelaxed->end());
            } else {
                lowestLadderObjectPosition.push_back(ladderQStrict->end());
            }
        } else if (scheduleQScheme == "SplayTree") {
            lowestLadderObjectPosition.push_back(splayTree->end());
        } else {
            cout << "Invalid schedule queue scheme" << endl;
        }
    }
}

ThreadedTimeWarpMultiSetLTSF::~ThreadedTimeWarpMultiSetLTSF() {
    objectStatusLock.clear();
    lowestObjectPosition.clear();
    delete scheduleQueueLock;
    if (scheduleQScheme == "MultiSet") {
        delete scheduleQueue;
    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            delete ladderQRelaxed;
        } else {
            delete ladderQStrict;
        }
    } else if (scheduleQScheme == "SplayTree") {
        delete splayTree;
    } else {
        cout << "Invalid schedule queue scheme" << endl;
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
            if (!ladderQRelaxed->empty()) {
                ret = &(ladderQRelaxed->begin()->getReceiveTime());
            }
        } else {
            this->getScheduleQueueLock(threadID);
            if (!ladderQStrict->empty()) {
                ret = &(ladderQStrict->begin()->getReceiveTime());
            }
            this->releaseScheduleQueueLock(threadID);
        }

    } else if (scheduleQScheme == "SplayTree") {
        this->getScheduleQueueLock(threadID);
        if (splayTree->peekEvent()) {
            ret = &(splayTree->peekEvent()->getReceiveTime());
        }
        this->releaseScheduleQueueLock(threadID);

    } else {
        cout << "Invalid schedule queue scheme" << endl;
    }
    return (ret);
}

// Lock based Counting -- Don't call this function in a loop
// No one uses this - possibly remove
int ThreadedTimeWarpMultiSetLTSF::getMessageCount(int threadId) {
    int count = 0;

    if (scheduleQScheme == "MultiSet") {
        getScheduleQueueLock(threadId);
        count = scheduleQueue->size();
        releaseScheduleQueueLock(threadId);

    } else if (scheduleQScheme == "LadderQueue") {
        cout << "LadderQ message count not handled for now" << endl;

    } else if (scheduleQScheme == "SplayTree") {
        getScheduleQueueLock(threadId);
        count = splayTree->size();
        releaseScheduleQueueLock(threadId);

    } else {
        cout << "Invalid schedule queue scheme" << endl;
    }

    return count;
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
    } else if (scheduleQScheme == "SplayTree") {
        return (splayTree->size() == 0) ? true : false;
    }
    throw std::runtime_error("Invalid schedule queue scheme");
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
    } else if (scheduleQScheme == "SplayTree") {
        this->getScheduleQueueLock(threadId);
        splayTree->clear();
        this->releaseScheduleQueueLock(threadId);

    } else {
        cout << "Invalid schedule queue scheme" << endl;
    }
}
void ThreadedTimeWarpMultiSetLTSF::setLowestObjectPosition(int threadId, int index) {
    if (scheduleQScheme == "MultiSet") {
        this->getScheduleQueueLock(threadId);
        lowestObjectPosition[index] = scheduleQueue->end();
        this->releaseScheduleQueueLock(threadId);

    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            lowestLadderObjectPosition[index] = ladderQRelaxed->end();
        } else {
            this->getScheduleQueueLock(threadId);
            lowestLadderObjectPosition[index] = ladderQStrict->end();
            this->releaseScheduleQueueLock(threadId);
        }

    } else if (scheduleQScheme == "SplayTree") {
        this->getScheduleQueueLock(threadId);
        lowestLadderObjectPosition[index] = splayTree->end();
        this->releaseScheduleQueueLock(threadId);

    } else {
        cout << "Invalid schedule queue scheme" << endl;
    }
}
// Removes the Event for the given Obj from the schedule queue,
//  and returns the event - used for reassigning LPs
const Event* ThreadedTimeWarpMultiSetLTSF::removeLP(int objId) {
    // Decrement number of LPs (so we know size of lowestObjectPosition
    const Event* removedEvent;
    // BUG: Update to work for other schemes...
    if (scheduleQScheme == "MultiSet") {
        if (lowestObjectPosition[objId] == scheduleQueue->end()) {
            removedEvent = NULL;
        } else {
            removedEvent = *(lowestObjectPosition[objId]);
        }
    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            if (lowestLadderObjectPosition[objId] == ladderQRelaxed->end()) {
                removedEvent = NULL;
            } else {
                removedEvent = lowestLadderObjectPosition[objId];
            }
        } else {
            if (lowestLadderObjectPosition[objId] == ladderQStrict->end()) {
                removedEvent = NULL;
            } else {
                removedEvent = lowestLadderObjectPosition[objId];
            }
        }
    } else if (scheduleQScheme == "SplayTree") {
        if (lowestLadderObjectPosition[objId] == splayTree->end()) {
            removedEvent = NULL;
        } else {
            removedEvent = lowestLadderObjectPosition[objId];
        }
    } else {
        cout << "Invalid schedule queue scheme" << endl;
    }
    eraseSkipFirst(objId);
    // We need to remap objectStatusLock
    lowestObjectPosition.erase(lowestObjectPosition.begin() + objId);
    objectStatusLock.erase(objectStatusLock.begin() + objId);
    objectCount--;
    return removedEvent;
}
// Adds an LP to the schedule queue, and returns the new mapped objId for it
int ThreadedTimeWarpMultiSetLTSF::addLP(int oldLockOwner) {
    if (scheduleQScheme == "MultiSet") {
        lowestObjectPosition.push_back(scheduleQueue->end());
    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            lowestLadderObjectPosition.push_back(ladderQRelaxed->end());
        } else {
            lowestLadderObjectPosition.push_back(ladderQStrict->end());
        }
    } else if (scheduleQScheme == "SplayTree") {
        lowestLadderObjectPosition.push_back(splayTree->end());
    } else {
        cout << "Invalid schedule queue scheme" << endl;
    }

    objectStatusLock.push_back(new LockState());
    if (oldLockOwner != -1) { // -1 if not locked
        getObjectLock(oldLockOwner, objectCount);
        cout << "Obtaining object lock on " << objectCount << endl;
    }

    // Expand array sizes
    return ++objectCount;
}
void ThreadedTimeWarpMultiSetLTSF::insertEvent(int objId, const Event* newEvent) {
    if (scheduleQScheme == "MultiSet") {
        ASSERT(newEvent);
        lowestObjectPosition[objId] = scheduleQueue->insert(newEvent);
    } else if (scheduleQScheme == "LadderQueue") {
        ASSERT(newEvent);
        if(eventCausality == "Relaxed") {
            lowestLadderObjectPosition[objId] = ladderQRelaxed->insert(newEvent);
        } else {
            debug::debugout<<"Inserting " << newEvent << "." <<endl;
            lowestLadderObjectPosition[objId] = ladderQStrict->insert(newEvent);
        }
    } else if (scheduleQScheme == "SplayTree") {
        ASSERT(newEvent);
        splayTree->insert(newEvent);
        lowestLadderObjectPosition[objId] = newEvent;
    } else {
        cout << "Invalid schedule queue scheme" << endl;
    }
}
void ThreadedTimeWarpMultiSetLTSF::insertEmptyEvent(int objId) {
    if (scheduleQScheme == "MultiSet") {
        lowestObjectPosition[objId] = scheduleQueue->end();
    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            lowestLadderObjectPosition[objId] = ladderQRelaxed->end();
        } else {
            lowestLadderObjectPosition[objId] = ladderQStrict->end();
        }
    } else if (scheduleQScheme == "SplayTree") {
        lowestLadderObjectPosition[objId] = splayTree->end();
    } else {
        cout << "Invalid schedule queue scheme" << endl;
    }
}
void ThreadedTimeWarpMultiSetLTSF::eraseSkipFirst(int objId) {
    // Do not erase the first time.
    if (scheduleQScheme == "MultiSet") {
        if (lowestObjectPosition[objId] != scheduleQueue->end()) {
            scheduleQueue->erase(lowestObjectPosition[objId]);
        } else {
            /*debug::debugout << "( "
             << mySimulationManager->getSimulationManagerID()
             << " ) Object - " << objId
             << " is returned for schedule thro' insert by the thread - "
             << threadId << "\n";*/
        }
    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            if (lowestLadderObjectPosition[objId] != ladderQRelaxed->end()) {
                ladderQRelaxed->erase(lowestLadderObjectPosition[objId]);
            }
        } else {
            if (lowestLadderObjectPosition[objId] != ladderQStrict->end()) {
                ladderQStrict->erase(lowestLadderObjectPosition[objId]);
            }
        }
    } else if (scheduleQScheme == "SplayTree") {
        if (lowestLadderObjectPosition[objId] != splayTree->end()) {
            splayTree->erase(lowestLadderObjectPosition[objId]);
        }
    } else {
        cout << "Invalid schedule queue scheme" << endl;
    }
}
int ThreadedTimeWarpMultiSetLTSF::getScheduleQueueSize() {
    if (scheduleQScheme == "MultiSet") {
        return scheduleQueue->size();
    } else if (scheduleQScheme == "LadderQueue") {
        cout << "LadderQ message count not handled for now" << endl;
    } else if (scheduleQScheme == "SplayTree") {
        return splayTree->size();
    }
    throw std::runtime_error("Invalid schedule queue scheme");
}
const Event* ThreadedTimeWarpMultiSetLTSF::peek(int threadId) {
    const Event* ret = NULL;

    if (scheduleQScheme == "MultiSet") {
        this->getScheduleQueueLock(threadId);
        if (scheduleQueue->size() > 0) {
            debug::debugout<<"( "<< threadId << " T ) Peeking from Schedule Queue"<<endl;

            ret = *(scheduleQueue->begin());
            unsigned int objId = LTSFObjId[ret->getReceiver().getSimulationObjectID()][0];
            debug::debugout <<" ( "<< threadId << ") Locking the Object " <<objId <<endl;

            getObjectLock(threadId, objId);
            //remove the object out of schedule
            scheduleQueue->erase(scheduleQueue->begin());
            //set the indexer/pointer to NULL
            lowestObjectPosition[objId] = scheduleQueue->end();
            /*      debug::debugout << "( "
            << mySimulationManager->getSimulationManagerID()
            << " ) Object - " << objId
            << " is removed out for schedule by the thread - "
            << threadId << "\n";*/
        }
        this->releaseScheduleQueueLock(threadId);

    } else if (scheduleQScheme == "LadderQueue") {
        if(eventCausality == "Relaxed") {
            this->getScheduleQueueLock(threadId);
            if (!ladderQRelaxed->empty()) {
                debug::debugout<<"( "<< threadId << " T ) Peeking from Schedule Queue"<<endl;
                if( !(ret = ladderQRelaxed->dequeue()) ) {
                    this->releaseScheduleQueueLock(threadId);
                    return NULL;
                }
                unsigned int objId = LTSFObjId[ret->getReceiver().getSimulationObjectID()][0];
                lowestLadderObjectPosition[objId] = NULL;
                debug::debugout<<"Dequeued " << ret << ". "<< endl;
                debug::debugout <<" ( "<< threadId << ") Locking the Object " <<objId <<endl;
                this->getObjectLock(threadId, objId);
            }
            this->releaseScheduleQueueLock(threadId);
        } else {
            this->getScheduleQueueLock(threadId);
            if (!ladderQStrict->empty()) {
                debug::debugout<<"( "<< threadId << " T ) Peeking from Schedule Queue"<<endl;
                ret = ladderQStrict->dequeue();
                if (ret == NULL) {
                    cout << "dequeue() func returned NULL" << endl;
                    return ret;
                }
                debug::debugout<<"Dequeued " << ret << ". "<< endl;
                unsigned int objId = LTSFObjId[ret->getReceiver().getSimulationObjectID()][0];

                debug::debugout <<" ( "<< threadId << ") Locking the Object " <<objId <<endl;

                this->getObjectLock(threadId, objId);

                //set the indexer/pointer to NULL
                lowestLadderObjectPosition[objId] = ladderQStrict->end();
            }
            this->releaseScheduleQueueLock(threadId);
        }

    } else if (scheduleQScheme == "SplayTree") {
        this->getScheduleQueueLock(threadId);
        if (splayTree->peekEvent()) {
            debug::debugout<<"( "<< threadId << " T ) Peeking from Schedule Queue"<<endl;
            if (NULL == (ret = splayTree->getEvent())) {
                cout << "getEvent() func returned NULL" << endl;
                this->releaseScheduleQueueLock(threadId);
                return ret;
            }
            unsigned int objId = LTSFObjId[ret->getReceiver().getSimulationObjectID()][0];

            debug::debugout <<" ( "<< threadId << ") Locking the Object " <<objId <<endl;

            this->getObjectLock(threadId, objId);

            //set the indexer/pointer to NULL
            lowestLadderObjectPosition[objId] = splayTree->end();
        }
        this->releaseScheduleQueueLock(threadId);

    } else {
        cout << "Invalid schedule queue scheme" << endl;
    }

    return ret;
}

void ThreadedTimeWarpMultiSetLTSF::getObjectLock(int threadId, int objId) {
    objectStatusLock[objId]->setLock(threadId, syncMechanism);
    /*  debug::debugout << "( " << mySimulationManager->getSimulationManagerID()
     << " ) Object - " << objId << " is Locked by the thread - "
     << threadId << "\n";*/
    ASSERT(objectStatusLock[objId]->hasLock(threadId, syncMechanism));
}
void ThreadedTimeWarpMultiSetLTSF::releaseObjectLock(int threadId, int objId) {
    ASSERT(objectStatusLock[objId]->hasLock(threadId, syncMechanism));
    objectStatusLock[objId]->releaseLock(threadId, syncMechanism);
    /*  debug::debugout << "( " << mySimulationManager->getSimulationManagerID()
     << " ) Object - " << objId << " is Released by the thread - "
     << threadId << "\n";*/
}

bool ThreadedTimeWarpMultiSetLTSF::isObjectScheduled(int objId) {
    return objectStatusLock[objId]->isLocked();
}
// Returns owner of object lock - used when transferring
int ThreadedTimeWarpMultiSetLTSF::whoHasObjectLock(int objId) {
    return objectStatusLock[objId]->whoHasLock();
}
bool ThreadedTimeWarpMultiSetLTSF::isObjectScheduledBy(int threadId, int objId) {
    return (objectStatusLock[objId]->whoHasLock() == threadId) ? true : false;
}
void ThreadedTimeWarpMultiSetLTSF::releaseObjectLocksRecovery(int objNum) {
    if (objectStatusLock[objNum]->isLocked()) {
        objectStatusLock[objNum]->releaseLock(
            objectStatusLock[objNum]->whoHasLock(),
            syncMechanism);
        debug::debugout << "Releasing Object " << objNum
                        << " during recovery." << endl;
    }
}
