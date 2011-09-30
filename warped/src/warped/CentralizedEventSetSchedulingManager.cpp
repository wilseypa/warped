// See copyright notice in file Copyright in the root directory of this archive.

#include "CentralizedEventSetSchedulingManager.h"

CentralizedEventSetSchedulingManager::CentralizedEventSetSchedulingManager(SimulationManager* simMgr) 
  : mySimulationManager(simMgr), 
    myEventSetHandle(NULL), 
    simulationObjectsArray(NULL), 
    lastScheduledEventTime( 0 ){}

CentralizedEventSetSchedulingManager::~CentralizedEventSetSchedulingManager() {
  delete simulationObjectsArray;
  delete lastScheduledEventTime;
}

void
CentralizedEventSetSchedulingManager::initialize(TimeWarpEventSet *eventSet,
                                                vector<SimulationObject*>*
                                                simObjArray){
   myEventSetHandle = (TimeWarpCentralizedEventSet *)eventSet;
   ASSERT(myEventSetHandle != NULL);
   // ownership of scheduler's list is transferred here.
   // the scheduler is now responsible for deleting this memory
   simulationObjectsArray = simObjArray;
}
