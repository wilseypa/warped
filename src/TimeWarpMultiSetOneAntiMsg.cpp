
#include "NegativeEvent.h"
#include "SimulationObject.h"
#include "TimeWarpMultiSetOneAntiMsg.h"
#include "TimeWarpSimulationManager.h"

TimeWarpMultiSetOneAntiMsg::TimeWarpMultiSetOneAntiMsg( TimeWarpSimulationManager *initSimulationManager ):
  TimeWarpMultiSet( initSimulationManager ){
}

TimeWarpMultiSetOneAntiMsg::~TimeWarpMultiSetOneAntiMsg(){
}

bool
TimeWarpMultiSetOneAntiMsg::handleAntiMessage( SimulationObject *object, const NegativeEvent *negEvent ){

  ASSERT(negEvent != NULL);
  ASSERT(object != NULL);

  unsigned int objId = object->getObjectID()->getSimulationObjectID();

  bool eventWasRemoved = false;
  bool updateLowObjIt = false;
  multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator it;
  multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator temp;
  it = unprocessedObjEvents[objId]->begin();

  while(it != unprocessedObjEvents[objId]->end()){
    if((*it)->getSender() == negEvent->getSender() &&
       (*it)->getSendTime() >= negEvent->getSendTime()){

      const Event *eventToRemove = *it;

      // Update the insertion iterator to point to the event preceding the
      // event to be removed. If the event is at the beginning, set the insertion
      // iterator to the beginning.
      if(it == insertObjPos[objId]){
        if(insertObjPos[objId] == unprocessedObjEvents[objId]->begin()){
          insertObjPos[objId]++;
        }
        else{
          insertObjPos[objId]--;
        }
      }

      // The iterator will be invalidated after the erase, so use this temp
      // iterator to erase while incrementing the other iterator.
      temp = it;
      it++;
      unprocessedObjEvents[objId]->erase(temp);

      // Update the lowest object event if it was just removed.
      if(lowObjPos[objId] != lowestObjEvents.end()){
        if(*eventToRemove == **(lowObjPos[objId])){
          lowestObjEvents.erase(lowObjPos[objId]);
          lowObjPos[objId] = lowestObjEvents.end();
          updateLowObjIt = true;
        }
      }

      // Put the removed event here in case it needs to be used for comparisons in
      // lazy cancellation.
      removedEvents[objId]->push_back(eventToRemove);
      eventWasRemoved = true;
    }
    else{
      it++;
    }
  }

  // Only update the lowest object event if the previous one was erased.
  if(updateLowObjIt){
    if(!(unprocessedObjEvents[objId]->empty())){
      lowObjPos[objId] = lowestObjEvents.insert(*(unprocessedObjEvents[objId]->begin()));
    }
    else{
      lowObjPos[objId] = lowestObjEvents.end();
    }
  }

  return eventWasRemoved;
}
