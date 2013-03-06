// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedTimeWarpMultiSet.h"
#include "SimulationObject.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "ThreadedTimeWarpMultiSetSchedulingManager.h"
#include "StragglerEvent.h"
#include "ThreadedTimeWarpMultiSetLTSF.h"

class NegativeEvent;
using std::cout;
using std::cerr;
using std::endl;

ThreadedTimeWarpMultiSet::ThreadedTimeWarpMultiSet(
		ThreadedTimeWarpSimulationManager* initSimulationManager) {
	//Input queues
	objectCount = initSimulationManager->getNumberOfSimulationObjects();

	//synchronization mechanism
	syncMechanism = initSimulationManager->getSyncMechanism();

	unprocessedQueueLockState = new LockState *[objectCount];
	processedQueueLockState = new LockState *[objectCount];
	removedQueueLockState = new LockState *[objectCount];

	int threadCount = initSimulationManager->getNumberofThreads();
	scheduleQScheme = initSimulationManager->getScheduleQScheme();

	LTSFCount = initSimulationManager->getScheduleQCount();

	LTSF = new ThreadedTimeWarpMultiSetLTSF *[LTSFCount];

	// Assert whether object count greater than or equal to LTSF queue count
	ASSERT(objectCount >= LTSFCount);

	// Assert whether thread count greater than or equal  to LTSF queue count
	ASSERT(threadCount >= LTSFCount);

	// Initialize schedule queues / setup lookup table to associate unprocessed
	// objId with schedule queue 
	for (int i=0; i < LTSFCount; i++) {
		if(i < objectCount % LTSFCount) {
			LTSF[i] = new ThreadedTimeWarpMultiSetLTSF(objectCount/LTSFCount + 1, LTSFCount, syncMechanism, scheduleQScheme);
		} else {
			LTSF[i] = new ThreadedTimeWarpMultiSetLTSF(objectCount/LTSFCount, LTSFCount, syncMechanism, scheduleQScheme);
		}
	}

	LTSFByThread = new ThreadedTimeWarpMultiSetLTSF *[threadCount];
	LTSFByObj = new ThreadedTimeWarpMultiSetLTSF *[objectCount];
	LTSFObjId = new int *[objectCount]; 

	// Assign threads to LTSF queues
	for (int i=0; i < threadCount; i++) {
		LTSFByThread[i] = LTSF[ i % LTSFCount ]; 
	}

	// Warning message if uneven distribution of threads to LTSF queues
	if ( (threadCount % LTSFCount ) != 0 ) {
		cout << "Threads unevenly distributed amongst LTSF queues" << endl;
	}

	//Iterators for each threads
	vectorIterator = new vIterate[threadCount];
	multisetIterator = new mIterate[threadCount];

	//Initializing Unprocessed Event Queue
	for (int i = 0; i < objectCount; i++) {
		multiset<const Event*, receiveTimeLessThanEventIdLessThan> *objSet =
				new multiset<const Event*, receiveTimeLessThanEventIdLessThan> ;
		unProcessedQueue.push_back(objSet);
		processedQueue.push_back(new vector<const Event*> );
		removedEventQueue.push_back(new vector<const Event*> );

		//Initialzing Locks for each Object
		unprocessedQueueLockState[i] = new LockState();
		processedQueueLockState[i] = new LockState();
		removedQueueLockState[i] = new LockState();

		//Create lookup table to associate between an unprocessed queue id
		// and the appropriate LTSF queue 
		LTSFByObj[i] = LTSF[ i % LTSFCount ];
		LTSFObjId[i] = new int[2];
		LTSFObjId[i][OBJID] = i / LTSFCount;
		LTSFObjId[i][LTSFOWNER] = i % LTSFCount;
		
		
		//LTSFByObj[i] = LTSF[ i / (objectCount / LTSFCount) ];
		//cout << i << ":" << endl << "LTSF " << i/(objectCount/LTSFCount) << endl;
		//LTSFObjId[i] = i % (objectCount / LTSFCount);
		//cout << "LTSFObjId = " << LTSFObjId[i] << endl;
	}
	mySimulationManager = initSimulationManager;

}

