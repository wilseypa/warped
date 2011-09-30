// See copyright notice in file Copyright in the root directory of this archive.

#include "DTLazyOutputManager.h"
#include "DTTimeWarpSimulationManager.h"
#include "DTOutputEvents.h"
#include "Event.h"
#include "EventFunctors.h"

DTLazyOutputManager::DTLazyOutputManager( DTTimeWarpSimulationManager *simMgr ) :
  DTOutputManagerImplementationBase( simMgr ){
	eventsToCancelAtomicState = new AtomicState *[simMgr->getNumberOfSimulationObjects()];
	lazyQueuesAtomicState = new AtomicState *[simMgr->getNumberOfSimulationObjects()];
	lazyHitCountAtomicState = new AtomicState();
	lazyMissCountAtomicState = new AtomicState();
	compareAndInsertModeAtomicState = new AtomicState();

  for(int i = 0; i < simMgr->getNumberOfSimulationObjects(); i++){
    lazyQueues.push_back( new vector<const Event*>() );
    eventsToCancel.push_back( new vector<const Event*>() );
    compareAndInsertMode.push_back(true);
    lazyHitCount.push_back(0);
    lazyMissCount.push_back(0);
  }
}

DTLazyOutputManager::~DTLazyOutputManager(){
	delete eventsToCancelAtomicState;
	delete lazyQueuesAtomicState;
	delete lazyHitCountAtomicState;
	delete lazyMissCountAtomicState;
	delete compareAndInsertModeAtomicState;

}

void DTLazyOutputManager::getLazyQueuesLock(int threadId, int objId) {
	while (!lazyQueuesAtomicState[objId]->setLock(threadId))
		;
	assert(unprocessedQueueAtomicState[objId]->hasLock(threadId));
}

void DTLazyOutputManager::releaseLazyQueuesLock(int threadId, int objId) {
	assert(lazyQueuesAtomicState[objId]->hasLock(threadId));
	lazyQueuesAtomicState[objId]->releaseLock(threadId);
}

void DTLazyOutputManager::getEventsToCancelLock(int threadId, int objId) {
	while (!eventsToCancelAtomicState[objId]->setLock(threadId))
		;
	assert(eventsToCancelAtomicState[objId]->hasLock(threadId));
}

void DTLazyOutputManager::releaseEventsToCancelLock(int threadId, int objId) {
	assert(eventsToCancelAtomicState[objId]->hasLock(threadId));
	eventsToCancelAtomicState[objId]->releaseLock(threadId);
}

void DTLazyOutputManager::getLazyHitCountLock(int threadId) {
		while (!lazyHitCountAtomicState->setLock(threadId))
			;
		assert(lazyHitCountAtomicState->hasLock(threadId));
}

void DTLazyOutputManager::releaseLazyHitCountLock(int threadId) {
	    assert(lazyHitCountAtomicState->hasLock(threadId));
	    lazyHitCountAtomicState->releaseLock(threadId);
}

void DTLazyOutputManager::getLazyMissCountLock(int threadId) {
		while (!lazyMissCountAtomicState->setLock(threadId))
			;
		assert(lazyMissCountAtomicState->hasLock(threadId));
}

void DTLazyOutputManager::releaseLazyMissCountLock(int threadId) {
	    assert(lazyMissCountAtomicState->hasLock(threadId));
	    lazyMissCountAtomicState->releaseLock(threadId);
}

void DTLazyOutputManager::getCompareAndInsertModeLock(int threadId) {
		while (!compareAndInsertModeAtomicState->setLock(threadId))
			;
		assert(compareAndInsertModeAtomicState->hasLock(threadId));
}

void DTLazyOutputManager::releaseCompareAndInsertModeLock(int threadId) {
	    assert(compareAndInsertModeAtomicState->hasLock(threadId));
	    compareAndInsertModeAtomicState->releaseLock(threadId);
}


