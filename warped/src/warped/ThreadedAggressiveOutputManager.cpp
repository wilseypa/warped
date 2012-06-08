// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedAggressiveOutputManager.h"
#include "ThreadedOutputEvents.h"
#include "ThreadedAggressiveOutputManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "Event.h"

ThreadedAggressiveOutputManager::ThreadedAggressiveOutputManager(
		ThreadedTimeWarpSimulationManager *simMgr) :
	ThreadedOutputManagerImplementationBase(simMgr), numberOfAntiMessage(0) {
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
		__sync_fetch_and_add(&numberOfAntiMessage, eventsToCancel->size());
		getSimulationManager()->cancelEvents(*eventsToCancel);
	}

	delete eventsToCancel;
}

unsigned int ThreadedAggressiveOutputManager::getNumberOfAntiMessage() {
	return numberOfAntiMessage;
}
