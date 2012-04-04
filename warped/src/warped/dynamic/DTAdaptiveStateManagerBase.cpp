// See copyright notice in file Copyright in the root directory of this archive.

#include "DTAdaptiveStateManagerBase.h"
#include "State.h"
#include "DTTimeWarpSimulationManager.h"
#include "Event.h"

DTAdaptiveStateManagerBase::DTAdaptiveStateManagerBase(
		DTTimeWarpSimulationManager *simMgr) :
	DTStateManagerImplementationBase(simMgr, 0),
			stopWatch(simMgr->getNumberOfSimulationObjects()),
			steadyState(simMgr->getNumberOfSimulationObjects(), false),
			coastForwardTime(simMgr->getNumberOfSimulationObjects()),
			StateSaveTimeWeighted(simMgr->getNumberOfSimulationObjects()),
			CoastForwardTimeWeighted(simMgr->getNumberOfSimulationObjects()) {
}

void DTAdaptiveStateManagerBase::startStateTiming(unsigned int id) {
	stopWatch[id].start();
}

void DTAdaptiveStateManagerBase::stopStateTiming(unsigned int id) {
	stopWatch[id].stop();
	StateSaveTimeWeighted[id].update(stopWatch[id].elapsed());
}

void DTAdaptiveStateManagerBase::saveState(const VTime& currentTime,
		SimulationObject *object, int threadId) {
	// This method should be the same as in PeriodicStateManager except
	// for the timing calls.
	// store this object's id temporarily
	OBJECT_ID *currentObjectID = object->getObjectID();
	unsigned int simObjectID = currentObjectID->getSimulationObjectID();

	if (periodCounter[simObjectID] <= 0) {
		if (!steadyState[simObjectID]) {
			startStateTiming(simObjectID);

			// we need to first allocate a state. Copy the current state into
			// this newly created state. Then allocate a state object and
			// fill in the current time and a pointer to the newly copied
			// current state.
			State *newState = object->allocateState();
			newState->copyState(object->getState());

			SetObject<State> stateObject(currentTime, newState);

			// look up the state queue of the simulation object and insert the
			// newly allocated state object
			this->getStateQueueLock(threadId, simObjectID);
			myStateQueue[simObjectID].insert(stateObject);
			this->releaseStateQueueLock(threadId, simObjectID);
			// reset period counter to state period
			periodCounter[simObjectID] = objectStatePeriod[simObjectID];

			stopStateTiming(simObjectID);
		} else {
			State *newState = object->allocateState();
			newState->copyState(object->getState());

			SetObject<State> stateObject(currentTime, newState);

			this->getStateQueueLock(threadId, simObjectID);
			myStateQueue[simObjectID].insert(stateObject);
			this->releaseStateQueueLock(threadId, simObjectID);

			periodCounter[simObjectID] = objectStatePeriod[simObjectID];
		}
	} else {
		// decrement period counter
		periodCounter[simObjectID] = periodCounter[simObjectID] - 1;
	}
}