bool DTLazyOutputManager::checkLazyCancelEvent(const Event *event, int threadId){

   bool suppressEvent = false;
   int id = getSimulationManager()->getObjectHandle(event->getSender())->getObjectID()->getSimulationObjectID();
   this->getEventsToCancelLock(threadId, id);
   this->getLazyQueuesLock(threadId, id);
   vector<const Event*> *lazyCancelEvents = lazyQueues[id];
   vector<const Event*> *eveToCan = eventsToCancel[id];

   //Perform lazy cancellation when there are still events to be compared to in the
   //lazy cancellation queue.
   if(lazyCancelEvents->size() != 0){
      vector<const Event *>::iterator LCEvent = lazyCancelEvents->begin();

      //Any events with a timestamp less than the current event time
      //were not regenerated. Add them to the cancel events list.
      while(LCEvent != lazyCancelEvents->end() && (*LCEvent)->getSendTime() < event->getSendTime()){
        eveToCan->push_back(*LCEvent);
        lazyCancelEvents->erase(LCEvent);
        this->getLazyMissCountLock(threadId);
        lazyMissCount[id]++;
        this->releaseLazyHitCountLock(threadId);
      }

      //Compare the events in the lazy cancellation queue to this event.
      //If the queue is empty after checking for past time stamps, end lazy cancellation.
      if(lazyCancelEvents->size() > 0){
         LCEvent = lazyCancelEvents->begin();

         while(suppressEvent == false && LCEvent != lazyCancelEvents->end()){

           if(const_cast<Event *>((*LCEvent))->eventCompare(event)){
             if(compareAndInsertMode[id]){
               DTOutputManagerImplementationBase::insert(*LCEvent,threadId);
             }
             suppressEvent = true;
             this->getLazyHitCountLock(threadId);
             lazyHitCount[id]++;
             this->releaseLazyHitCountLock(threadId);
             LCEvent = lazyCancelEvents->erase(LCEvent);
           }
           else{
             LCEvent++;
           }
         }

         if( lazyCancelEvents->size() <= 0 ){
           //End lazy cancellation phase.
           utils::debug << "Lazy Cancellation Phase Complete For Object: " << event->getSender()
                          << " Lazy Hits: " << lazyHitCount[id] << " Lazy Misses: " << lazyMissCount[id] << "\n";
           this->getLazyHitCountLock(threadId);
           lazyHitCount[id] = 0;
           this->releaseLazyHitCountLock(threadId);
           this->getLazyMissCountLock(threadId);
           lazyMissCount[id] = 0;
           this->releaseLazyMissCountLock(threadId);
         }
      }
      else{
        //End lazy cancellation phase.
        utils::debug << "Lazy Cancellation Phase Complete For Object: " << event->getSender()
                       << " Lazy Hits: " << lazyHitCount[id] << " Lazy Misses: " << lazyMissCount[id] << "\n";
        this->getLazyHitCountLock(threadId);
        lazyHitCount[id] = 0;
        this->releaseLazyHitCountLock(threadId);
        this->getLazyMissCountLock(threadId);
        lazyMissCount[id] = 0;
        this->releaseLazyMissCountLock(threadId);
      }
   }
   else{
     // Not in lazy cancellation.
	    this->getLazyHitCountLock(threadId);
	    lazyHitCount[id] = 0;
	    this->releaseLazyHitCountLock(threadId);
	    this->getLazyMissCountLock(threadId);
	    lazyMissCount[id] = 0;
	    this->releaseLazyMissCountLock(threadId);
   }

   this->releaseEventsToCancelLock(threadId, id);
   this->releaseLazyQueuesLock(threadId, id);
   return suppressEvent;
}

void DTLazyOutputManager::handleCancelEvents(int threadId){
  for(int t = 0; t < eventsToCancel.size(); t++){
    if((eventsToCancel[t])->size() > 0){
      this->getEventsToCancelLock(threadId, t);
      getSimulationManager()->cancelEvents(*(eventsToCancel[t]));
      (eventsToCancel[t])->clear();
      this->releaseEventsToCancelLock(threadId, t);
    }
  }
}

void DTLazyOutputManager::handleCancelEvents(SimulationObject *object, int threadId){
  int id = object->getObjectID()->getSimulationObjectID();
  this->getEventsToCancelLock(threadId, id);
  if((eventsToCancel[id])->size() > 0){
    getSimulationManager()->cancelEvents(*(eventsToCancel[id]));
    (eventsToCancel[id])->clear();
  }
  this->releaseEventsToCancelLock(threadId, id);
}

void DTLazyOutputManager::emptyLazyQueues(const VTime &time, int threadId){
  for(int t = 0; t < lazyQueues.size(); t++){
	this->getEventsToCancelLock(threadId, t);
	this->getLazyQueuesLock(threadId, t);
    if( (lazyQueues[t])->size() > 0){
      vector<const Event *>::iterator LCEvent = (lazyQueues[t])->begin();
      while(LCEvent != (lazyQueues[t])->end() && (*LCEvent)->getSendTime() < time){
        (eventsToCancel[t])->push_back(*LCEvent);
        LCEvent = (lazyQueues[t])->erase(LCEvent);
        this->getLazyMissCountLock(threadId);
        lazyMissCount[t]++;
        this->releaseLazyMissCountLock(threadId);
      }
    }
    this->releaseEventsToCancelLock(threadId, t);
    this->releaseLazyQueuesLock(threadId, t);
  }
  handleCancelEvents(threadId);
}

