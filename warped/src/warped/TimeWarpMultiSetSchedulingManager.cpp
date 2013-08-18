
#include "TimeWarpMultiSetSchedulingManager.h"
#include "warped/TimeWarpSimulationManager.h"
#include "warped/Event.h"

TimeWarpMultiSetSchedulingManager::TimeWarpMultiSetSchedulingManager( TimeWarpSimulationManager *initSimulationManager )
  : mySimulationManager( initSimulationManager ),
    lastScheduledTime( initSimulationManager->getZero().clone() ){}

TimeWarpMultiSetSchedulingManager::~TimeWarpMultiSetSchedulingManager(){}

const Event *
TimeWarpMultiSetSchedulingManager::peekNextEvent(){
  TimeWarpEventSet *eventSet = mySimulationManager->getEventSetManager();
  ASSERT( eventSet != 0 );
  
  const Event *earliestEvent = eventSet->peekEvent(NULL);

  return earliestEvent;
}

const VTime &
TimeWarpMultiSetSchedulingManager::getLastEventScheduledTime(){
  const Event *earliestEvent = peekNextEvent();
  if( earliestEvent != 0 ){
    lastScheduledTime = &earliestEvent->getReceiveTime();
  }
  else{
    lastScheduledTime = &mySimulationManager->getPositiveInfinity();
  }
  ASSERT( lastScheduledTime != 0 );
  return *lastScheduledTime;
}
