// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedAggressiveOutputManager.h"
#include "ThreadedOutputEvents.h"
#include "ThreadedAggressiveOutputManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "Event.h"

ThreadedAggressiveOutputManager::ThreadedAggressiveOutputManager(
		ThreadedTimeWarpSimulationManager *simMgr) :
	ThreadedOutputManagerImplementationBase(simMgr) {
}

ThreadedAggressiveOutputManager::~ThreadedAggressiveOutputManager() {
}

void ThreadedAggressiveOutputManager::rollback(SimulationObject *object,
		const VTime &rollbackTime, int threadID) {
	ThreadedOutputEvents &outputEvents = getOutputEventsFor(
			*(object->getObjectID()));
	vector<const Event *>
			*eventsToCancel = outputEvents.getEventsSentAtOrAfterAndRemove(
					rollbackTime, threadID);

	if (eventsToCancel->size() > 0) {
		getSimulationManager()->cancelEvents(*eventsToCancel);
	}

	delete eventsToCancel;
}
