
#include <stddef.h>                     // for NULL

#include "CentralizedEventSetSchedulingManager.h"
#include "VTime.h"                      // for VTime
#include "warped.h"                     // for ASSERT

class TimeWarpEventSet;

CentralizedEventSetSchedulingManager::CentralizedEventSetSchedulingManager(
    SimulationManager* simMgr)
    : mySimulationManager(simMgr),
      myEventSetHandle(NULL),
      simulationObjectsArray(NULL),
      lastScheduledEventTime(0) {}

CentralizedEventSetSchedulingManager::~CentralizedEventSetSchedulingManager() {
    delete simulationObjectsArray;
    delete lastScheduledEventTime;
}

void
CentralizedEventSetSchedulingManager::initialize(TimeWarpEventSet* eventSet,
                                                 vector<SimulationObject*>*
                                                 simObjArray) {
    myEventSetHandle = (TimeWarpCentralizedEventSet*)eventSet;
    ASSERT(myEventSetHandle != NULL);
    // ownership of scheduler's list is transferred here.
    // the scheduler is now responsible for deleting this memory
    simulationObjectsArray = simObjArray;
}