ThreadedTimeWarpMultiSet::~ThreadedTimeWarpMultiSet() {
	//freeing Unprocessed Event Queue
	multiset<const Event*, receiveTimeLessThanEventIdLessThan> *deleteMultiSet =
			NULL;
	while (!unProcessedQueue.empty()) {
		deleteMultiSet = unProcessedQueue.back();
		delete deleteMultiSet;
	}

	vector<const Event*> *toDeleteVector = NULL;
	while (!processedQueue.empty()) {
		toDeleteVector = processedQueue.back();
		processedQueue.pop_back();
		delete toDeleteVector;
	}
	delete unprocessedQueueLockState;
	delete processedQueueLockState;
	delete removedQueueLockState;
	////delete objectStatusLock;
	//	deleting each Threads Iterator
	delete vectorIterator;
	delete multisetIterator;
}

// Moves the given LP to the new LP
// BUG: If thread accesses schedule queue during reassignment, it will
// get the old LTSF queue (when it uses LTSFByObj)
void ThreadedTimeWarpMultiSet::moveLP(int sourceObj, int destLTSF) {
	// Update LTSFByOBJ - here, or after locking?
	// Also update LTSFObjId
	//cout << "Moving " << sourceObj << " from " << LTSFObjId[sourceObj][LTSFOWNER] << " to " << destLTSF << endl;

	// Lock LTSF Destination
	LTSF[destLTSF]->getScheduleQueueLock(0);
	ThreadedTimeWarpMultiSetLTSF* sourceLTSF = LTSFByObj[sourceObj];
	LTSFByObj[sourceObj] = LTSF[destLTSF];
	// Lock LTSF Source
	sourceLTSF->getScheduleQueueLock(0);

	// Copy and Delete Event from source LTSF
	int mappedSourceId = LTSFObjId[sourceObj][OBJID];
	int removedLockOwner = sourceLTSF->whoHasObjectLock(mappedSourceId);
	const Event* removedEvent = sourceLTSF->removeLP(mappedSourceId);

	// Shift all objId mappings after removedLP back one
	int sourceLTSFOwner = LTSFObjId[sourceObj][LTSFOWNER];
	for (int i = 0; i<objectCount; i++) {
		if (LTSFObjId[i][LTSFOWNER] == sourceLTSFOwner) {
			if (LTSFObjId[i][OBJID] > mappedSourceId) {
				LTSFObjId[i][OBJID]--;
			}
		}
	}

	// Insert Event into destination LTSF - subtract one since it is an array
	LTSFObjId[sourceObj][OBJID] = LTSF[destLTSF]->addLP(removedLockOwner) - 1;
	LTSFObjId[sourceObj][LTSFOWNER] = destLTSF;
	if (removedEvent != NULL) {
		//cout << "removedEvent = " << *removedEvent << endl;
		LTSF[destLTSF]->insertEvent(LTSFObjId[sourceObj][OBJID], removedEvent);
	}

	// Unlock LTSF Destination
	LTSF[destLTSF]->releaseScheduleQueueLock(0);
	// Unlock LTSF Source
	sourceLTSF->releaseScheduleQueueLock(0);
	cout << "LP Swap Completed " << sourceObj << " moved to " << destLTSF << endl;
}

bool ThreadedTimeWarpMultiSet::threadHasUnprocessedQueueLock(int threadId,
		int objId) {
	return (unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism));
}

