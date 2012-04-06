// See copyright notice in file Copyright in the root directory of this archive.

#include "DTTimeWarpMultiSet.h"
#include <assert.h>
#include "SimulationObject.h"
#include "DTTimeWarpSimulationManager.h"
#include "DTTimeWarpMultiSetSchedulingManager.h"
#include "StragglerEvent.h"

class NegativeEvent;
using std::cout;
using std::cerr;
using std::endl;

DTTimeWarpMultiSet::DTTimeWarpMultiSet(
		DTTimeWarpSimulationManager* initSimulationManager) {
	//Input queues
	objectCount = initSimulationManager->getNumberOfSimulationObjects();

	unprocessedQueueAtomicState = new AtomicState *[objectCount];
	processedQueueAtomicState = new AtomicState *[objectCount];
	removedQueueAtomicState = new AtomicState *[objectCount];

	//Schedule Queues
	scheduleQueue = new multiset<const Event*,
			receiveTimeLessThanEventIdLessThan> ;
	objectStatusLock = new AtomicState *[objectCount];
	scheduleQueueLock = new AtomicState();
	//	unProcessedQueueArray = new MS[NUMBER_OF_OBJECTS];
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
		objectStatusLock[i] = new AtomicState();
		//Schedule queue
		lowestObjectPosition.push_back(scheduleQueue->end());
	}
	mySimulationManager = initSimulationManager;

}

DTTimeWarpMultiSet::~DTTimeWarpMultiSet() {
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
	delete objectStatusLock;
	//	deleting each Threads Iterator
	delete vectorIterator;
	delete multisetIterator;
}

void DTTimeWarpMultiSet::getunProcessedLock(int threadId, int objId) {
	while (!unprocessedQueueAtomicState[objId]->setLock(threadId))
		;
	/*	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
	 << " ) Queue - " << objId << " is Locked by the thread - "
	 << threadId << "\n";*/
	assert(unprocessedQueueAtomicState[objId]->hasLock(threadId));
}
void DTTimeWarpMultiSet::releaseunProcessedLock(int threadId, int objId) {
	assert(unprocessedQueueAtomicState[objId]->hasLock(threadId));
	unprocessedQueueAtomicState[objId]->releaseLock(threadId);
	/*	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
	 << " ) Queue - " << objId << " is Released by the thread - "
	 << threadId << "\n";*/
}
void DTTimeWarpMultiSet::getProcessedLock(int threadId, int objId) {
	while (!processedQueueAtomicState[objId]->setLock(threadId))
		;
	assert(processedQueueAtomicState[objId]->hasLock(threadId));
}
void DTTimeWarpMultiSet::releaseProcessedLock(int threadId, int objId) {
	assert(processedQueueAtomicState[objId]->hasLock(threadId));
	processedQueueAtomicState[objId]->releaseLock(threadId);
}
void DTTimeWarpMultiSet::getremovedLock(int threadId, int objId) {
	while (!removedQueueAtomicState[objId]->setLock(threadId))
		;
	assert(removedQueueAtomicState[objId]->hasLock(threadId));
}
void DTTimeWarpMultiSet::releaseremovedLock(int threadId, int objId) {
	assert(removedQueueAtomicState[objId]->hasLock(threadId));
	removedQueueAtomicState[objId]->releaseLock(threadId);
}

void DTTimeWarpMultiSet::getScheduleQueueLock(int threadId) {
	while (!scheduleQueueLock->setLock(threadId))
		;
	assert(scheduleQueueLock->hasLock(threadId));
}
void DTTimeWarpMultiSet::releaseScheduleQueueLock(int threadId) {
	assert(scheduleQueueLock->hasLock(threadId));
	scheduleQueueLock->releaseLock(threadId);
}
void DTTimeWarpMultiSet::getObjectLock(int threadId, int objId) {
	while (!objectStatusLock[objId]->setLock(threadId))
		;
	/*	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
	 << " ) Object - " << objId << " is Locked by the thread - "
	 << threadId << "\n";*/
	assert(objectStatusLock[objId]->hasLock(threadId));
}
void DTTimeWarpMultiSet::releaseObjectLock(int threadId, int objId) {
	//cout << objId << " is released" << endl;
	assert(objectStatusLock[objId]->hasLock(threadId));
	objectStatusLock[objId]->releaseLock(threadId);
	/*	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
	 << " ) Object - " << objId << " is Released by the thread - "
	 << threadId << "\n";*/
}

bool DTTimeWarpMultiSet::isObjectScheduled(int objId) {
	return objectStatusLock[objId]->isLocked();
}

