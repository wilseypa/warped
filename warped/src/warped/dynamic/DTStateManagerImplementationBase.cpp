// See copyright notice in file Copyright in the root directory of this archive.

#include "DTStateManagerImplementationBase.h"
#include "SimulationObject.h"
#include "State.h"
#include "OptFossilCollManager.h"
#include "SetObject.h"

using std::cerr;
using std::endl;

DTStateManagerImplementationBase::DTStateManagerImplementationBase(
		DTTimeWarpSimulationManager *simMgr, unsigned int period) :
			mySimulationManager(simMgr),
			statePeriod(period),
			myStateQueue(
					new multiset<SetObject<State> > [simMgr->getNumberOfSimulationObjects()]),
			objectStatePeriod(simMgr->getNumberOfSimulationObjects(), period),
			rollbackEventNumber(simMgr->getNumberOfSimulationObjects(), 0),
			lastRollbackSenderObjectId(simMgr->getNumberOfSimulationObjects(),
					0),
			lastRollbackSenderObjectSimId(
					simMgr->getNumberOfSimulationObjects(), 0),
			lastRollbackTime(simMgr->getNumberOfSimulationObjects(), NULL),
			periodCounter(simMgr->getNumberOfSimulationObjects(), -1),
			stateQueueLock(
					new AtomicState*[simMgr->getNumberOfSimulationObjects()]) {
	/*unsigned int numberOfObjects = (unsigned int)simMgr->getNumberOfSimulationObjects();
	 for (int i = 0; i < 8; i++) {
	 stateQueueLock[i] = new AtomicState();
	 }*/
	initStateQueueLocks(simMgr);
}

DTStateManagerImplementationBase::~DTStateManagerImplementationBase() {
	delete[] myStateQueue;
	delete stateQueueLock;
}

void DTStateManagerImplementationBase::saveState(const VTime &currentTime,
		unsigned int eventNumber, SimulationObject *object,
		const ObjectID senderId, int threadID) {
	cerr << "DTStateManagerImplementationBase::saveState called" << endl;
	cerr << "Exiting simulation ..." << endl;
	exit(-1);
}
void DTStateManagerImplementationBase::updateStateWhileCoastForward(
		const VTime &currentTime, unsigned int eventNumber,
		SimulationObject *object, const ObjectID senderId, int threadID) {
	cerr << "DTStateManagerImplementationBase::saveState called" << endl;
	cerr << "Exiting simulation ..." << endl;
	exit(-1);
}
void DTStateManagerImplementationBase::saveState(const VTime &currentTime,
		SimulationObject *object, int threadID) {
	cerr << "DTStateManagerImplementationBase::saveState called" << endl;
	cerr << "Exiting simulation ..." << endl;
	exit(-1);
}

unsigned int DTStateManagerImplementationBase::getStatePeriod(int threadID) {
	return statePeriod;
}

vector<unsigned int> DTStateManagerImplementationBase::getObjectStatePeriod(
		int threadID) {
	return objectStatePeriod;
}

const VTime&
DTStateManagerImplementationBase::restoreState(const VTime &rollbackTime,
		SimulationObject *object, int threadID) {
	// store this object's id temporarilyAa
	OBJECT_ID *currentObjectID = object->getObjectID();
	unsigned int objId = currentObjectID->getSimulationObjectID();

	this->getStateQueueLock(threadID, objId);
	multiset<SetObject<State> >::iterator iter_begin =
			myStateQueue[objId].begin();

	// start from the end of the queue
	multiset<SetObject<State> >::iterator iter_end = myStateQueue[objId].end();

	if (iter_end != iter_begin) {
		iter_end--;

		// restore current state to the last state in the state queue
		// that is less than the rollback time
		while (iter_end != iter_begin && (*iter_end).getMainTime()
				> rollbackTime) {
			// No longer used for optimistic fossil collection.
			/*if(mySimulationManager->getOptFossilColl()){
			 mySimulationManager->getFossilCollManager()->deleteState((*iter_end).getElement(), objId);
			 }
			 else{
			 object->deallocateState((*iter_end).getElement());
			 }*/
			object->deallocateState((*iter_end).getElement());
			myStateQueue[objId].erase(iter_end--);
		}
		(lastRollbackTime[objId]) = (*iter_end).getMainTime().clone();
		lastRollbackSenderObjectId[objId] = (*iter_end).getsenderObjectId();
/*		cout << "--------------------+++++++++++-------- "
				<< (*iter_end).getMainTime()
				<< "--------------------++++++++++++"
				<< (*iter_end).getsenderObjectId()
				<< "--------------------++++++++++++" << endl;*/
		//	iter_end--;
		//	int tempCount = 0;
		//	while (iter_end != iter_begin && (*iter_end).getMainTime() == *tempTime) {
		//		tempCount++;
		//		iter_end--;
		//	}
		/*	//	cout << "-------------------->>>>>>>>-------- " << tempCount
		 //			<< "--------------------<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;*/
		//	if (tempCount > 0) {
		//		std::advance(iter_end, tempCount + 1);
		//		while (tempCount > 0) {
		//			object->deallocateState((*iter_end).getElement());
		//			myStateQueue[objId].erase(iter_end--);
		//			tempCount--;
		//		}
		//	} else
		//		iter_end++;
		// at this point, the iterator points to the state we want to restore
		object->getState()->copyState((*iter_end).getElement());
		rollbackEventNumber[objId] = (*iter_end).getEventNumber();
		//	cout << "EventId Value While retiving ::::::::::::::::: "
		//			<< rollbackEventNumber[objId] << endl;
		this->releaseStateQueueLock(threadID, objId);
		//	return (*iter_end).getMainTime();
		return *(lastRollbackTime[objId]);

	} else {
		// There are no states to be restored. If using optimistic fossil collection, rollback to a saved
		// checkpoint. This should never happen when not using optimistic fossil collection, so it will
		// result in a fatal error.
		if (mySimulationManager->getOptFossilColl()) {
			if (!mySimulationManager->getRecoveringFromCheckpoint()) {
				utils::debug << mySimulationManager->getSimulationManagerID()
						<< " - No states saved for this object. Rollback time: "
						<< rollbackTime << endl;
				utils::debug << "object" << *currentObjectID
						<< ": Current Simulation Time is "
						<< object->getSimulationTime() << endl;

				mySimulationManager->getOptFossilCollManagerNew()->startRecovery(
						objId, rollbackTime.getApproximateIntTime());
			}
		} else {
			cerr << mySimulationManager->getSimulationManagerID()
					<< " - No states saved for this object. Rollback time: "
					<< rollbackTime << endl;
			cerr << "object" << *currentObjectID
					<< ": Current Simulation Time is "
					<< object->getSimulationTime() << endl;

			abort();
		}

		return mySimulationManager->getZero();
	}
}