void ThreadedTimeWarpMultiSet::getunProcessedLock(int threadId, int objId) {
	while (!unprocessedQueueLockState[objId]->setLock(threadId, syncMechanism));
	ASSERT(unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism));
}
void ThreadedTimeWarpMultiSet::releaseunProcessedLock(int threadId, int objId) {
	ASSERT(unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism));
	unprocessedQueueLockState[objId]->releaseLock(threadId, syncMechanism);
}
void ThreadedTimeWarpMultiSet::getProcessedLock(int threadId, int objId) {
	while (!processedQueueLockState[objId]->setLock(threadId, syncMechanism))
		;
	ASSERT(processedQueueLockState[objId]->hasLock(threadId, syncMechanism));
}
void ThreadedTimeWarpMultiSet::releaseProcessedLock(int threadId, int objId) {
	ASSERT(processedQueueLockState[objId]->hasLock(threadId, syncMechanism));
	processedQueueLockState[objId]->releaseLock(threadId, syncMechanism);
}
void ThreadedTimeWarpMultiSet::getremovedLock(int threadId, int objId) {
	while (!removedQueueLockState[objId]->setLock(threadId, syncMechanism))
		;
	ASSERT(removedQueueLockState[objId]->hasLock(threadId, syncMechanism));
}
void ThreadedTimeWarpMultiSet::releaseremovedLock(int threadId, int objId) {
	ASSERT(removedQueueLockState[objId]->hasLock(threadId, syncMechanism));
	removedQueueLockState[objId]->releaseLock(threadId, syncMechanism);
}

bool ThreadedTimeWarpMultiSet::isObjectScheduled(int objId) {
	return LTSFByObj[objId]->isObjectScheduled(LTSFObjId[objId][OBJID]);
}

bool ThreadedTimeWarpMultiSet::isObjectScheduledBy(int threadId, int objId) {
	return LTSFByObj[objId]->isObjectScheduledBy(threadId, LTSFObjId[objId][OBJID]);
}

//not thread Safe
int ThreadedTimeWarpMultiSet::getQueueEventCount(int objId) {
	int size;
	size = unProcessedQueue[objId]->size();
	return size;
}
//This Function will be called by the worker when the object has been scheduled, so no need to update schedule queue(need to verify this)
const Event* ThreadedTimeWarpMultiSet::getEvent(SimulationObject *simObj,
		int threadId) {
	const Event* ret = NULL;
	unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
	if (!this->unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism))
		this->getunProcessedLock(threadId, objId);
	if (getQueueEventCount(objId) > 0) {
		//Remove from Unprocessed Queue
		ret = *(unProcessedQueue[objId]->begin());
		//Return NULL if ret is a Straggler/Negative
		if (dynamic_cast<const StragglerEvent*> (ret) || ret->getReceiveTime()
				< simObj->getSimulationTime()) {
			this->releaseunProcessedLock(threadId, objId);
			return NULL;
		}
		unProcessedQueue[objId]->erase(unProcessedQueue[objId]->begin());
		this->releaseunProcessedLock(threadId, objId);
		//Insert into Processed Queue
		if (dynamic_cast<const StragglerEvent*> (ret))
			ASSERT(false);
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
		SimulationObject *simObj, int threadId) {
	const Event* ret = NULL;
	unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
	if (getQueueEventCount(objId) > 0) {
		//Remove from Unprocessed Queue
		ret = *(unProcessedQueue[objId]->begin());
		//Return NULL if ret is a Straggler/Negative
		if (dynamic_cast<const StragglerEvent*> (ret) || ret->getReceiveTime()
				< simObj->getSimulationTime()) {
			this->releaseunProcessedLock(threadId, objId);
			return NULL;
		}
		unProcessedQueue[objId]->erase(unProcessedQueue[objId]->begin());
		//Insert into Processed Queue
		if (dynamic_cast<const StragglerEvent*> (ret))
			ASSERT(false);
		this->getProcessedLock(threadId, objId);
		processedQueue[objId]->push_back(ret);
		this->releaseProcessedLock(threadId, objId);
	}
	//ASSERT(this->isObjectScheduledBy(threadId, objId));
	return ret;
}
const Event* ThreadedTimeWarpMultiSet::getEventIfStraggler(
		SimulationObject *simObj, int threadId) {
	const Event* ret = NULL;
	unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
	this->getunProcessedLock(threadId, objId);
	if (getQueueEventCount(objId) > 0) {
		//Remove from Unprocessed Queue
		ret = (*unProcessedQueue[objId]->begin());
		if (dynamic_cast<const StragglerEvent*> (ret)) {
			unProcessedQueue[objId]->erase(unProcessedQueue[objId]->begin());
		} else if (ret->getReceiveTime() >= simObj->getSimulationTime()) {
			this->releaseunProcessedLock(threadId, objId);
			ret = NULL;
		}
	} else
		this->releaseunProcessedLock(threadId, objId);

	//ASSERT(this->isObjectScheduledBy(threadId, objId));

	return ret;
}
const Event *ThreadedTimeWarpMultiSet::getEvent(SimulationObject *simObj,
		const VTime &minimumTime, int threadId) {
	const Event *retval = NULL;
	ASSERT( simObj != NULL );
	const Event *peeked = peekEvent(simObj, threadId);
	if (peeked != NULL) {
		retval = getEventWhileRollback(simObj, threadId);
		ASSERT( peeked == retval );
	}
	return retval;
}

