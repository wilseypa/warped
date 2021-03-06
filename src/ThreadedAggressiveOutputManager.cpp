
#include <vector>                       // for vector

#include "SimulationObject.h"           // for SimulationObject
#include "ThreadedAggressiveOutputManager.h"
#include "ThreadedOutputEvents.h"       // for ThreadedOutputEvents
#include "TimeWarpSimulationManager.h"  // for TimeWarpSimulationManager

class ThreadedTimeWarpSimulationManager;
class VTime;

ThreadedAggressiveOutputManager::ThreadedAggressiveOutputManager(
    ThreadedTimeWarpSimulationManager* simMgr) :
    ThreadedOutputManagerImplementationBase(simMgr) {
}

ThreadedAggressiveOutputManager::~ThreadedAggressiveOutputManager() {
}

void ThreadedAggressiveOutputManager::rollback(SimulationObject* object,
                                               const VTime& rollbackTime, int threadID) {
    ThreadedOutputEvents& outputEvents = getOutputEventsFor(
                                             *(object->getObjectID()));
    vector<const Event*>
    * eventsToCancel = outputEvents.getEventsSentAtOrAfterAndRemove(
                           rollbackTime, threadID);

    if (eventsToCancel->size() > 0) {
        getSimulationManager()->cancelEvents(*eventsToCancel);
    }

    delete eventsToCancel;
}