const VTime&
DTStateManagerImplementationBase::fossilCollect(SimulationObject *object,
		const VTime &fossilCollectTime, int threadID) {
	// store this object's id temporarily
	OBJECT_ID *objID = object->getObjectID();
	unsigned int objId = objID->getSimulationObjectID();
	this->getStateQueueLock(threadID, objId);
	if (!myStateQueue[objID->getSimulationObjectID()].empty()) {
		if (fossilCollectTime != mySimulationManager->getPositiveInfinity()) {
			// construct my search key
			SetObject<State> searchObject(fossilCollectTime);

			// get a handle to the beginning of this queue
			multiset<SetObject<State> >::iterator iter_begin =
					myStateQueue[objID->getSimulationObjectID()].begin();

			// find the last state upto which fossil collection will take place.
			// note: one state older than fossilCollectTime will be kept in the
			// queue since we can rollback to GVT. this is our marker.
			multiset<SetObject<State> >::iterator iter_end =
					myStateQueue[objID->getSimulationObjectID()].lower_bound(
							searchObject);

			if (iter_end != iter_begin) {
				--iter_end;
			}

			while (iter_begin != iter_end) {
				object->deallocateState((*iter_begin).getElement());
				myStateQueue[objID->getSimulationObjectID()].erase(iter_begin++);
			}
		} else {
			// walk from the front of the queue and delete everything
			multiset<SetObject<State> >::iterator iter_begin =
					myStateQueue[objID->getSimulationObjectID()].begin();
			multiset<SetObject<State> >::iterator iter_end =
					myStateQueue[objID->getSimulationObjectID()].end();
			while (iter_begin != iter_end) {
				object->deallocateState((*iter_begin).getElement());

				myStateQueue[objID->getSimulationObjectID()].erase(iter_begin++);
			}
		}
	} // else do nothing

	// Return the lowest timestamped state.
	multiset<SetObject<State> >::iterator iter_begin =
			myStateQueue[objID->getSimulationObjectID()].begin();
	if (!myStateQueue[objID->getSimulationObjectID()].empty()) {
		this->releaseStateQueueLock(threadID, objId);
		return (*iter_begin).getMainTime();
	} else {
		this->releaseStateQueueLock(threadID, objId);
		return mySimulationManager->getPositiveInfinity();
	}
	this->releaseStateQueueLock(threadID, objId);
}