const VTime* ThreadedTimeWarpMultiSet::nextEventToBeScheduledTime(int threadId) {
	const VTime* minimum = NULL;
	const VTime* temp;
	// Iterate through all schedule queues, and find lowest item
	for (int i=0; i<LTSFCount; i++) {
		temp = (LTSF[i]->nextEventToBeScheduledTime(threadId));
		if ( (minimum == NULL) || ( (temp != NULL) && (*temp < *minimum) ) )
			minimum = temp;
	}
	return minimum;
}

const Event* ThreadedTimeWarpMultiSet::peekEvent(SimulationObject *simObj,
		int threadId) {
	const Event* ret = NULL;
	
	bool releaseWhileReturn = true;
	SimulationObject *simObject = NULL;
	if (simObj == NULL) {
		ret = LTSFByThread[threadId-1]->peekIt(threadId, LTSFObjId);
	} else if (simObj != NULL) {
		unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
		if (!this->unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism)) {
			this->getunProcessedLock(threadId, objId);
			releaseWhileReturn = false;
		}
		ret = *(unProcessedQueue[objId]->begin());
		if (dynamic_cast<const StragglerEvent*> (ret)) {
			if (!releaseWhileReturn)
				this->releaseunProcessedLock(threadId, objId);
			return NULL;
		}
		if (!releaseWhileReturn)
			this->releaseunProcessedLock(threadId, objId);
	}

	return ret;
}

const Event* ThreadedTimeWarpMultiSet::peekEventCoastForward(
		SimulationObject *simObj, int threadId) {
	const Event* ret = NULL;
	unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
	this->getunProcessedLock(threadId, objId);
	if (getQueueEventCount(objId) > 0) {
		ret = *(unProcessedQueue[objId]->begin());
		if (dynamic_cast<const StragglerEvent*> (ret) || ret->getReceiveTime()
				< simObj->getSimulationTime()) {
			this->releaseunProcessedLock(threadId, objId);
			return NULL;
		}
	} else
		this->releaseunProcessedLock(threadId, objId);

	return ret;
}

