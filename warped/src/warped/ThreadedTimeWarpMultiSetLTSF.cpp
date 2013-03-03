#include "ThreadedTimeWarpMultiSetLTSF.h"

ThreadedTimeWarpMultiSetLTSF::ThreadedTimeWarpMultiSetLTSF(int inObjectCount, int LTSFCountVal, const string syncMech, const string scheQScheme) {
	objectCount = inObjectCount;

	//scheduleQ scheme
	scheduleQScheme = scheQScheme;

	// Set up scheduleQueue (LTSF queue)
	if( scheduleQScheme == "MULTISET" ) {
		scheduleQueue = new multiset<const Event*,
				receiveTimeLessThanEventIdLessThan> ;
	} else if( scheduleQScheme == "LADDERQ" ) {
		ladderQ = new LadderQueue;
	} else if( scheduleQScheme == "SPLAYTREE" ) {
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
		if( scheduleQScheme == "MULTISET" ) {
			lowestObjectPosition.push_back(scheduleQueue->end());
		} else if( scheduleQScheme == "LADDERQ" ) {
			lowestLadderObjectPosition.push_back(ladderQ->end());
		} else if( scheduleQScheme == "SPLAYTREE" ) {
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
	if( scheduleQScheme == "MULTISET" ) {
		delete scheduleQueue;
	} else if ( scheduleQScheme == "LADDERQ" ) {
		delete ladderQ;
	} else if( scheduleQScheme == "SPLAYTREE" ) {
		delete splayTree;
	} else {
		cout << "Invalid schedule queue scheme" << endl;
	}
}

void ThreadedTimeWarpMultiSetLTSF::getScheduleQueueLock(int threadId) {
	utils::debug << "( "<<threadId<<" T ) Getting the Sche Lock." << endl;
	while (!scheduleQueueLock->setLock(threadId, syncMechanism));
	ASSERT(scheduleQueueLock->hasLock(threadId, syncMechanism));
}
void ThreadedTimeWarpMultiSetLTSF::releaseScheduleQueueLock(int threadId) {
	ASSERT(scheduleQueueLock->hasLock(threadId, syncMechanism));
	scheduleQueueLock->releaseLock(threadId, syncMechanism);
	utils::debug << "( "<<threadId<<" T ) Releasing the Sche Lock." << endl;
}
const VTime* ThreadedTimeWarpMultiSetLTSF::nextEventToBeScheduledTime(int threadID) {
	const VTime* ret = NULL;
	this->getScheduleQueueLock(threadID);
	if( scheduleQScheme == "MULTISET" ) {
		if (scheduleQueue->size() > 0) {
			ret = &((*scheduleQueue->begin())->getReceiveTime());
		}
	} else if( scheduleQScheme == "LADDERQ" ) {
		if(!ladderQ->empty())
			//cout << "nextEvent" << endl;
			ret = &(ladderQ->begin()->getReceiveTime());
	} else if( scheduleQScheme == "SPLAYTREE" ) {
		if(splayTree->peekEvent()) {
			ret = &(splayTree->peekEvent()->getReceiveTime());
		}
	} else {
		cout << "Invalid schedule queue scheme" << endl;
	}
	this->releaseScheduleQueueLock(threadID);
	return (ret);
}

// Lock based Counting -- Don't call this function in a loop
int ThreadedTimeWarpMultiSetLTSF::getMessageCount(int threadId) {
	int count = 0;
	getScheduleQueueLock(threadId);

	if( scheduleQScheme == "MULTISET" ) {
		count = scheduleQueue->size();
	} else if ( scheduleQScheme == "LADDERQ" ) {
		cout << "LadderQ message count not handled for now" << endl;
	} else if( scheduleQScheme == "SPLAYTREE" ) {
		count = splayTree->size();
	} else {
		cout << "Invalid schedule queue scheme" << endl;
	}

	releaseScheduleQueueLock(threadId);
	return count;
}
bool ThreadedTimeWarpMultiSetLTSF::isScheduleQueueEmpty() {
	if( scheduleQScheme == "MULTISET" ) {
		return scheduleQueue->empty();
	} else if ( scheduleQScheme == "LADDERQ" ) {
		return ladderQ->empty();
	} else if( scheduleQScheme == "SPLAYTREE" ) {
		return (splayTree->size() == 0) ? true : false;
	} else {
		cout << "Invalid schedule queue scheme" << endl;
	}	
}

void ThreadedTimeWarpMultiSetLTSF::releaseAllScheduleQueueLocks()
{
	if (scheduleQueueLock->isLocked()) {
		scheduleQueueLock->releaseLock(scheduleQueueLock->whoHasLock(), syncMechanism);
		utils::debug << "Releasing Schedule Queue during recovery." << endl;
	}
}
void ThreadedTimeWarpMultiSetLTSF::clearScheduleQueue(int threadId)
{
	this->getScheduleQueueLock(threadId);
	if( scheduleQScheme == "MULTISET" ) {
		scheduleQueue->clear();
	} else if ( scheduleQScheme == "LADDERQ" ) {
		ladderQ->clear();
	} else if( scheduleQScheme == "SPLAYTREE" ) {
		splayTree->clear();
	} else {
		cout << "Invalid schedule queue scheme" << endl;
	}
	this->releaseScheduleQueueLock(threadId);
}
void ThreadedTimeWarpMultiSetLTSF::setLowestObjectPosition(int threadId, int index)
{
	this->getScheduleQueueLock(threadId);
	if( scheduleQScheme == "MULTISET" ) {
		lowestObjectPosition[index] = scheduleQueue->end();
	} else if ( scheduleQScheme == "LADDERQ" ) {
		lowestLadderObjectPosition[index] = ladderQ->end();
	} else if( scheduleQScheme == "SPLAYTREE" ) {
		lowestLadderObjectPosition[index] = splayTree->end();
	} else {
		cout << "Invalid schedule queue scheme" << endl;
	}
	this->releaseScheduleQueueLock(threadId);
}
// Removes the Event for the given Obj from the schedule queue,
//  and returns the event - used for reassigning LPs
const Event* ThreadedTimeWarpMultiSetLTSF::removeLP(int objId)
{
	// Decrement number of LPs (so we know size of lowestObjectPosition
	const Event* removedEvent;
	// BUG: Update to work for other schemes...
	if( scheduleQScheme == "MULTISET" ) {
		if (lowestObjectPosition[objId] == scheduleQueue->end() ) {
			removedEvent = NULL;
		} else {
			removedEvent = *(lowestObjectPosition[objId]);
		}
	} else if ( scheduleQScheme == "LADDERQ" ) {
		if (lowestLadderObjectPosition[objId] == ladderQ->end() ) {
			removedEvent = NULL;
		} else {
			removedEvent = lowestLadderObjectPosition[objId];
		}
	} else if( scheduleQScheme == "SPLAYTREE" ) {
		if (lowestLadderObjectPosition[objId] == splayTree->end() ) {
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
int ThreadedTimeWarpMultiSetLTSF::addLP(int oldLockOwner)
{
	if( scheduleQScheme == "MULTISET" ) {
		lowestObjectPosition.push_back(scheduleQueue->end());
	} else if ( scheduleQScheme == "LADDERQ" ) {
		lowestLadderObjectPosition.push_back(ladderQ->end());
	} else if( scheduleQScheme == "SPLAYTREE" ) {
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
void ThreadedTimeWarpMultiSetLTSF::insertEvent(int objId, const Event* newEvent)
{
	if( scheduleQScheme == "MULTISET" ) {
		ASSERT(newEvent);
		lowestObjectPosition[objId] = scheduleQueue->insert(newEvent);
	} else if ( scheduleQScheme == "LADDERQ" ) {
		ASSERT(newEvent);
		lowestLadderObjectPosition[objId] = ladderQ->insert(newEvent);
	} else if( scheduleQScheme == "SPLAYTREE" ) {
		ASSERT(newEvent);
		splayTree->insert(newEvent);
		lowestLadderObjectPosition[objId] = newEvent;
	} else {
		cout << "Invalid schedule queue scheme" << endl;
	}
}
void ThreadedTimeWarpMultiSetLTSF::insertEventEnd(int objId)
{
	if( scheduleQScheme == "MULTISET" ) {
		lowestObjectPosition[objId] = scheduleQueue->end();
	} else if ( scheduleQScheme == "LADDERQ" ) {
		lowestLadderObjectPosition[objId] = ladderQ->end();
	} else if( scheduleQScheme == "SPLAYTREE" ) {
		lowestLadderObjectPosition[objId] = splayTree->end();
	} else {
		cout << "Invalid schedule queue scheme" << endl;
	}
}
void ThreadedTimeWarpMultiSetLTSF::eraseSkipFirst(int objId)
{
	// Do not erase the first time.
	if( scheduleQScheme == "MULTISET" ) {
		if (lowestObjectPosition[objId] != scheduleQueue->end()) {
			scheduleQueue->erase(lowestObjectPosition[objId]);
		} else {
			/*utils::debug << "( "
			 << mySimulationManager->getSimulationManagerID()
			 << " ) Object - " << objId
			 << " is returned for schedule thro' insert by the thread - "
			 << threadId << "\n";*/
		}
	} else if ( scheduleQScheme == "LADDERQ" ) {
		if(lowestLadderObjectPosition[objId] != ladderQ->end()) {
			ladderQ->erase(lowestLadderObjectPosition[objId]);
		}
	} else if( scheduleQScheme == "SPLAYTREE" ) {
		if(lowestLadderObjectPosition[objId] != splayTree->end()) {
			splayTree->erase(lowestLadderObjectPosition[objId]);
		}
	} else {
		cout << "Invalid schedule queue scheme" << endl;
	}
}
int ThreadedTimeWarpMultiSetLTSF::getScheduleQueueSize()
{
	if( scheduleQScheme == "MULTISET" ) {
		return scheduleQueue->size();
	} else if ( scheduleQScheme == "LADDERQ" ) {
		cout << "LadderQ message count not handled for now" << endl;
	} else if( scheduleQScheme == "SPLAYTREE" ) {
		return splayTree->size();
	} else {
		cout << "Invalid schedule queue scheme" << endl;
	}
}
const Event* ThreadedTimeWarpMultiSetLTSF::peekIt(int threadId, int **LTSFObjId)
{
	const Event* ret = NULL;
	this->getScheduleQueueLock(threadId);

	if( scheduleQScheme == "MULTISET" ) {
		if (scheduleQueue->size() > 0) {
			utils::debug<<"( "<< threadId << " T ) Peeking from Schedule Queue"<<endl;

			ret = *(scheduleQueue->begin());
			//cout << "T" << threadId << ": Getting event with timestamp " << ret->getReceiveTime().getApproximateIntTime() << endl;
			unsigned int objId = LTSFObjId[ret->getReceiver().getSimulationObjectID()][0];
			utils::debug <<" ( "<< threadId << ") Locking the Object " <<objId <<endl;
	
			getObjectLock(threadId, objId);
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
	} else if ( scheduleQScheme == "LADDERQ" ) {
		if( !ladderQ->empty() ) {
			utils::debug<<"( "<< threadId << " T ) Peeking from Schedule Queue"<<endl;
			ret = ladderQ->dequeue();
			if(ret == NULL) {
				cout << "dequeue() func returned NULL" << endl;
				this->releaseScheduleQueueLock(threadId);
				return ret;
			}
			unsigned int newMinTime = ret->getReceiveTime().getApproximateIntTime();
			if ( newMinTime < minReceiveTime )
				cout << "Event received out of order" << endl;
			unsigned int objId = LTSFObjId[ret->getReceiver().getSimulationObjectID()][0];

			utils::debug <<" ( "<< threadId << ") Locking the Object " <<objId <<endl;

			this->getObjectLock(threadId, objId);

			//set the indexer/pointer to NULL
			lowestLadderObjectPosition[objId] = ladderQ->end();


		}
	} else if( scheduleQScheme == "SPLAYTREE" ) {
		if( splayTree->peekEvent() ) {
			utils::debug<<"( "<< threadId << " T ) Peeking from Schedule Queue"<<endl;
			if(NULL == (ret = splayTree->getEvent()) ) {
				cout << "getEvent() func returned NULL" << endl;
				this->releaseScheduleQueueLock(threadId);
				return ret;
			}
			unsigned int objId = LTSFObjId[ret->getReceiver().getSimulationObjectID()][0];

			utils::debug <<" ( "<< threadId << ") Locking the Object " <<objId <<endl;

			this->getObjectLock(threadId, objId);

			//set the indexer/pointer to NULL
			lowestLadderObjectPosition[objId] = splayTree->end();
		}
	} else {
		cout << "Invalid schedule queue scheme" << endl;
	}

	this->releaseScheduleQueueLock(threadId);
	return ret;
}

void ThreadedTimeWarpMultiSetLTSF::getObjectLock(int threadId, int objId) {
	while (!objectStatusLock[objId]->setLock(threadId, syncMechanism));
	/*	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
	 << " ) Object - " << objId << " is Locked by the thread - "
	 << threadId << "\n";*/
	ASSERT(objectStatusLock[objId]->hasLock(threadId, syncMechanism));
}
void ThreadedTimeWarpMultiSetLTSF::releaseObjectLock(int threadId, int objId) {
	ASSERT(objectStatusLock[objId]->hasLock(threadId, syncMechanism));
	objectStatusLock[objId]->releaseLock(threadId, syncMechanism);
	/*	utils::debug << "( " << mySimulationManager->getSimulationManagerID()
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
		utils::debug << "Releasing Object " << objNum
				<< " during recovery." << endl;
	}
}