const VTime&
DTStateManagerImplementationBase::fossilCollect(SimulationObject *object,
		int fossilCollectTime, int threadID) {
	// store this object's id temporarily
	OBJECT_ID *objID = object->getObjectID();
	unsigned int objId = objID->getSimulationObjectID();

	this->getStateQueueLock(threadID, objId);
	if (!myStateQueue[objID->getSimulationObjectID()].empty()) {
		if (fossilCollectTime
				!= mySimulationManager->getPositiveInfinity().getApproximateIntTime()) {
			// get a handle to the beginning of this queue
			multiset<SetObject<State> >::iterator iter_begin =
					myStateQueue[objID->getSimulationObjectID()].begin();

			// find the last state upto which fossil collection will take place.
			// note: one state older than fossilCollectTime will be kept in the
			// queue since we can rollback to GVT. this is our marker.
			multiset<SetObject<State> >::iterator iter_end =
					myStateQueue[objID->getSimulationObjectID()].end();

			while (iter_begin != iter_end
					&& iter_begin->getMainTime().getApproximateIntTime()
							< fossilCollectTime) {
				object->deallocateState((*iter_begin).getElement());
				myStateQueue[objID->getSimulationObjectID()].erase(iter_begin++);
			}
		} else {
			// walk from the front of the queue and delete everything
			multiset<SetObject<State> >::iterator iter_begin =
					myStateQueue[objID->getSimulationObjectID()].begin();
			multiset<SetObject<State> >::iterator iter_end =
					myStateQueue[objID->getSimulationObjectID()].end();
			while (iter_begin != iter_end) {
				object->deallocateState((*iter_begin).getElement());

				myStateQueue[objID->getSimulationObjectID()].erase(iter_begin++);
			}
		}
	} // else do nothing

	// Return the lowest timestamped state.
	multiset<SetObject<State> >::iterator iter_begin =
			myStateQueue[objID->getSimulationObjectID()].begin();
	if (!myStateQueue[objID->getSimulationObjectID()].empty()) {
		this->releaseStateQueueLock(threadID, objId);
		return (*iter_begin).getMainTime();
	} else {
		this->releaseStateQueueLock(threadID, objId);
		return mySimulationManager->getPositiveInfinity();
	}
	this->releaseStateQueueLock(threadID, objId);
}
// not thread safe as of now.
void DTStateManagerImplementationBase::printStateQueue(
		const VTime &currentTime, SimulationObject *object, ostream &out,
		int threadID) {
	// store this object's id temporarily
	OBJECT_ID *currentObjectID = object->getObjectID();

	if (!myStateQueue[currentObjectID->getSimulationObjectID()].empty()) {

		// get a handle to the beginning of this queue
		multiset<SetObject<State> >::iterator iter_begin =
				myStateQueue[currentObjectID->getSimulationObjectID()].begin();

		// get a handle to the end of this queue
		multiset<SetObject<State> >::iterator iter_end =
				myStateQueue[currentObjectID->getSimulationObjectID()].end();

		// iterate through the queue and print each state's time
		out << "object " << currentObjectID->getSimulationObjectID() << "@("
				<< currentTime << "): ";
		while (iter_begin != iter_end) {
			out << *iter_begin << " ";
			++iter_begin;
		}
		out << endl;
	}
}

void DTStateManagerImplementationBase::ofcPurge(int threadID) {
	multiset<SetObject<State> >::iterator it;
	for (int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++) {
		this->getStateQueueLock(threadID, i);
		it = myStateQueue[i].begin();
		while (it != myStateQueue[i].end()) {
			delete (*it).getElement();
			myStateQueue[i].erase(it++);
		}
		this->releaseStateQueueLock(threadID, i);
		periodCounter[i] = -1;
	}
}

void DTStateManagerImplementationBase::ofcPurge(unsigned int objId,
		int threadID) {
	multiset<SetObject<State> >::iterator it;
	this->getStateQueueLock(threadID, objId);

	it = myStateQueue[objId].begin();
	while (it != myStateQueue[objId].end()) {
		delete (*it).getElement();
		myStateQueue[objId].erase(it++);
	}
	this->releaseStateQueueLock(threadID, objId);
	periodCounter[objId] = -1;
}

bool DTStateManagerImplementationBase::getStateQueueLock(int threadId,
		int objId) {
	while (!stateQueueLock[objId]->setLock(threadId))
		;
	assert(stateQueueLock[objId]->hasLock(threadId));
}
bool DTStateManagerImplementationBase::releaseStateQueueLock(int threadId,
		int objId) {
	assert(stateQueueLock[objId]->hasLock(threadId));
	stateQueueLock[objId]->releaseLock(threadId);
}
void DTStateManagerImplementationBase::initStateQueueLocks(
		DTTimeWarpSimulationManager *simMgr) {
	for (int i = 0; i < simMgr->getNumberOfSimulationObjects(); i++) {
		stateQueueLock[i] = new AtomicState();
	}
}
const unsigned int DTStateManagerImplementationBase::getEventIdForRollback(
		int threadId, int objId) {
	//    cout <<"Retrived EventId is;;;;;;;;;; " << *ret <<endl;
	unsigned int ret = rollbackEventNumber[objId];
	//	rollbackEventNumber[objId] = 0;
	return ret;
}

const unsigned int DTStateManagerImplementationBase::getSenderObjectIdForRollback(
		int threadId, int objId) {
	return lastRollbackSenderObjectId[objId];
}

const unsigned int DTStateManagerImplementationBase::getSenderObjectSimIdForRollback(
		int threadId, int objId) {
	return lastRollbackSenderObjectSimId[objId];
}
