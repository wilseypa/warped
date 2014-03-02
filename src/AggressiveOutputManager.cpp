
#include <vector>                       // for vector

#include "AggressiveOutputManager.h"
#include "OutputEvents.h"               // for OutputEvents
#include "SimulationObject.h"           // for SimulationObject
#include "TimeWarpSimulationManager.h"  // for TimeWarpSimulationManager

class VTime;

AggressiveOutputManager::AggressiveOutputManager(TimeWarpSimulationManager* simMgr) :
    OutputManagerImplementationBase(simMgr) {
}

AggressiveOutputManager::~AggressiveOutputManager() {}

void
AggressiveOutputManager::rollback(SimulationObject* object,
                                  const VTime& rollbackTime) {
    OutputEvents& outputEvents = getOutputEventsFor(*(object->getObjectID()));
    vector<const Event*>* eventsToCancel = outputEvents.getEventsSentAtOrAfterAndRemove(rollbackTime);

    if (eventsToCancel->size() > 0) {
        getSimulationManager()->cancelEvents(*eventsToCancel);
    }

    delete eventsToCancel;
}
