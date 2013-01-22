// Copyright (c) The University of Cincinnati.  
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF 
// THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.
// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

#include "MemRouterObject.h"
#include "MemEvents.h"
#include "MemRouterState.h"

MemRouterObject::MemRouterObject(string initName):
  objectName(initName) {
}

MemRouterObject::~MemRouterObject() {
   deallocateState(getState());
}

void
MemRouterObject::initialize() {
}

void
MemRouterObject::finalize() {
}

void
MemRouterObject::executeProcess(){
   MemRequest *toForward = NULL;
   IntVTime sendTime = static_cast<const IntVTime&> (getSimulationTime());

   while(true == haveMoreEvents()) {
      toForward = (MemRequest*)getEvent();
      if( toForward != NULL ){
         string dest = toForward->getProcessor();
         SimulationObject *receiver = getObjectHandle(dest);

         MemRequest *newEvent = new MemRequest(sendTime, sendTime,
                                               this, receiver);
         newEvent->setStartTime(toForward->getStartTime());
         newEvent->setProcessor(toForward->getProcessor());

         receiver->receiveEvent(newEvent);
      }
   }
}

State*
MemRouterObject::allocateState() {
  return new MemRouterState();
}

void
MemRouterObject::deallocateState( const State *state ){
   delete state;
}

void
MemRouterObject::reclaimEvent(const Event *event){
   delete event;
}

const string &
MemRouterObject::getName()const{
  return objectName;
}
