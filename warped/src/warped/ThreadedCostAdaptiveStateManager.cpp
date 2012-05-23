// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedCostAdaptiveStateManager.h"
#include "ThreadedTimeWarpSimulationManager.h"

// These are the default values.
const unsigned int defaultRecalculationPeriod = 100;
const unsigned int defaultAdaptionValue = 1;
const unsigned int maxDefaultInterval = 30;

ThreadedCostAdaptiveStateManager::ThreadedCostAdaptiveStateManager(
		ThreadedTimeWarpSimulationManager *simMgr) :
	ThreadedStateManagerImplementationBase(simMgr, 0),
			stopWatch(simMgr->getNumberOfSimulationObjects()),
			StateSaveTimeWeighted(simMgr->getNumberOfSimulationObjects()),
			CoastForwardTimeWeighted(simMgr->getNumberOfSimulationObjects()) {
	int numSimObjs = simMgr->getNumberOfSimulationObjects();
	eventsBetweenRecalculation.resize(numSimObjs, defaultRecalculationPeriod);
	forwardExecutionLength.resize(numSimObjs, 0);
	adaptionValue.resize(numSimObjs, defaultAdaptionValue);
	oldCostIndex.resize(numSimObjs, 0);
	filteredCostIndex.resize(numSimObjs, 0);
}

void ThreadedCostAdaptiveStateManager::startStateTiming(unsigned int id) {
	stopWatch[id].start();
}

void ThreadedCostAdaptiveStateManager::stopStateTiming(unsigned int id) {
	stopWatch[id].stop();
	StateSaveTimeWeighted[id].update(stopWatch[id].elapsed());
}

void ThreadedCostAdaptiveStateManager::calculatePeriod(SimulationObject *object) {
	OBJECT_ID *currentObjectID = object->getObjectID();
	unsigned int objId = currentObjectID->getSimulationObjectID();

	// The current period for this object.
	int period = objectStatePeriod[objId];

	// Calculate the raw current cost function.
	double stateTime = StateSaveTimeWeighted[objId].getData();
	double coastTime = CoastForwardTimeWeighted[objId].getData();
	double costIndex = stateTime + coastTime;

	// To prevent oscillations, calculate the filtered cost index.
	filteredCostIndex[objId] = 0.4 * filteredCostIndex[objId] + 0.6 * costIndex;

	// When the cost index is 0, continue to increase the period. Otherwise,
	// change the value as specified.
	if (oldCostIndex[objId] > 0) {
		if (oldCostIndex[objId] > 1.2 * filteredCostIndex[objId]) {
			period += adaptionValue[objId];
			oldCostIndex[objId] = filteredCostIndex[objId];
		} else if (oldCostIndex[objId] < 0.8 * filteredCostIndex[objId]) {
			adaptionValue[objId] = -adaptionValue[objId];
			period += adaptionValue[objId];
			oldCostIndex[objId] = filteredCostIndex[objId];
		}
		if (coastTime == 0) {
			adaptionValue[objId] = 1;
			period++;
			oldCostIndex[objId] = filteredCostIndex[objId];
		}
	} else {
		period++;
		oldCostIndex[objId] = filteredCostIndex[objId];
	}

	// Make sure that the period stays in the range: 0 <= period <= 30.
	if (period < 0) {
		period = 0;
	} else if (period > maxDefaultInterval) {
		period = maxDefaultInterval;
	}

	utils::debug << object->getName() << " period: " << period << "\n";

	// Reset values.
	forwardExecutionLength[objId] = 1;
	StateSaveTimeWeighted[objId].reset();
	CoastForwardTimeWeighted[objId].reset();

	objectStatePeriod[objId] = period;
}

void ThreadedCostAdaptiveStateManager::coastForwardTiming(unsigned int id,
		double coastforwardtime) {
	CoastForwardTimeWeighted[id].update(coastforwardtime);
}

double ThreadedCostAdaptiveStateManager::getCoastForwardTime(unsigned int id) {
	return (CoastForwardTimeWeighted[id].getData());
}

void ThreadedCostAdaptiveStateManager::saveState(const VTime& currentTime,
		SimulationObject *object, int threadID) {
	ObjectID *currentObjectID = object->getObjectID();
	unsigned int simObjectID = currentObjectID->getSimulationObjectID();

	// The period is only recalculated after the specified number of events.
	if (forwardExecutionLength[simObjectID]
			< eventsBetweenRecalculation[simObjectID]) {
		forwardExecutionLength[simObjectID]++;
	} else {
		calculatePeriod(object);
	}
	if (periodCounter[simObjectID] <= 1) {
		// we need to first allocate a state. Copy the current state into
		// this newly created state. Then allocate a state object and
		// fill in the current time and a pointer to the newly copied
		// current state.
		startStateTiming(simObjectID);

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
		stopStateTiming(simObjectID);
	} else {
		// decrement period counter
		periodCounter[simObjectID] = periodCounter[simObjectID] - 1;
	}
}

void ThreadedCostAdaptiveStateManager::saveState(const VTime& currentTime,
		unsigned int eventNumber, SimulationObject *object,
		const ObjectID senderId, int threadID) {
	ObjectID *currentObjectID = object->getObjectID();
	unsigned int simObjectID = currentObjectID->getSimulationObjectID();

	// The period is only recalculated after the specified number of events.
	if (forwardExecutionLength[simObjectID]
			< eventsBetweenRecalculation[simObjectID]) {
		forwardExecutionLength[simObjectID]++;
	} else {
		calculatePeriod(object);
	}
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
			ThreadedStateManagerImplementationBase::releaseStateQueueLock(
					threadID, simObjectID);
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

const unsigned int ThreadedCostAdaptiveStateManager::getEventIdForRollback(
		int threadId, int objId) {
	return ThreadedStateManagerImplementationBase::getEventIdForRollback(
			threadId, objId);

}
void ThreadedCostAdaptiveStateManager::updateStateWhileCoastForward(
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
			ThreadedStateManagerImplementationBase::releaseStateQueueLock(
					threadID, simObjectID);
		}
		mySimulationManager->resetRollbackCompletedStatus(simObjectID);
	}
}

const VTime&
ThreadedCostAdaptiveStateManager::restoreState(const VTime &rollbackTime,
		SimulationObject *object, int threadID) {
	OBJECT_ID *currentObjectID = object->getObjectID();
	//unsigned int simObjectID = currentObjectID->getSimulationObjectID();
	//periodCounter[simObjectID] = objectStatePeriod[simObjectID];
	return ThreadedStateManagerImplementationBase::restoreState(rollbackTime,
			object, threadID);
}