const Event *ThreadedTimeWarpMultiSet::peekEvent(SimulationObject *simObj,
		const VTime &minimumTime, int threadId) {
	const Event *retval = peekEvent(simObj, threadId);
	if (retval != NULL) {
		if (!(retval->getReceiveTime() < minimumTime)) {
			retval = NULL;
		}
	}
	return retval;
}
const Event *ThreadedTimeWarpMultiSet::peekEventLockUnprocessed(
		SimulationObject *simObj, const VTime &minimumTime, int threadId) {
	const Event *retval = peekEventLockUnprocessed(simObj, threadId);
	if (retval != NULL) {
		if (!(retval->getReceiveTime() < minimumTime)) {
			retval = NULL;
			unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
			releaseunProcessedLock(threadId, objId);
		}
	}
	return retval;
}
bool ThreadedTimeWarpMultiSet::insert(const Event *receivedEvent, int threadId) {
	unsigned int objId = receivedEvent->getReceiver().getSimulationObjectID();
	this->getunProcessedLock(threadId, objId);
	unProcessedQueue[objId]->insert(receivedEvent);
	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
			<< " ) " << mySimulationManager->getObjectHandle(
			receivedEvent->getReceiver())->getName() << " has received ::::"
			<< *receivedEvent << " - " << threadId << "\n";
	multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator itee;
	itee = unProcessedQueue[objId]->begin();
	// The event was just inserted at the beginning, so update the Schedule Queue
	if (receivedEvent == *(itee)) {
		LTSFByObj[objId]->getScheduleQueueLock(threadId);
		if (!this->isObjectScheduled(objId)) {
			LTSFByObj[objId]->eraseSkipFirst(LTSFObjId[objId][OBJID]);
			LTSFByObj[objId]->insertEvent(LTSFObjId[objId][OBJID], receivedEvent);
		}
		LTSFByObj[objId]->releaseScheduleQueueLock(threadId);
	}

	this->releaseunProcessedLock(threadId, objId);
	//ASSERT( LTSFByObj[objId]->getScheduleQueueSize() <= mySimulationManager->getNumberOfSimulationObjects() );
	//return false;
}

bool ThreadedTimeWarpMultiSet::isScheduleQueueEmpty(int ltsfIndex) {
	if(ltsfIndex < LTSFCount) {
		return LTSF[ltsfIndex]->isScheduleQueueEmpty();
	} else if (ltsfIndex == LTSFCount) { // simulation termination check condition
		bool isEmpty = true;

		for(unsigned int index = 0; (index < LTSFCount) && (isEmpty); index++) { // check all schedule queues
			isEmpty &= LTSF[index]->isScheduleQueueEmpty();
		}
		return isEmpty;
	} else {
		ASSERT(false);
	}
}

