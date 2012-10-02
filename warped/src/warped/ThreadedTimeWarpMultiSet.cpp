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

	unprocessedQueueAtomicState = new AtomicState *[objectCount];
	processedQueueAtomicState = new AtomicState *[objectCount];
	removedQueueAtomicState = new AtomicState *[objectCount];

	//Schedule Queue (LTSF)
	LTSF1 = new ThreadedTimeWarpMultiSetLTSF(objectCount);
	////scheduleQueue = new multiset<const Event*,
	////		receiveTimeLessThanEventIdLessThan> ;
	////objectStatusLock = new AtomicState *[objectCount];
	////scheduleQueueLock = new AtomicState();
	int threadCount = initSimulationManager->getNumberofThreads();
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
		//Initialzing Atomic Locks for each Object
		unprocessedQueueAtomicState[i] = new AtomicState();
		processedQueueAtomicState[i] = new AtomicState();
		removedQueueAtomicState[i] = new AtomicState();
		////objectStatusLock[i] = new AtomicState();
		//Schedule queue
		////lowestObjectPosition.push_back(scheduleQueue->end());
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
	delete unprocessedQueueAtomicState;
	delete processedQueueAtomicState;
	delete removedQueueAtomicState;
	////delete objectStatusLock;
	//	deleting each Threads Iterator
	delete vectorIterator;
	delete multisetIterator;
}

bool ThreadedTimeWarpMultiSet::threadHasUnprocessedQueueLock(int threadId,
		int objId) {
	return (unprocessedQueueAtomicState[objId]->hasLock(threadId));
}

void ThreadedTimeWarpMultiSet::getunProcessedLock(int threadId, int objId) {
	while (!unprocessedQueueAtomicState[objId]->setLock(threadId))
		;
	/*	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
	 << " ) Queue - " << objId << " is Locked by the thread - "
	 << threadId << "\n";*/
	ASSERT(unprocessedQueueAtomicState[objId]->hasLock(threadId));
}
void ThreadedTimeWarpMultiSet::releaseunProcessedLock(int threadId, int objId) {
	ASSERT(unprocessedQueueAtomicState[objId]->hasLock(threadId));
	unprocessedQueueAtomicState[objId]->releaseLock(threadId);
	/*	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
	 << " ) Queue - " << objId << " is Released by the thread - "
	 << threadId << "\n";*/
}
void ThreadedTimeWarpMultiSet::getProcessedLock(int threadId, int objId) {
	while (!processedQueueAtomicState[objId]->setLock(threadId))
		;
	ASSERT(processedQueueAtomicState[objId]->hasLock(threadId));
}
void ThreadedTimeWarpMultiSet::releaseProcessedLock(int threadId, int objId) {
	ASSERT(processedQueueAtomicState[objId]->hasLock(threadId));
	processedQueueAtomicState[objId]->releaseLock(threadId);
}
void ThreadedTimeWarpMultiSet::getremovedLock(int threadId, int objId) {
	while (!removedQueueAtomicState[objId]->setLock(threadId))
		;
	ASSERT(removedQueueAtomicState[objId]->hasLock(threadId));
}
void ThreadedTimeWarpMultiSet::releaseremovedLock(int threadId, int objId) {
	ASSERT(removedQueueAtomicState[objId]->hasLock(threadId));
	removedQueueAtomicState[objId]->releaseLock(threadId);
}

////void ThreadedTimeWarpMultiSet::getScheduleQueueLock(int threadId) {
////	while (!scheduleQueueLock->setLock(threadId))
////		;
////	ASSERT(scheduleQueueLock->hasLock(threadId));
////}
////void ThreadedTimeWarpMultiSet::releaseScheduleQueueLock(int threadId) {
////	ASSERT(scheduleQueueLock->hasLock(threadId));
////	scheduleQueueLock->releaseLock(threadId);
////}
////void ThreadedTimeWarpMultiSetLTSF::getObjectLock(int threadId, int objId) {
////	while (!objectStatusLock[objId]->setLock(threadId))
////		;
////	/*	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
////	 << " ) Object - " << objId << " is Locked by the thread - "
////	 << threadId << "\n";*/
////	ASSERT(objectStatusLock[objId]->hasLock(threadId));
////}
////void ThreadedTimeWarpMultiSetLTSF::releaseObjectLock(int threadId, int objId) {
////	ASSERT(objectStatusLock[objId]->hasLock(threadId));
////	objectStatusLock[objId]->releaseLock(threadId);
////	/*	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
////	 << " ) Object - " << objId << " is Released by the thread - "
////	 << threadId << "\n";*/
////}
bool ThreadedTimeWarpMultiSet::isObjectScheduled(int objId) {
	return LTSF1->isObjectScheduled(objId);
}

