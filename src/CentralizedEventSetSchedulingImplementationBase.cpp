
#include "SimulationObject.h"
#include "TimeWarpSimulationManager.h"
#include "TimeWarpCentralizedEventSet.h"
#include "CentralizedEventSetSchedulingImplementationBase.h"
#include "Event.h"

CentralizedEventSetSchedulingImplementationBase::CentralizedEventSetSchedulingImplementationBase(SimulationManager *simMgr) :
   CentralizedEventSetSchedulingManager(simMgr) {}

CentralizedEventSetSchedulingImplementationBase::~CentralizedEventSetSchedulingImplementationBase() {}


const VTime &
CentralizedEventSetSchedulingImplementationBase::getLastEventScheduledTime(){
  return *lastScheduledEventTime;
}

const Event *
CentralizedEventSetSchedulingImplementationBase::getEventToSchedule(){
  return myEventSetHandle->peekEvent(NULL);
}

void
CentralizedEventSetSchedulingImplementationBase::runProcesses( const Event *event ){
   SimulationObject *scheduledSimulationObject = NULL;

   ASSERT(mySimulationManager != NULL);
   scheduledSimulationObject = mySimulationManager->getObjectHandle( event->getReceiver() );
   ASSERT(scheduledSimulationObject != NULL);
   scheduledSimulationObject->executeProcess();
   delete lastScheduledEventTime;
   lastScheduledEventTime = event->getReceiveTime().clone();
}


