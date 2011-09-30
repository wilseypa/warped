// See copyright notice in file Copyright in the root directory of this archive.

#include "TimeWarpAppendQueue.h"
#include "Event.h"
#include "SimulationObject.h"
#include "DummyEvent.h"
#include "TimeWarpSimulationManager.h"
using std::cerr;
using std::endl;

TimeWarpAppendQueue::TimeWarpAppendQueue( TimeWarpSimulationManager *initSimulationManager ) {
  senderQMap = new typeSenderMap;
  processedQ = new list<Event*>;
  sortedUnProcessedQ = new list<Event*>;
  scheduleList = new vector<ScheduleListContainer*>;
  peekedEvent=0;
  makeHeapFlag=false;
  popHeapFlag = false;
  mySimulationManager = initSimulationManager;
}


TimeWarpAppendQueue::~TimeWarpAppendQueue() {

  processedQ->clear();
  sortedUnProcessedQ->clear();
  scheduleList->clear();

  delete senderQMap;
  delete processedQ;
  delete sortedUnProcessedQ;
  delete scheduleList;
}

void
TimeWarpAppendQueue::initSenderQueueEntry( const string &senderId ){
  //Add the new sender's event
  SenderQueueContainer *senderQContainer = new SenderQueueContainer;
  
  senderQMap->insert( senderId, senderQContainer );
  senderQContainer->headEventPtr =  DummyEvent::instance();
  ScheduleListContainer *scheduleListContainer = 
    new ScheduleListContainer(&(senderQContainer->headEventPtr));
  
  scheduleList->insert(scheduleList->end(), scheduleListContainer);
}

bool
TimeWarpAppendQueue::insert( Event * event, SimulationObject* object) {
  ASSERT(event != NULL);
  ASSERT(object != NULL);

  const string &senderId = event->getSender();

  VTime receiveTime = event->getReceiveTime();

  SenderQueueContainer *senderQContainer = senderQMap->find( senderId );
  if( senderQContainer == 0 ){
    initSenderQueueEntry( senderId );
    senderQContainer = senderQMap->find( senderId );
    ASSERT( senderQContainer != 0 );
  }

  // The simulation Time is modified by the simulation manager if the event
  // to be inserted is a straggler.  The event is in the past, if the event
  // to be inserted is <= to the last event processed by the receiver
  // object.
  bool inThePast = isInThePast(event);
  if( inThePast == true ){
    // push_back all events in processedQ with receiveTime > 
    while( !processedQ->empty() && processedQ->front()->getReceiveTime() >= receiveTime ){
      sortedUnProcessedQ->push_back( processedQ->front() );
      processedQ->pop_front();
    }
    // Insert the new event in the correct place in the sender Q
    sortedUnProcessedQ->push_back(event);
    
    VTime simulationTime = object->getSimulationTime();

    // The State saving may have period != 1 So add all the events greater
    // than the simulation time into senderQ
    while( !processedQ->empty() && processedQ->front()->getReceiveTime() > simulationTime ){
      sortedUnProcessedQ->push_back( processedQ->front() );
      processedQ->pop_front();
    }
  }
  else {
    // Not in the past.
    list<Event*> *senderQ = senderQContainer->senderQ;
    bool headFlag = false;
    Event *prevHead = 0;
    if( !senderQ->empty() ){
      prevHead = senderQ->front();
      if( prevHead->getReceiveTime() <= event->getReceiveTime() ){
	senderQ->push_front(event);
      }
      else {
	// The event is not a straggler.  The event to be inserted can be
	// inserted in the senderQ or in the sortedUnProcessedQ.  Check to
	// see if it has to be inserted in the SortedUnProcessedQ.
	if( senderQ->back()->getReceiveTime() <= receiveTime || 
	    sortedUnProcessedQ->empty() == true ){
	  prevHead = senderQ->back();	

	  list<Event*>::iterator insertionPoint = senderQ->begin();
	  while( insertionPoint!= senderQ->end() && 
		 (*insertionPoint)->getReceiveTime() > receiveTime ){
	    insertionPoint++;
	  }
	  senderQ->insert( insertionPoint, event);
	  headFlag = true;
	}
	else {
	  list<Event*>::iterator beginListIter = sortedUnProcessedQ->begin();
	  list<Event*>::iterator endListIter = sortedUnProcessedQ->end();
	  while((beginListIter != endListIter) &&
		((*beginListIter)->getReceiveTime() > receiveTime)) {
	    beginListIter++;
	  }
	  sortedUnProcessedQ->insert(beginListIter,event);
	}
      }
    }
    else {
      if(!sortedUnProcessedQ->empty() &&
	 (sortedUnProcessedQ->front()->getReceiveTime() > receiveTime)) {
	//search and insert into sortedUnprocessedQ
	list<Event*>::iterator beginListIter = sortedUnProcessedQ->begin();
	list<Event*>::iterator endListIter = sortedUnProcessedQ->end();
	while((beginListIter != endListIter) &&
	      ((*beginListIter)->getReceiveTime() > receiveTime)) {
	  beginListIter++;
	}
	sortedUnProcessedQ->insert(beginListIter,event);
      }
      else {
	senderQ->push_front(event);
	makeHeapFlag = true;
	senderQContainer->headEventPtr = event;
      }
    }
    if(headFlag == true) {
      Event* currentHead = senderQ->back();
      if(currentHead != prevHead) {
	//Add the currently inserted element into the
	//scheduleList 
	senderQContainer->headEventPtr = currentHead;
	makeHeapFlag = true;
      }
    }
  }
  peekedEvent = 0;
  return inThePast;
}

