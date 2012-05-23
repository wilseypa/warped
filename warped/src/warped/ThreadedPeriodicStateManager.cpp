// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedPeriodicStateManager.h"
#include "SimulationObject.h"
#include "State.h"
#include "TimeWarpSimulationManager.h"
#include "SchedulingManager.h"
#include "Event.h"
#include "OptFossilCollManager.h"

ThreadedPeriodicStateManager::ThreadedPeriodicStateManager(
		ThreadedTimeWarpSimulationManager *simMgr, unsigned int period) :
	ThreadedStateManagerImplementationBase(simMgr, period) {
}

ThreadedPeriodicStateManager::~ThreadedPeriodicStateManager() {
}

const VTime&
ThreadedPeriodicStateManager::restoreState(const VTime &rollbackTime,
		SimulationObject *object, int threadID) {
	OBJECT_ID *currentObjectID = object->getObjectID();
	//unsigned int simObjectID = currentObjectID->getSimulationObjectID();
	//periodCounter[simObjectID] = objectStatePeriod[simObjectID];
	return ThreadedStateManagerImplementationBase::restoreState(rollbackTime, object,
			threadID);
}

void ThreadedPeriodicStateManager::saveState(const VTime& currentTime,
		unsigned int eventNumber, SimulationObject *object,
		const ObjectID senderId, int threadID) {
	// store this object's id temporarily
	OBJECT_ID *currentObjectID = object->getObjectID();
	unsigned int simObjectID = currentObjectID->getSimulationObjectID();
	if (mySimulationManager->isRollbackJustCompleted(simObjectID)) {
	//	cout << "Last Rollback Time ::{{{{{{{{{{{{{{{{{}}}}}}}} "
	//			<< *(lastRollbackTime[simObjectID]) << endl;
		multiset<SetObject<State> >::iterator iter_end =
				myStateQueue[simObjectID].end();
		/*cout << "Current Num = " << eventNumber << ":::::  Old Number = "
				<< rollbackEventNumber[simObjectID] << endl;*/
		if (*(lastRollbackTime[simObjectID]) == currentTime && eventNumber
				<= rollbackEventNumber[simObjectID]) {
	//		cout << " Need to update the Event Number in the state ::::"
	//				<< eventNumber << endl;
			ThreadedStateManagerImplementationBase::getStateQueueLock(threadID,
					simObjectID);
			State *newState = object->allocateState();
			newState->copyState(object->getState());
			SetObject<State> stateObject(currentTime, newState, eventNumber,
					senderId.getSimulationObjectID(),
					senderId.getSimulationObjectID());
			//object->deallocateState((*iter_end).getElement());
			//myStateQueue[simObjectID].erase(iter_end);
			// look up the state queue of the simulation object and insert the
			// newly allocated state object

			myStateQueue[simObjectID].insert(stateObject);
			//		cout << "Saving at the Time Stamp :::::"
			//				<< currentTime.getApproximateIntTime() << endl;
			//		cout << "Imm. After savig the EventId ::::;"
			//				<< stateObject.getEventNumber() << endl;
			// *iter_end = stateObject;
			ThreadedStateManagerImplementationBase::releaseStateQueueLock(threadID,
					simObjectID);
		}
		mySimulationManager->resetRollbackCompletedStatus(simObjectID);
		return;
	}
	if (periodCounter[simObjectID] <= 0) {
		// we need to first allocate a state. Copy the current state into
		// this newly created state. Then allocate a state object and
		// fill in the current time and a pointer to the newly copied
		// current state.
		State *newState = object->allocateState();
		newState->copyState(object->getState());
		SetObject<State> stateObject(currentTime, newState, eventNumber,
				senderId.getSimulationObjectID(),
				senderId.getSimulationObjectID());

		// look up the state queue of the simulation object and insert the
		// newly allocated state object
		ThreadedStateManagerImplementationBase::getStateQueueLock(threadID,
				simObjectID);
		myStateQueue[simObjectID].insert(stateObject);
	//	cout << "Saving at the Time Stamp :::::"
	//			<< currentTime.getApproximateIntTime()
	//			<< "  With Event Number :::: " << eventNumber
	//			<< "  With senderObjectId :::: "
	//			<< senderId.getSimulationObjectID() << endl;

		multiset<SetObject<State> >::iterator iter_end =
				myStateQueue[simObjectID].end();
		iter_end--;
		/*cout << "Imm. After saving the EventId ::::::: "
				<< (*iter_end).getEventNumber() << endl;
		cout << "Imm. After saving the SenderId ::::::: "
				<< (*iter_end).getsenderObjectId() << endl;*/

		ThreadedStateManagerImplementationBase::releaseStateQueueLock(threadID,
				simObjectID);
		// reset period counter to state period
		periodCounter[simObjectID] = objectStatePeriod[simObjectID];
	} else {
		// decrement period counter
		periodCounter[simObjectID] = periodCounter[simObjectID] - 1;
	}
}
void ThreadedPeriodicStateManager::saveState(const VTime& currentTime,
		SimulationObject *object, int threadID) {
	// store this object's id temporarily
	OBJECT_ID *currentObjectID = object->getObjectID();
	unsigned int simObjectID = currentObjectID->getSimulationObjectID();

	if (periodCounter[simObjectID] <= 1) {
		// we need to first allocate a state. Copy the current state into
		// this newly created state. Then allocate a state object and
		// fill in the current time and a pointer to the newly copied
		// current state.

		State *newState = object->allocateState();
		newState->copyState(object->getState());
		SetObject<State> stateObject(currentTime, newState);

		// look up the state queue of the simulation object and insert the
		// newly allocated state object
		ThreadedStateManagerImplementationBase::getStateQueueLock(threadID,
				simObjectID);
		myStateQueue[simObjectID].insert(stateObject);
		/*cout << "Saving at the Time Stamp :::::"
		 << currentTime.getApproximateIntTime() << endl;*/
		ThreadedStateManagerImplementationBase::releaseStateQueueLock(threadID,
				simObjectID);

		// reset period counter to state period
		periodCounter[simObjectID] = objectStatePeriod[simObjectID];
	} else {
		// decrement period counter
		periodCounter[simObjectID] = periodCounter[simObjectID] - 1;
	}
}
const unsigned int ThreadedPeriodicStateManager::getEventIdForRollback(int threadId,
		int objId) {
	//    const EventId *ret;
	//    this->getStateQueueLock(threadId,objId);
	//    multiset<SetObject<State> >::iterator iter_begin = myStateQueue[objId].begin();
	//    multiset<SetObject<State> >::iterator iter_end = myStateQueue[objId].end();
	//    if (iter_end != iter_begin)
	//        iter_end--;
	//    ret=((*iter_end).getEventId());
	//    this->releaseStateQueueLock(threadId, objId);
	//    return ret;
	return ThreadedStateManagerImplementationBase::getEventIdForRollback(threadId,
			objId);

}
void ThreadedPeriodicStateManager::updateStateWhileCoastForward(
		const VTime& currentTime, unsigned int eventNumber,
		SimulationObject *object, const ObjectID senderId, int threadID) {
	// store this object's id temporarily
	OBJECT_ID *currentObjectID = object->getObjectID();
	unsigned int simObjectID = currentObjectID->getSimulationObjectID();
	if (mySimulationManager->isRollbackJustCompleted(simObjectID)) {
		/*cout << "Last Rollback Time ::{{{{{{{{{{{{{{{{{}}}}}}}} "
				<< *(lastRollbackTime[simObjectID]) << endl;*/
		multiset<SetObject<State> >::iterator iter_end =
				myStateQueue[simObjectID].end();
		/*cout << "Current Num = " << eventNumber << ":::::  Old Number = "
				<< rollbackEventNumber[simObjectID] << endl;*/
		if (*(lastRollbackTime[simObjectID]) == currentTime && eventNumber
				<= rollbackEventNumber[simObjectID]) {
			/*cout << " Need to update the Event Number in the state ::::"
					<< eventNumber << endl;*/
			ThreadedStateManagerImplementationBase::getStateQueueLock(threadID,
					simObjectID);
			State *newState = object->allocateState();
			newState->copyState(object->getState());
			SetObject<State> stateObject(currentTime, newState, eventNumber,
					senderId.getSimulationObjectID(),
					senderId.getSimulationObjectID());
			//object->deallocateState((*iter_end).getElement());
			//myStateQueue[simObjectID].erase(iter_end);
			// look up the state queue of the simulation object and insert the
			// newly allocated state object

			myStateQueue[simObjectID].insert(stateObject);
			ThreadedStateManagerImplementationBase::releaseStateQueueLock(threadID,
					simObjectID);
		}
		mySimulationManager->resetRollbackCompletedStatus(simObjectID);
	}
}
