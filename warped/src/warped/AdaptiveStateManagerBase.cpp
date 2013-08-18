
#include "AdaptiveStateManagerBase.h"
#include "State.h"
#include "TimeWarpSimulationManager.h"
#include "Event.h"

AdaptiveStateManagerBase::AdaptiveStateManagerBase(TimeWarpSimulationManager *simMgr):
   StateManagerImplementationBase(simMgr,0),
   eventTime( simMgr->getNumberOfSimulationObjects(), 0 ),
   lastRollbackCount( simMgr->getNumberOfSimulationObjects(), -1 ),
   stopWatch( simMgr->getNumberOfSimulationObjects() ),
   eventStopWatch( simMgr->getNumberOfSimulationObjects() ),
   rollbackStopWatch( simMgr->getNumberOfSimulationObjects() ),
   stateStartTime( simMgr->getNumberOfSimulationObjects(), 0 ),
   stateStopTime( simMgr->getNumberOfSimulationObjects(), 0 ),
   rollbackTime( simMgr->getNumberOfSimulationObjects(), 0 ),
   pausedTime( simMgr->getNumberOfSimulationObjects(), 0 ),
   steadyState( simMgr->getNumberOfSimulationObjects(), false ),
   stateSaveTime( simMgr->getNumberOfSimulationObjects() ),
   eventExecutionTime( simMgr->getNumberOfSimulationObjects() ),
   sumEventTime( simMgr->getNumberOfSimulationObjects(), 0 ),
   coastForwardTime( simMgr->getNumberOfSimulationObjects() ),
   rollbackLength( simMgr->getNumberOfSimulationObjects() ),
   StateSaveTimeWeighted( simMgr->getNumberOfSimulationObjects() ),
   EventExecutionTimeWeighted( simMgr->getNumberOfSimulationObjects() ),
   CoastForwardTimeWeighted( simMgr->getNumberOfSimulationObjects() ),
   rollbackLengthWeighted( simMgr->getNumberOfSimulationObjects() ){
}

void AdaptiveStateManagerBase::startEventTiming(unsigned int id) {
   pausedTime[id] = 0;
   stopWatch[id].start();
}

bool AdaptiveStateManagerBase::stopEventTiming(unsigned int id) {
   stopWatch[id].stop();
   eventTime[id] = stopWatch[id].elapsed() - pausedTime[id];
   return ( executionTiming(id, eventTime[id]) ); 
}


void AdaptiveStateManagerBase::pauseEventTiming(unsigned int id) {
   eventStopWatch[id].start();
}

void AdaptiveStateManagerBase::resumeEventTiming(unsigned int id) {
   eventStopWatch[id].stop();
   pausedTime[id] = eventStopWatch[id].elapsed();
}

void AdaptiveStateManagerBase::startRollbackTiming(unsigned int id) {
   rollbackStopWatch[id].start();
}

void AdaptiveStateManagerBase::finishRollbackTiming(unsigned int id) {
   rollbackStopWatch[id].stop();
   rollbackTime[id] += rollbackStopWatch[id].elapsed();
}

double AdaptiveStateManagerBase::getEventExecutionTime(unsigned int id) {
   return(EventExecutionTimeWeighted[id].getData());
}

double AdaptiveStateManagerBase::getStateSavingTime(unsigned int id) {
   return(StateSaveTimeWeighted[id].getData());
}

double AdaptiveStateManagerBase::getCoastForwardTime(unsigned int id) {
   return(CoastForwardTimeWeighted[id].getData());
}

void AdaptiveStateManagerBase::startStateTiming(unsigned int id) {
   stopWatch[id].start();
}

void AdaptiveStateManagerBase::stopStateTiming(unsigned int id) { 
   stopWatch[id].stop();
   StateSaveTimeWeighted[id].update ( stopWatch[id].elapsed() );
}

bool AdaptiveStateManagerBase::executionTiming(unsigned int id, double execTime) { 

   // NOTE: This is the execution time for the PREVIOUS RUN. We sum the 
   // values until we move to the next LVT value and then save the sum.
   EventExecutionTimeWeighted[id].update( sumEventTime[id] );

   // Add the time required for any rollbacks to the time to execute 
   // this event.
   sumEventTime[id] = execTime + rollbackTime[id];
   rollbackTime[id] = 0;

   return((bool)!steadyState[id]);
}

void AdaptiveStateManagerBase::saveState(const VTime& currentTime, SimulationObject *object) {
   // This method should be the same as in PeriodicStateManager except
   // for the timing calls.

   // store this object's id temporarily
   OBJECT_ID *currentObjectID = object->getObjectID();
   unsigned int simObjectID = currentObjectID->getSimulationObjectID();
   
   if(periodCounter[simObjectID] <= 0){
      if(!steadyState[simObjectID]){
         startStateTiming(simObjectID);
          
         // we need to first allocate a state. Copy the current state into
         // this newly created state. Then allocate a state object and
         // fill in the current time and a pointer to the newly copied
         // current state.
         State *newState = object->allocateState();
         newState->copyState(object->getState());

         SetObject<State> stateObject(currentTime, newState);

         // look up the state queue of the simulation object and insert the
         // newly allocated state object
         myStateQueue[simObjectID].insert(stateObject);
         // reset period counter to state period
         periodCounter[simObjectID] = objectStatePeriod[simObjectID];

         stopStateTiming(simObjectID);
      }
      else {
         State *newState = object->allocateState();
         newState->copyState(object->getState());

         SetObject<State> stateObject(currentTime, newState);

         myStateQueue[simObjectID].insert(stateObject);
         
         periodCounter[simObjectID] = objectStatePeriod[simObjectID];
      }
   }
   else {
      // decrement period counter
      periodCounter[simObjectID] = periodCounter[simObjectID] - 1;
   }
}
