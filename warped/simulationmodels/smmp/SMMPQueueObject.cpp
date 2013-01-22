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

#include "SMMPQueueObject.h"                                               
#include "SMMPQueueState.h"
#include "IntVTime.h"
#include "MemEvents.h"

SMMPQueueObject::SMMPQueueObject(string initName, string initServerName):
   myObjectName(initName),
   serverName(initServerName){
}

SMMPQueueObject::~SMMPQueueObject(){
}

void
SMMPQueueObject::initialize(){
}

void
SMMPQueueObject::finalize(){
}

void
SMMPQueueObject::executeProcess(){
   MemRequest *recvEvent = NULL;
   IntVTime sendTime = static_cast<const IntVTime&> (getSimulationTime());

   while(haveMoreEvents() == true){
      recvEvent = (MemRequest*)getEvent();

      if ( recvEvent != NULL ) {
         SimulationObject *receiver = getObjectHandle(serverName);

         MemRequest *newEvent = new MemRequest(sendTime, sendTime,
                                               this, receiver);
         newEvent->setStartTime(recvEvent->getStartTime());
         newEvent->setProcessor(recvEvent->getProcessor());

         receiver->receiveEvent(newEvent);
      }
   }
}

State*
SMMPQueueObject::allocateState() {
  return new SMMPQueueState();
}

void
SMMPQueueObject::deallocateState( const State *state ){
   delete state;
}

void
SMMPQueueObject::reclaimEvent(const Event *event){
   delete event;
}
