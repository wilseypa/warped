// See copyright notice in file Copyright in the root directory of this archive.

#include "DTPeriodicStateManager.h"
#include "SimulationObject.h"
#include "State.h"
#include "TimeWarpSimulationManager.h"
#include "SchedulingManager.h"
#include "Event.h"
#include "OptFossilCollManager.h"

DTPeriodicStateManager::DTPeriodicStateManager(
		DTTimeWarpSimulationManager *simMgr, unsigned int period) :
	DTStateManagerImplementationBase(simMgr, period) {
}

DTPeriodicStateManager::~DTPeriodicStateManager() {
}

const VTime&
DTPeriodicStateManager::restoreState(const VTime &rollbackTime,
		SimulationObject *object, int threadID) {
	OBJECT_ID *currentObjectID = object->getObjectID();
	unsigned int simObjectID = currentObjectID->getSimulationObjectID();
	//periodCounter[simObjectID] = objectStatePeriod[simObjectID];
	return DTStateManagerImplementationBase::restoreState(rollbackTime, object,
			threadID);
}

void DTPeriodicStateManager::saveState(const VTime& currentTime,
		SimulationObject *object, int threadID) {
	// store this object's id temporarily
	OBJECT_ID *currentObjectID = object->getObjectID();
	unsigned int simObjectID = currentObjectID->getSimulationObjectID();

	if (periodCounter[simObjectID] <= 0) {
		// we need to first allocate a state. Copy the current state into
		// this newly created state. Then allocate a state object and
		// fill in the current time and a pointer to the newly copied
		// current state.

		State *newState = object->allocateState();
		// No longer used for optimistic fossil collection.
		/*if(mySimulationManager->getOptFossilColl()){
		 newState = mySimulationManager->getFossilCollManager()->newState(object);
		 }
		 else{
		 newState = object->allocateState();
		 }*/
		newState->copyState(object->getState());

		SetObject<State> stateObject(currentTime, newState);

		// look up the state queue of the simulation object and insert the
		// newly allocated state object
		DTStateManagerImplementationBase::getStateQueueLock(threadID, simObjectID);
		myStateQueue[simObjectID].insert(stateObject);
		DTStateManagerImplementationBase::releaseStateQueueLock(threadID, simObjectID);

		// reset period counter to state period
		periodCounter[simObjectID] = objectStatePeriod[simObjectID];
	} else {
		// decrement period counter
		periodCounter[simObjectID] = periodCounter[simObjectID] - 1;
	}
}