bool ThreadedTimeWarpMultiSet::isObjectScheduledBy(int threadId, int objId) {
	return LTSF1->isObjectScheduledBy(threadId, objId);
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
	if (!this->unprocessedQueueAtomicState[objId]->hasLock(threadId))
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
	ASSERT(this->isObjectScheduledBy(threadId, objId));
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
	ASSERT(this->isObjectScheduledBy(threadId, objId));
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

	ASSERT(this->isObjectScheduledBy(threadId, objId));

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
////const VTime* ThreadedTimeWarpMultiSet::nextEventToBeScheduledTime(int threadID) {
////	const VTime* ret = NULL;
////	this->getScheduleQueueLock(threadID);
////	if (scheduleQueue->size() > 0)
////		ret = &((*scheduleQueue->begin())->getReceiveTime());
////	this->releaseScheduleQueueLock(threadID);
////	return (ret);
////}
const VTime* ThreadedTimeWarpMultiSet::nextEventToBeScheduledTime(int threadId) {
	return LTSF1->nextEventToBeScheduledTime(threadId);
}
const Event* ThreadedTimeWarpMultiSet::peekEvent(SimulationObject *simObj,
		int threadId) {
	const Event* ret = NULL;
	
	bool releaseWhileReturn = true;
	SimulationObject *simObject = NULL;
	if (simObj == NULL) {
		////this->getScheduleQueueLock(threadId);
		////if (scheduleQueue->size() > 0) {
		////	ret = *(scheduleQueue->begin());
		////	unsigned int objId = ret->getReceiver().getSimulationObjectID();
		////	this ->getObjectLock(threadId, objId);
		////	//remove the object out of schedule
		////	scheduleQueue->erase(scheduleQueue->begin());
		////	//set the indexer/pointer to NULL
		////	lowestObjectPosition[objId] = scheduleQueue->end();
		////	/*		utils::debug << "( "
		////	 << mySimulationManager->getSimulationManagerID()
		////	 << " ) Object - " << objId
		////	 << " is removed out for schedule by the thread - "
		////	 << threadId << "\n";*/
		////}
		////this->releaseScheduleQueueLock(threadId);
               		ret = LTSF1->peekIt(threadId);
	} else if (simObj != NULL) {
		unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
		if (!this->unprocessedQueueAtomicState[objId]->hasLock(threadId)) {
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
	this->releaseunProcessedLock(threadId, objId);
	// The event was just inserted at the beginning, so update the Schedule Queue
	LTSF1->getScheduleQueueLock(threadId);
	if (!this->isObjectScheduled(objId)) {
		if (receivedEvent == *(itee)) {
			////// Do not erase the first time.
			////if (lowestObjectPosition[objId] != scheduleQueue->end()) {
			////	scheduleQueue->erase(lowestObjectPosition[objId]);
			////} else {
			////	/*utils::debug << "( "
			////	 << mySimulationManager->getSimulationManagerID()
			////	 << " ) Object - " << objId
			////	 << " is returned for schedule thro' insert by the thread - "
			////	 << threadId << "\n";*/
			////}
			////lowestObjectPosition[objId] = scheduleQueue->insert(receivedEvent);
			LTSF1->eraseSkipFirst(objId);
			LTSF1->insertEvent(objId, receivedEvent);

		}
	}
	LTSF1->releaseScheduleQueueLock(threadId);

	ASSERT(
			////scheduleQueue->size()
			LTSF1->getScheduleQueueSize()
					<= mySimulationManager->getNumberOfSimulationObjects());
	return false;
}
////// Lock based Counting -- Don't call this function in a loop
////int ThreadedTimeWarpMultiSet::getMessageCount(int threadId) {
////	int count = 0;
////	getScheduleQueueLock(threadId);
////	count = scheduleQueue->size();
////	releaseScheduleQueueLock(threadId);
////	return count;
////}
////bool ThreadedTimeWarpMultiSet::isScheduleQueueEmpty(int threadId) {
////	bool ret = false;
////	getScheduleQueueLock(threadId);
////	ret = scheduleQueue->empty();
////	releaseScheduleQueueLock(threadId);
////	return ret;
////}
bool ThreadedTimeWarpMultiSet::isScheduleQueueEmpty(int threadId) {
	return LTSF1->isScheduleQueueEmpty(threadId);
}
bool ThreadedTimeWarpMultiSet::handleAntiMessage(SimulationObject *simObj,
		const NegativeEvent* negativeEvent, int threadId) {
	bool eventWasRemoved = false;
	unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
	if (!this->unprocessedQueueAtomicState[objId]->hasLock(threadId))
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

void ThreadedTimeWarpMultiSet::updateScheduleQueueAfterExecute(int objId,
		int threadId) {
	const Event* firstEvent = NULL;
	if (!this->isObjectScheduledBy(threadId, objId))
		ASSERT(false);
	////this->getScheduleQueueLock(threadId);
	LTSF1->getScheduleQueueLock(threadId);
	if (!this->unprocessedQueueAtomicState[objId]->hasLock(threadId))
		this->getunProcessedLock(threadId, objId);
	if (unProcessedQueue[objId]->size() > 0) {
		firstEvent = *(unProcessedQueue[objId]->begin());
		/*utils::debug << "( " << mySimulationManager->getSimulationManagerID()
		 << " ) Object - " << objId << " has still "
		 << unProcessedQueue[objId]->size() << " events pending - "
		 << threadId << "\n";*/
	}
	this->releaseunProcessedLock(threadId, objId);

	// TODO: ADD THIS BACK IN!!!!
	////if (lowestObjectPosition[objId] != scheduleQueue->end()) {
	////	ASSERT(false);
	////}
	
	if (firstEvent != NULL) {
		LTSF1->insertEvent(objId, firstEvent);
		/*utils::debug << "( " << mySimulationManager->getSimulationManagerID()
		 << " ) Object - " << objId
		 << " is returned for schedule by the thread - " << threadId
		 << "\n";*/
	} else
		LTSF1->insertEventEnd(objId);

	LTSF1->releaseObjectLock(threadId, objId);
	utils::debug <<" ( "<< threadId << ") Returning the Object " <<objId <<" back to SCheQ"<<endl;
	LTSF1->releaseScheduleQueueLock(threadId);
	ASSERT(
			////scheduleQueue->size()
			LTSF1->getScheduleQueueSize()
					<= mySimulationManager->getNumberOfSimulationObjects());

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
	////this->getScheduleQueueLock(threadId);
	////scheduleQueue->clear();
	////this->releaseScheduleQueueLock(threadId);
	LTSF1->clearScheduleQueue(threadId);
	for (int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++) {
		this->getunProcessedLock(threadId, i);
		msit = unProcessedQueue[i]->begin();
		while (msit != unProcessedQueue[i]->end()) {
			(*msit)->~Event();
			::operator delete((void*) (*msit));
			unProcessedQueue[i]->erase(msit++);
		}
		this->releaseunProcessedLock(threadId, i);
		////this->getScheduleQueueLock(threadId);
		////lowestObjectPosition[i] = scheduleQueue->end();
		////this->releaseScheduleQueueLock(threadId);
		LTSF1->setLowestObjectPosition(threadId, i);
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
		////if (objectStatusLock[objNum]->isLocked()) {
		////	objectStatusLock[objNum]->releaseLock(
		////			objectStatusLock[objNum]->whoHasLock());
		////	utils::debug << "Releasing Object " << objNum
		////			<< " during recovery." << endl;
		////}
		LTSF1->releaseObjectLocksRecovery(objNum);
		if (unprocessedQueueAtomicState[objNum]->isLocked()) {
			unprocessedQueueAtomicState[objNum]->releaseLock(
					unprocessedQueueAtomicState[objNum]->whoHasLock());
			utils::debug << "Releasing Unprocessed Queue " << objNum
					<< " during recovery." << endl;
		}
		if (processedQueueAtomicState[objNum]->isLocked()) {
			processedQueueAtomicState[objNum]->releaseLock(
					processedQueueAtomicState[objNum]->whoHasLock());
			utils::debug << "Releasing Processed Queue " << objNum
					<< " during recovery." << endl;
		}
		if (removedQueueAtomicState[objNum]->isLocked()) {
			removedQueueAtomicState[objNum]->releaseLock(
					removedQueueAtomicState[objNum]->whoHasLock());
			utils::debug << "Releasing Removed Queue " << objNum
					<< " during recovery." << endl;
		}
	}
	////if (scheduleQueueLock->isLocked()) {
	////	scheduleQueueLock->releaseLock(scheduleQueueLock->whoHasLock());
	////	utils::debug << "Releasing Schedule Queue during recovery." << endl;
	////}
	LTSF1->releaseAllScheduleQueueLocks();
}