void
TimeWarpAppendQueue::handleAntiMessage( Event *event,
					SimulationObject *object ){
  ASSERT(event != NULL);
  ASSERT(object != NULL);
  
  const OBJECT_ID senderID = mySimulationManager->getObjectId( event->getSender() );
  const OBJECT_ID receiverID = mySimulationManager->getObjectId( event->getReceiver() );
  VTime receiveTime = event->getReceiveTime();
  VTime sendTime = event->getSendTime();
  SenderQueueContainer* senderQContainer = senderQMap->find( event->getSender() );
  list<Event*>* senderQ = senderQContainer->senderQ;

  if( isInThePast(event) ) {

    list<Event*>::iterator beginListIter = sortedUnProcessedQ->begin();
    list<Event*>::iterator endListIter = sortedUnProcessedQ->end();
    while(beginListIter != endListIter) {
      if( (*beginListIter)->getSender() == event->getSender() && 
	  (*beginListIter)->getSendTime() >= sendTime ){
	list<Event*>::iterator tmpIter = beginListIter++;
	object->reclaimEvent(*tmpIter);
	*tmpIter = 0;
	sortedUnProcessedQ->erase(tmpIter);
      }
      else {
	beginListIter++;
      }
    } 

    //push_back all events in processedQ with receiveTime > 
    // new Events receiveTime into senderQ
    while((!processedQ->empty()) && 
	  (processedQ->front()->getReceiveTime() >= receiveTime)) {
      Event* tmpEvent = processedQ->front();
      if( tmpEvent->getSender() != event->getSender() ){
	sortedUnProcessedQ->push_back(tmpEvent);
      }
      else if(tmpEvent->getSendTime() < sendTime) {//senderid ==event's sender
	sortedUnProcessedQ->push_back(tmpEvent);
      }
      else {
	object->reclaimEvent(tmpEvent);
      }
      processedQ->pop_front();
    }
    
    //Now Destroy all events in the senderQ of the
    //anti-message's sender
    //rest of the senderQ's events stays
    list<Event*>::iterator beginIter = senderQ->begin();
    list<Event*>::iterator endIter = senderQ->end();
    while(beginIter != endIter) {
      if((*beginIter)->getSendTime() >= sendTime) {
	list<Event*>::iterator tmpIter = beginIter++;
	object->reclaimEvent(*tmpIter);
	*tmpIter = 0;
	senderQ->erase(tmpIter);
      }
      else {
	beginIter++;
      }
    }
    
    VTime simulationTime = object->getSimulationTime();
    
    //The State saving may have period != 1
    //So add all the events greater than the 
    //simulation time into senderQ
    while((!processedQ->empty()) && 
	  (processedQ->front()->getReceiveTime() > simulationTime)) {
      Event* tmpEvent = processedQ->front();
      sortedUnProcessedQ->push_back(tmpEvent);
      processedQ->pop_front();
    }
  }
  else {
    list<Event*>::iterator beginListIter = sortedUnProcessedQ->begin();
    list<Event*>::iterator endListIter = sortedUnProcessedQ->end();
    while(beginListIter != endListIter) {
      if((*beginListIter)->getReceiveTime() >= receiveTime) {
	if(((*beginListIter)->getSender() == event->getSender() ) &&
	   ((*beginListIter)->getSendTime() >= sendTime)) {
	  list<Event*>::iterator tmpIter = beginListIter++;
	  object->reclaimEvent(*tmpIter);
	  *tmpIter = 0;
	  sortedUnProcessedQ->erase(tmpIter);
	}
	else {
	  beginListIter++;
	}
      }
      else {
	break;
      }
    }
    
    //    Insert the antimessage into the sender queue
    if((!senderQ->empty()) && 
       (senderQ->front()->getReceiveTime() < receiveTime)) {
      //Ignore the Fast Anti Message
      cerr << "Warning!!! Received FAST ANTIMESSAGE" << endl;
      cerr << "Ignoring FAST ANTIMESSAGE and Proceeding with the Simulation... " << endl;
    }
    else {
      list<Event*>::iterator beginListIter = senderQ->begin();
      list<Event*>::iterator endListIter = senderQ->end();
      //search and remove the messages
      while(beginListIter != endListIter) {
	list<Event*>::iterator tmpIter = beginListIter++;
	Event* tmpEvent = (*tmpIter);
	if(tmpEvent->getReceiveTime() >= receiveTime) {
	  if(tmpEvent->getSendTime() >= sendTime) {
	    object->reclaimEvent(tmpEvent);
	    *tmpIter = 0;
	    senderQ->erase(tmpIter);
	  }
	}
	else {
	  break;
	}
      }
    }
  }

  if(!senderQ->empty()) {
    //Add the currently inserted element into the
    //scheduleList 
    Event* currentHead = senderQ->back();
    senderQContainer->headEventPtr = currentHead;
  }
  else {
    senderQContainer->headEventPtr = DummyEvent::instance();
  }
  makeHeapFlag = true;
  peekedEvent = 0;
}