bool ThreadedTimeWarpMultiSet::handleAntiMessage(SimulationObject *simObj,
		const NegativeEvent* negativeEvent, int threadId) {
	bool eventWasRemoved = false;
	unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
	if (!this->unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism))
		this->getunProcessedLock(threadId, objId);
	multisetIterator[threadId] = unProcessedQueue[objId]->begin();

	while (multisetIterator[threadId] != unProcessedQueue[objId]->end()
			&& !eventWasRemoved) {
		if ((*(multisetIterator[threadId]))->getSender()
				== negativeEvent->getSender()
				&& ((*(multisetIterator[threadId]))->getEventId()
						== negativeEvent->getEventId())) {
			const Event *eventToRemove = *multisetIterator[threadId];
			if (dynamic_cast<const StragglerEvent*> (*(multisetIterator[threadId]))) {
				utils::debug
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

void ThreadedTimeWarpMultiSet::rollback(SimulationObject *simObj,
		const VTime &rollbackTime, int threadId) {
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
		const unsigned int
				eventIdRollback =
						mySimulationManager->getStateManagerNew()->getEventIdForRollback(
								threadId, objId);
		const unsigned int
				senderObjectId =
						mySimulationManager->getStateManagerNew()->getSenderObjectIdForRollback(
								threadId, objId);
		const unsigned int
				senderObjectSimId =
						mySimulationManager->getStateManagerNew()->getSenderObjectSimIdForRollback(
								threadId, objId);
		//	cout << "The saved EventId is --------------------->>>>>>>>>>>> : "
		//			<< eventIdRollback << endl;
		//	cout << "The First EventId is --------------------->>>>>>>>>>>> : "
		//			<< (*(vectorIterator[threadId]))->getEventId() << endl;
		//	cout << "The saved SenderObjectId is --------------------->>>>>>>>>>>> : "
		//			<< senderObjectId << endl;
		while (vectorIterator[threadId] != processedQueue[objId]->end()) {
			EventId tempEventId = (*(vectorIterator[threadId]))->getEventId();
			unsigned int
					tempSenderObjectId =
							(*(vectorIterator[threadId]))->getSender().getSimulationObjectID();
			if (tempEventId.getEventNum() != eventIdRollback
					|| tempSenderObjectId != senderObjectId) {
				/*cout << "Skipping Event.......::::::::::::: "
				 << **(vectorIterator[threadId]) << endl;*/
				(vectorIterator[threadId])++;
				tempCount++;
			} else {
				/*			cout << " Matched EventId ::::" << **(vectorIterator[threadId])
				 << endl;*/
				break;
			}
		}
		tempCount = processedQueue[objId]->size() - tempCount;
	}
	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
			<< " ) Object - " << objId << " Rollback returns : " << tempCount
			<< " events back to Unprocessed Queue - " << threadId << endl;
	unProcessedQueue[objId]->insert(vectorIterator[threadId],
			processedQueue[objId]->end());
	processedQueue[objId]->erase(vectorIterator[threadId],
			processedQueue[objId]->end());
	this->releaseProcessedLock(threadId, objId);

}
void ThreadedTimeWarpMultiSet::fossilCollect(SimulationObject *simObj,
		const VTime &fossilCollectTime, int threadId) {
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
			const Event *eventToReclaim = *(vectorIterator[threadId]);
			vectorIterator[threadId] = removedEventQueue[objId]->erase(
					vectorIterator[threadId]);
			simObj->reclaimEvent(eventToReclaim);
			//	delete eventToReclaim;//Replace this by a reclaim function from object
		} else {
			vectorIterator[threadId]++;
		}
	}
	this->releaseremovedLock(threadId, objId);

}
void ThreadedTimeWarpMultiSet::fossilCollect(SimulationObject *object,
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
			const Event *eventToReclaim = *(vectorIterator[threadId]);
			vectorIterator[threadId] = removedEventQueue[objId]->erase(
					vectorIterator[threadId]);
			object->reclaimEvent(eventToReclaim);
		} else {
			vectorIterator[threadId]++;
		}
	}
	this->releaseremovedLock(threadId, objId);
}

void ThreadedTimeWarpMultiSet::fossilCollect(const Event *toRemove,
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
	//ASSERT(this->isObjectScheduledBy(threadId, objId));

	if (!this->unprocessedQueueLockState[objId]->hasLock(threadId, syncMechanism))
		this->getunProcessedLock(threadId, objId);
	LTSFByObj[objId]->getScheduleQueueLock(threadId);
	if (unProcessedQueue[objId]->size() > 0) {
		firstEvent = *(unProcessedQueue[objId]->begin());
	}

	// Check that lowest object position for this objId is scheduleQueue->end
	
	if (firstEvent != NULL) {
		LTSFByObj[objId]->insertEvent(LTSFObjId[objId][OBJID], firstEvent);
	} else {
		LTSFByObj[objId]->insertEventEnd(LTSFObjId[objId][OBJID]);
	}

	utils::debug <<" ( "<< threadId << ") Returning object " <<objId <<" back to SCheQ"<<endl;

	LTSFByObj[objId]->releaseObjectLock(threadId, LTSFObjId[objId][OBJID]);
	LTSFByObj[objId]->releaseScheduleQueueLock(threadId);
	this->releaseunProcessedLock(threadId, objId);
}