bool DTTimeWarpMultiSet::isObjectScheduledBy(int threadId, int objId) {
	return (objectStatusLock[objId]->whoHasLock() == threadId) ? true : false;
}
//not thread Safe
int DTTimeWarpMultiSet::getQueueEventCount(int objId) {
	int size;
	size = unProcessedQueue[objId]->size();
	//size = unProcessedQueueArray[objId]->size();
	return size;
}
//This Function will be called by the worker when the object has been scheduled, so no need to update schedule queue(need to verify this)
const Event* DTTimeWarpMultiSet::getEvent(SimulationObject *simObj,
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
			//ret = dynamic_cast<const StragglerEvent*> (ret)->getPositiveEvent();
			assert(false);
		this->getProcessedLock(threadId, objId);
		processedQueue[objId]->push_back(ret);
		this->releaseProcessedLock(threadId, objId);
		//	cout << "Event Inserted to PQ :::::::::::::" << *ret << endl;
	} else {
		this->releaseunProcessedLock(threadId, objId);
	}
	assert(this->isObjectScheduledBy(threadId, objId));
	return ret;
}
const Event* DTTimeWarpMultiSet::getEventWhileRollback(
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
			//ret = dynamic_cast<const StragglerEvent*> (ret)->getPositiveEvent();
			assert(false);
		this->getProcessedLock(threadId, objId);
		processedQueue[objId]->push_back(ret);
		this->releaseProcessedLock(threadId, objId);
	}
	assert(this->isObjectScheduledBy(threadId, objId));
	return ret;
}
const Event* DTTimeWarpMultiSet::getEventIfStraggler(SimulationObject *simObj,
		int threadId) {
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

	assert(this->isObjectScheduledBy(threadId, objId));

	/*if (ret != NULL && !(objectStatusLock[objId]->hasLock(threadId))) {
	 this->getObjectLock(threadId, objId);
	 }*/
	return ret;
}
const Event *DTTimeWarpMultiSet::getEvent(SimulationObject *simObj,
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
const VTime* DTTimeWarpMultiSet::nextEventToBeScheduledTime(int threadID) {
	const VTime* ret = NULL;
	this->getScheduleQueueLock(threadID);
	if (scheduleQueue->size() > 0)
		ret = &((*scheduleQueue->begin())->getReceiveTime());
	this->releaseScheduleQueueLock(threadID);
	return (ret);
}
const Event* DTTimeWarpMultiSet::peekEvent(SimulationObject *simObj,
		int threadId) {
	const Event* ret = NULL;
	bool releaseWhileReturn = true;
	SimulationObject *simObject = NULL;
	if (simObj == NULL /*&& scheduleQueue->begin() != scheduleQueue->end()*/) {
		this->getScheduleQueueLock(threadId);
		if (scheduleQueue->size() > 0) {
			ret = *(scheduleQueue->begin());
			unsigned int objId = ret->getReceiver().getSimulationObjectID();
			this ->getObjectLock(threadId, objId);
			//remove the object out of schedule
			scheduleQueue->erase(scheduleQueue->begin());
			//set the indexer/pointer to NULL
			lowestObjectPosition[objId] = scheduleQueue->end();
			/*		utils::debug << "( "
			 << mySimulationManager->getSimulationManagerID()
			 << " ) Object - " << objId
			 << " is removed out for schedule by the thread - "
			 << threadId << "\n";*/
		}
		this->releaseScheduleQueueLock(threadId);
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

const Event* DTTimeWarpMultiSet::peekEventCoastForward(
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

const Event *DTTimeWarpMultiSet::peekEvent(SimulationObject *simObj,
		const VTime &minimumTime, int threadId) {
	const Event *retval = peekEvent(simObj, threadId);
	if (retval != NULL) {
		if (!(retval->getReceiveTime() < minimumTime)) {
			retval = NULL;
		}
	}
	return retval;
}
const Event *DTTimeWarpMultiSet::peekEventLockUnprocessed(
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
bool DTTimeWarpMultiSet::insert(const Event *receivedEvent, int threadId) {
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
	this->getScheduleQueueLock(threadId);
	if (!this->isObjectScheduled(objId)) {
		if (receivedEvent == *(itee)) {
			// Do not erase the first time.
			if (lowestObjectPosition[objId] != scheduleQueue->end()) {
				scheduleQueue->erase(lowestObjectPosition[objId]);
			} else {
				/*utils::debug << "( "
				 << mySimulationManager->getSimulationManagerID()
				 << " ) Object - " << objId
				 << " is returned for schedule thro' insert by the thread - "
				 << threadId << "\n";*/
			}
			lowestObjectPosition[objId] = scheduleQueue->insert(receivedEvent);

		}
	}
	this->releaseScheduleQueueLock(threadId);

	assert(
			scheduleQueue->size()
					<= mySimulationManager->getNumberOfSimulationObjects());
	return false;//Old Code Always Returns False-- Check why?
}
// Lock based Counting -- Don't Use it a loop
int DTTimeWarpMultiSet::getMessageCount(int threadId) {
	int count = 0;
	/*for (int i = 0; i < objectCount; i++) {
	 this->getunProcessedLock(threadId, i);
	 count = count + unProcessedQueue[i]->size();
	 this->releaseunProcessedLock(threadId, i);
	 }*/
	getScheduleQueueLock(threadId);
	count = scheduleQueue->size();
	releaseScheduleQueueLock(threadId);
	//utils::debug << "Unprocessed Event Count :: " << count << "\n";
	return count;
}
bool DTTimeWarpMultiSet::isScheduleQueueEmpty(int threadId) {
	bool ret = false;
	getScheduleQueueLock(threadId);
	ret = scheduleQueue->empty();
	releaseScheduleQueueLock(threadId);
	return ret;
}
//This Function will be called by the worker when the object has been scheduled, so no need to update schedule queue(need to verify this)
bool DTTimeWarpMultiSet::handleAntiMessage(SimulationObject *simObj,
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
			//utils::debug << "Removed .." << endl;
		} else {
			multisetIterator[threadId]++;
		}
	}
	this->releaseunProcessedLock(threadId, objId);

	return eventWasRemoved;
}

void DTTimeWarpMultiSet::rollback(SimulationObject *simObj,
		const VTime &rollbackTime, int threadId) {
	// Go through the entire processed events queue and put any events with
	// a receive time greater than or equal to the rollback time back in the
	// unprocessed queue.
	unsigned int objId = simObj->getObjectID()->getSimulationObjectID();
	this->getProcessedLock(threadId, objId);
	//	std::sort(processedQueue[objId]->begin(), processedQueue[objId]->end(),
	//			receiveTimeLessThan());
	vectorIterator[threadId] = processedQueue[objId]->begin();
	//	while (vectorIterator[threadId] != processedQueue[objId]->end()) {
	//		cout << "Printing Event.............::::::::::::: " << **(vectorIterator[threadId]) << endl;
	//		(vectorIterator[threadId])++;
	//	}
	int tempCount = 0;
	if (rollbackTime.getApproximateIntTime() == 0) {
		tempCount = processedQueue[objId]->size();
	} else {
		vectorIterator[threadId] = processedQueue[objId]->begin();
		while (vectorIterator[threadId] != processedQueue[objId]->end()
				&& (*(vectorIterator[threadId]))->getReceiveTime()
						< rollbackTime) {
			//cout << "Skipping Event.............::::::::::::: " << **(vectorIterator[threadId]) << endl;
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
	//this->getunProcessedLock(threadId, objId);
	unProcessedQueue[objId]->insert(vectorIterator[threadId],
			processedQueue[objId]->end());
	//this->releaseunProcessedLock(threadId, objId);
	processedQueue[objId]->erase(vectorIterator[threadId],
			processedQueue[objId]->end());
	this->releaseProcessedLock(threadId, objId);

}
void DTTimeWarpMultiSet::fossilCollect(SimulationObject *simObj,
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
void DTTimeWarpMultiSet::fossilCollect(SimulationObject *object,
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

void DTTimeWarpMultiSet::fossilCollect(const Event *toRemove, int threadId) {
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
void DTTimeWarpMultiSet::printAll() {

	//Re-implement Print All
	/*	cout << "UnProcessed Queue" << endl;
	 multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator it;
	 for (int i = 0; i < NUMBER_OF_OBJECTS; i++) {
	 cout << "Queue " << i << " ::: ";
	 for (it = unProcessedQueue[i]->begin(); it
	 != unProcessedQueue[i]->end(); it++) {
	 cout << "-" << (*it)->getReceiveTime();
	 }
	 cout << " " << endl;
	 }
	 cout << "Processed Queue" << endl;
	 vector<event*>::iterator iv;
	 for (int i = 0; i < NUMBER_OF_OBJECTS; i++) {
	 cout << "Queue " << i << " ::: ";
	 for (iv = processedQueue[i]->begin(); iv != processedQueue[i]->end(); iv++) {
	 cout << "-" << (*iv)->receiveTime;
	 }
	 cout << " " << endl;
	 }
	 cout << "Schedule Queue" << endl;
	 multiset<event*, compareTime>::iterator iv1;
	 for (iv1 = scheduleQueue->begin(); iv1 != scheduleQueue->end(); iv1++) {
	 cout << "-" << (*iv1)->receiveTime;
	 }
	 cout << " " << endl;
	 cout << "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: "
	 << endl;*/
}
//Stub function to insert into processed queue
void DTTimeWarpMultiSet::fillProcessed(SimulationObject *simObj) {
	this->getEvent(simObj, 1);
}

void DTTimeWarpMultiSet::updateScheduleQueueAfterExecute(int objId,
		int threadId) {
	const Event* firstEvent = NULL;
	if (!this->isObjectScheduledBy(threadId, objId))
		assert(false);
	this->getScheduleQueueLock(threadId);
	if (!this->unprocessedQueueAtomicState[objId]->hasLock(threadId))
		this->getunProcessedLock(threadId, objId);
	//this->getunProcessedLock(threadId, objId);
	if (unProcessedQueue[objId]->size() > 0) {
		firstEvent = *(unProcessedQueue[objId]->begin());
		/*utils::debug << "( " << mySimulationManager->getSimulationManagerID()
		 << " ) Object - " << objId << " has still "
		 << unProcessedQueue[objId]->size() << " events pending - "
		 << threadId << "\n";*/
	}
	this->releaseunProcessedLock(threadId, objId);
	//	this->getScheduleQueueLock(threadId);

	if (lowestObjectPosition[objId] != scheduleQueue->end()) {
		//scheduleQueue->erase(lowestObjectPosition[objId]);
		ASSERT(false);
	}

	if (firstEvent != NULL /*&& lowestObjectPosition[objId]
	 == scheduleQueue->end()*/) {
		lowestObjectPosition[objId] = scheduleQueue->insert(firstEvent);
		/*utils::debug << "( " << mySimulationManager->getSimulationManagerID()
		 << " ) Object - " << objId
		 << " is returned for schedule by the thread - " << threadId
		 << "\n";*/
	} else
		lowestObjectPosition[objId] = scheduleQueue->end();
	this->releaseObjectLock(threadId, objId);
	this->releaseScheduleQueueLock(threadId);
	//if (isObjectScheduledBy(threadId, objId))
	assert(
			scheduleQueue->size()
					<= mySimulationManager->getNumberOfSimulationObjects());

}

//Dont Know, Who call this function. Its not completely tested
bool DTTimeWarpMultiSet::inThePast(const Event *toCheck, int threadId) {
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
//this Fucntion is really screwed-up need to test this
void DTTimeWarpMultiSet::ofcPurge(int threadId) {
	multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator msit;
	this->getScheduleQueueLock(threadId);
	scheduleQueue->clear();
	this->releaseScheduleQueueLock(threadId);

	for (int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++) {
		this->getunProcessedLock(threadId, i);
		msit = unProcessedQueue[i]->begin();
		while (msit != unProcessedQueue[i]->end()) {
			(*msit)->~Event();
			::operator delete((void*) (*msit));
			unProcessedQueue[i]->erase(msit++);
		}
		this->releaseunProcessedLock(threadId, i);
		this->getScheduleQueueLock(threadId);
		lowestObjectPosition[i] = scheduleQueue->end();
		this->releaseScheduleQueueLock(threadId);
		//	insertObjPos[i] = unprocessedObjEvents[i]->end();
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
/*const VTime &DTTimeWarpMultiSet::getMinimumOfAll(int threadId) {
 const VTime* minTime = NULL;
 for (int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++) {
 this->getunProcessedLock(threadId, i);
 }
 minTime = &(mySimulationManager->getPositiveInfinity());
 for (int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++) {
 if (*(unProcessedQueue[i]->begin()) != NULL)
 if (*minTime > (*(unProcessedQueue[i]->begin()))-> getReceiveTime()) {
 minTime = &(*(unProcessedQueue[i]->begin()))-> getReceiveTime();
 }
 }
 for (int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++) {
 this->releaseunProcessedLock(threadId, i);
 }
 return *minTime->clone();
 }*/
const Event* DTTimeWarpMultiSet::peekEventLockUnprocessed(
		SimulationObject *simObj, int threadId) {
	const Event* ret = NULL;
	SimulationObject *simObject = NULL;
	assert(simObj != NULL);
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
const VTime* DTTimeWarpMultiSet::getMinEventTime(unsigned int threadId,
		unsigned objId) {
	VTime* ret = NULL;
	getunProcessedLock(threadId, objId);
	if (unProcessedQueue[objId]->size() > 0) {
		ret = (*unProcessedQueue[objId]->begin())->getReceiveTime().clone();
	}
	releaseunProcessedLock(threadId, objId);
	return ret;
}
void DTTimeWarpMultiSet::releaseObjectLocksRecovery() {
	for (int objNum = 0; objNum
			< mySimulationManager->getNumberOfSimulationObjects(); objNum++) {
		if (objectStatusLock[objNum]->isLocked()) {
			objectStatusLock[objNum]->releaseLock(
					objectStatusLock[objNum]->whoHasLock());
			utils::debug << "Releasing Object " << objNum
					<< " during recovery." << endl;
		}
	}
}