void
TimeWarpAppendQueue::remove( Event *eventToRemove, 
			     findMode mode, 
			     SimulationObject* simObj ) {
  abort();
}

Event*
TimeWarpAppendQueue::getEvent( SimulationObject *object ){

  ASSERT(object != NULL);
  Event* eventToProcess = 0;
  eventToProcess = peekEvent(object);
  if(eventToProcess != 0) {
    if(!sortedUnProcessedQ->empty()) {
      //remove it from sortedUnProcessed Q
      sortedUnProcessedQ->pop_back();
    }
    else {
      SenderQueueContainer *senderQContainer = senderQMap->find( eventToProcess->getSender() );
      list<Event*>* senderQ = senderQContainer->senderQ;

      senderQ->pop_back();
      popHeapFlag = true;
      if(senderQ->empty() == false) {
	//Add the currently inserted element into the
	//scheduleList 
	Event* currentHead = senderQ->back();
	senderQContainer->headEventPtr = currentHead;
      }
      else {
	senderQContainer->headEventPtr = DummyEvent::instance();
      }
      push_heap(scheduleList->begin(), scheduleList->end(),
		GreaterThan_ScheduleListContainer()); 
    }
    //Add the event to the processed Q
    processedQ->push_front(eventToProcess);
  }
  peekedEvent = 0;
  //return the event
  return eventToProcess;
}

Event*
TimeWarpAppendQueue::peekEvent(SimulationObject* object) {
  //  ASSERT(object != NULL);
  Event* eventToProcess = 0;
  
  if(peekedEvent != 0) {
    return peekedEvent;
  }

  if(!sortedUnProcessedQ->empty()) {
    eventToProcess = sortedUnProcessedQ->back();
    return (peekedEvent=eventToProcess);
  }
  else { //search in the schedule list
    if(scheduleList->empty()) {
      return (peekedEvent=0);
    }

    if(makeHeapFlag) {

    	make_heap(scheduleList->begin(), scheduleList->end(),
		      GreaterThan_ScheduleListContainer());
    }
    if(popHeapFlag || makeHeapFlag) {

      pop_heap(scheduleList->begin(), scheduleList->end(),
	       GreaterThan_ScheduleListContainer());
      popHeapFlag = false;
    }
    makeHeapFlag = false;
    
    ScheduleListContainer* scheduleListContainer = scheduleList->back();
    
    if(*(scheduleListContainer->headEventPtr) == DummyEvent::instance()) {
      eventToProcess = 0;
    }
    else {
      eventToProcess = *(scheduleListContainer->headEventPtr);
    }
  }
  
  //return the event
  return (peekedEvent=eventToProcess);
}

Event*
TimeWarpAppendQueue::find(const VTIME&, findMode, SimulationObject* ) {

  cerr << "find() : Currently not supported for Sender Queues" << endl;
  return 0;
}

void
TimeWarpAppendQueue::fossilCollect(const VTIME& fossilCollectTime,
                                    SimulationObject* simObj) {
  Event* event = 0;
  while(!processedQ->empty()) {
    event = processedQ->back();
    if (event->getReceiveTime() < fossilCollectTime) {
      // DEM - erased this as OutputManagerImplementationBase::fossilCollect seems
      // to already be deleting the events...
//       simObj->reclaimEvent(event);
      processedQ->pop_back();
    }
    else {
      break;
    }
  }
}


bool
TimeWarpAppendQueue::isInThePast( const Event *event ){

  VTime receiveTime = event->getReceiveTime();

  if(processedQ->empty()) {
    return false;
  }
  else {
    if(processedQ->front()->getReceiveTime() >= receiveTime) {
      return true;
    }
    else {
      return false;
    }
  }
}