void DTLazyOutputManager::emptyLazyQueue(SimulationObject *object, const VTime &time, int threadId){
  int id = object->getObjectID()->getSimulationObjectID();
  this->getEventsToCancelLock(threadId, id);
  this->getLazyQueuesLock(threadId, id);
  if( (lazyQueues[id])->size() > 0){
    vector<const Event *>::iterator LCEvent = (lazyQueues[id])->begin();
    while(LCEvent != (lazyQueues[id])->end() && (*LCEvent)->getSendTime() < time){
      (eventsToCancel[id])->push_back(*LCEvent);
      LCEvent = (lazyQueues[id])->erase(LCEvent);
      this->getLazyMissCountLock(threadId);
      lazyMissCount[id]++;
      this->releaseLazyMissCountLock(threadId);
    }
  this->releaseEventsToCancelLock(threadId, id);
  this->releaseLazyQueuesLock(threadId, id);
  }
  handleCancelEvents(object, threadId);
}

const VTime &DTLazyOutputManager::getLazyQMinTime(int threadId){
  const VTime *minTime = &getSimulationManager()->getPositiveInfinity();
  const VTime *curTime = NULL;
  for(int objectID = 0; objectID < lazyQueues.size(); objectID++){
	  this->getLazyQueuesLock(threadId, ObjectID);
	  curTime = &getLazyQMinTime(objectID);
      if(*curTime < *minTime){
        minTime = curTime;
      this->releaseLazyQueuesLock(threadId, objectID);
      }
  }
  return (*minTime);
}

const VTime &DTLazyOutputManager::getLazyQMinTime(const unsigned int objectID, int threadId){
  const VTime *minTime = &getSimulationManager()->getPositiveInfinity();
  this->getLazyQueuesLock(threadId, ObjectID);
  if( (lazyQueues[objectID])->size() > 0){
	  minTime = &(lazyQueues[objectID])->front()->getReceiveTime();
  }
  this->releaseLazyQueuesLock(threadId, objectID);
  return (*minTime);
}

bool DTLazyOutputManager::lazyCancel(const Event *event, int threadId){
  SimulationObject *sender = getSimulationManager()->getObjectHandle(event->getSender());
  int id = sender->getObjectID()->getSimulationObjectID();
  bool retval = checkLazyCancelEvent(event, threadId);
  if(retval && compareAndInsertMode[id]){
    //Inserting the original event, reclaim this one.
    sender->reclaimEvent(event);
  }
  else{
    DTOutputManagerImplementationBase::insert(event, threadId);
  }

  handleCancelEvents(sender, threadId);
  return retval;
}

void DTLazyOutputManager::setCompareMode(SimulationObject *obj, bool mode){
  int id = obj->getObjectID()->getSimulationObjectID();
  compareAndInsertMode[id] = mode;
}

bool DTLazyOutputManager::getCompareMode(SimulationObject *obj){
  int id = obj->getObjectID()->getSimulationObjectID();
  return compareAndInsertMode[id];
}

void DTLazyOutputManager::rollback( SimulationObject *object, const VTime &rollbackTime, int threadId ){

  //Put the events that have a timestamp greater than rollback event's
  //timestamp into the Lazy Cancellation Queue to be compared to regenerated output.
  OutputEvents &outputEvents = getOutputEventsFor( *(object->getObjectID()) );
  vector<const Event*> *tempOutEvents = outputEvents.getEventsSentAtOrAfterAndRemove( rollbackTime );
  this->getLazyQueuesLock(threadId, object->getObjectID()->getSimulationObjectID());
  vector<const Event*> *lazyCancelEvents = lazyQueues[object->getObjectID()->getSimulationObjectID()];

  //These output events need to be added to the lazy cancel queue. There may already be
  //events in the queue, so the new ones need to be added.
  lazyCancelEvents->insert(lazyCancelEvents->end(), tempOutEvents->begin(), tempOutEvents->end());
  std::sort(lazyCancelEvents->begin(), lazyCancelEvents->end(), receiveTimeLessThan());

//  outputEvents.remove(*tempOutEvents);
  this->releaseLazyQueuesLock(threadId, object->getObjectID()->getSimulationObjectID());
  delete tempOutEvents;
}

void DTLazyOutputManager::ofcPurge(int threadId){
  // Reset all queues and values.
  for(int i = 0; i < getSimulationManager()->getNumberOfSimulationObjects(); i++){

    this->getLazyQueuesLock(threadId, id);
    lazyQueues[i]->clear();
    this->releaseLazyQueuesLock(threadId, id);
    this->getEventsToCancelLock(threadId, id);
    eventsToCancel[i]->clear();
    this->releaseEventsToCancelLock(threadId, id);
    compareAndInsertMode[i] = true;
    this->getLazyHitCountLock(threadId);
    lazyHitCount[i] = 0;
    this->releaseLazyHitCountLock(threadId);
    this->getLazyMissCountLock(threadId);
    lazyMissCount[i] = 0;
    this->releaseLazyMissCountLock(threadId);

  }

  DTOutputManagerImplementationBase::ofcPurge();
}