//Dont Know, Who call this function. Its not completely tested
bool ThreadedTimeWarpMultiSet::inThePast(const Event *toCheck, int threadId) {
	unsigned int objId = toCheck->getReceiver().getSimulationObjectID();
	bool retval = false;
	this->getProcessedLock(threadId, objId);
	if (!processedQueue[objId]->empty()) {
		// Events are pushed on to the back of the processed events vector.
		// Because they can only be pushed on in order, the vector is always sorted.
		const Event *lastProc = processedQueue[objId]->back();

		if (lastProc != NULL) {
			if (toCheck->getReceiveTime() != lastProc->getReceiveTime()) {
				retval = toCheck->getReceiveTime() < lastProc->getReceiveTime();
			} else {
				if (toCheck->getEventId() != lastProc->getEventId()) {
					retval = toCheck->getEventId() < lastProc->getEventId();
				} else {
					retval = toCheck->getSender() < lastProc->getSender();
				}
			}
		}
	}
	this->releaseProcessedLock(threadId, objId);
	return retval;
}
void ThreadedTimeWarpMultiSet::ofcPurge(int threadId) {
	multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator msit;

	for(int index = 0; index < LTSFCount; index++) {
		LTSF[index]->clearScheduleQueue(threadId);
	}
	for (int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++) {
		this->getunProcessedLock(threadId, i);
		msit = unProcessedQueue[i]->begin();
		while (msit != unProcessedQueue[i]->end()) {
			(*msit)->~Event();
			::operator delete((void*) (*msit));
			unProcessedQueue[i]->erase(msit++);
		}
		this->releaseunProcessedLock(threadId, i);
		LTSFByObj[i]->setLowestObjectPosition(threadId, LTSFObjId[i][OBJID]);
	}
	for (int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++) {
		this->getProcessedLock(threadId, i);
		vector<const Event*>::iterator ip = processedQueue[i]->begin();
		while (ip != processedQueue[i]->end()) {
			(*ip)->~Event();
			::operator delete((void*) (*ip));
			ip++;
		}
		processedQueue[i]->clear();
		this->releaseProcessedLock(threadId, i);
	}

	for (int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++) {
		vector<const Event*>::iterator ir = removedEventQueue[i]->begin();
		while (ir != removedEventQueue[i]->end()) {
			(*ir)->~Event();
			::operator delete((void*) (*ir));
			ir++;
		}
		removedEventQueue[i]->clear();
	}
}
const Event* ThreadedTimeWarpMultiSet::peekEventLockUnprocessed(
		SimulationObject *simObj, int threadId) {
	const Event* ret = NULL;
	SimulationObject *simObject = NULL;
	ASSERT(simObj != NULL);
	unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
	this->getunProcessedLock(threadId, objId);
	if (getQueueEventCount(objId) > 0) {
		ret = *(unProcessedQueue[objId]->begin());
		if (dynamic_cast<const StragglerEvent*> (ret) || ret->getReceiveTime()
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
	for (int objNum = 0; objNum
			< mySimulationManager->getNumberOfSimulationObjects(); objNum++) {
		for (int i = 0; i<LTSFCount; i++) {
			LTSF[i]->releaseObjectLocksRecovery(LTSFObjId[objNum][OBJID]);
		}
		if (unprocessedQueueLockState[objNum]->isLocked()) {
			unprocessedQueueLockState[objNum]->releaseLock(
									unprocessedQueueLockState[objNum]->whoHasLock(),
									syncMechanism);
			utils::debug << "Releasing Unprocessed Queue " << objNum
					<< " during recovery." << endl;
		}
		if (processedQueueLockState[objNum]->isLocked()) {
			processedQueueLockState[objNum]->releaseLock(
									processedQueueLockState[objNum]->whoHasLock(),
									syncMechanism);
			utils::debug << "Releasing Processed Queue " << objNum
					<< " during recovery." << endl;
		}
		if (removedQueueLockState[objNum]->isLocked()) {
			removedQueueLockState[objNum]->releaseLock(
									removedQueueLockState[objNum]->whoHasLock(),
									syncMechanism);
			utils::debug << "Releasing Removed Queue " << objNum
					<< " during recovery." << endl;
		}
	}
	for (int i = 0; i<LTSFCount; i++) {
		LTSF[i]->releaseAllScheduleQueueLocks();
	}
}
