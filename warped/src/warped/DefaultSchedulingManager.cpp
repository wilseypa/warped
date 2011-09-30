// See copyright notice in file Copyright in the root directory of this archive.

#include "DefaultSchedulingManager.h"
#include "warped/TimeWarpSimulationManager.h"
#include "warped/Event.h"

DefaultSchedulingManager::DefaultSchedulingManager( TimeWarpSimulationManager *initSimulationManager ) 
  : mySimulationManager( initSimulationManager ),
    lastScheduledTime( initSimulationManager->getZero().clone() ){}

DefaultSchedulingManager::~DefaultSchedulingManager(){}

const Event *
DefaultSchedulingManager::peekNextEvent(){
  TimeWarpEventSet *eventSet = mySimulationManager->getEventSetManager();
  ASSERT( eventSet != 0 );

  const Event *earliestEvent = 0;
  int simMgrId = mySimulationManager->getSimulationManagerID();
  for(int i = 0; i < mySimulationManager->getNumberOfSimulationObjects(); i++){
    const Event *tempEvent = eventSet->peekEvent( mySimulationManager->getObjectHandle( ObjectID(i,simMgrId) ) );
    if( earliestEvent != 0 && tempEvent != 0 ){
      if( tempEvent->getReceiveTime() < earliestEvent->getReceiveTime() ){
        earliestEvent = tempEvent;
      }
      else if(tempEvent->getReceiveTime() == earliestEvent->getReceiveTime()){
        if(tempEvent->getEventId() < earliestEvent->getEventId()){
          earliestEvent = tempEvent;
        }
      }
    } 
    else if( earliestEvent == 0 && tempEvent != 0 ){
      earliestEvent = tempEvent;      
    }
  }

  if( earliestEvent != 0 ){
    setLastScheduledEventTime( earliestEvent->getReceiveTime() );
  }
  else{
    setLastScheduledEventTime( mySimulationManager->getPositiveInfinity() );
  }

  return earliestEvent;
}

void
DefaultSchedulingManager::setLastScheduledEventTime( const VTime &newTime ){
  delete lastScheduledTime;
  lastScheduledTime = newTime.clone();
}


const VTime &
DefaultSchedulingManager::getLastEventScheduledTime(){
  peekNextEvent();
  ASSERT( lastScheduledTime != 0 );
  return *lastScheduledTime;
}
