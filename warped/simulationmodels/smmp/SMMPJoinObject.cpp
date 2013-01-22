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

#include "SMMPJoinObject.h"
#include "SMMPJoinState.h"
#include "IntVTime.h"
#include "MemEvents.h"

SMMPJoinObject::SMMPJoinObject(string myName, string initDest):
  myObjectName(myName),
  dest(initDest),
  joinDelay(0){
}

SMMPJoinObject::~SMMPJoinObject(){
   deallocateState(getState());
}

void
SMMPJoinObject::initialize(){
}

void
SMMPJoinObject::finalize(){
}

void
SMMPJoinObject::executeProcess(){
  MemRequest *recvEvent = NULL;
  IntVTime sendTime = static_cast<const IntVTime&> (getSimulationTime());

  while(haveMoreEvents() == true){ 
     recvEvent = (MemRequest*)getEvent();
     if ( recvEvent != NULL ) {
        // send event to server or join or fork or queue as specified by user
        SimulationObject *receiver = getObjectHandle(dest);

        MemRequest *newEvent = new MemRequest(sendTime, sendTime + joinDelay,
                                              this, receiver);
        newEvent->setStartTime(recvEvent->getStartTime());
        newEvent->setProcessor(recvEvent->getProcessor());

        receiver->receiveEvent(newEvent);
     }
  }
}

void
SMMPJoinObject::setJoinDestination(string destination, int delay){
  dest = destination;
  joinDelay = delay ;
}

State*
SMMPJoinObject::allocateState() {
  return new SMMPJoinState();
}

void
SMMPJoinObject::deallocateState(const State *state) {
  delete state;
}

void
SMMPJoinObject::reclaimEvent(const Event *event){
  delete event;
}
