// See copyright notice in file Copyright in the root directory of this archive.

#include "DTAggressiveOutputManager.h"
#include "DTOutputEvents.h"
#include "DTAggressiveOutputManager.h"
#include "DTTimeWarpSimulationManager.h"
#include "Event.h"

DTAggressiveOutputManager::DTAggressiveOutputManager(
		DTTimeWarpSimulationManager *simMgr) :
	DTOutputManagerImplementationBase(simMgr) {
}

DTAggressiveOutputManager::~DTAggressiveOutputManager() {
}

void DTAggressiveOutputManager::rollback(SimulationObject *object,
		const VTime &rollbackTime, int threadID) {
	DTOutputEvents &outputEvents = getOutputEventsFor(*(object->getObjectID()));
	vector<const Event *> *eventsToCancel =
			outputEvents.getEventsSentAtOrAfterAndRemove(rollbackTime, threadID);

	if (eventsToCancel->size() > 0) {
		getSimulationManager()->cancelEvents(*eventsToCancel);
	}

	delete eventsToCancel;
}
