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
 
#include "SMMPForkObject.h"
#include "SMMPForkState.h"
#include "IntVTime.h"
#include "MemEvents.h"

const double first = 0.0;
const double second = 1.0;

SMMPForkObject::SMMPForkObject(string myName, double initSeed, double cHitRat,
                               vector<string> fanNames) :
  myObjectName(myName),
  seed(initSeed),
  numFanOut(0),
  cacheHitRatio(cHitRat),
  fanOutNames(fanNames){
}

SMMPForkObject::~SMMPForkObject(){
   deallocateState(getState());
}

void
SMMPForkObject::initialize() {
   SMMPForkState *myState = dynamic_cast<SMMPForkState*>(getState());
   myState->gen = new MLCG(seed , seed + 1);
   
   fanOutHandles.resize(fanOutNames.size(),NULL);
   for(int i = 0; i < fanOutHandles.size(); i++){
     fanOutHandles[i] = getObjectHandle(fanOutNames[i]);
   }
}

void
SMMPForkObject::finalize(){
}

void
SMMPForkObject::executeProcess() {
   SMMPForkState *myState = static_cast<SMMPForkState*>(getState());
   IntVTime sendTime = static_cast<const IntVTime&> (getSimulationTime());
   MemRequest *recvEvent = NULL;
   string recvName = "";
   SimulationObject *receiver = NULL;
   //int index = 0;

   while(haveMoreEvents() == true){ 
      recvEvent = (MemRequest*) getEvent();
      
      if ( recvEvent != NULL ) {

         double randNum;
         MemRequest *newEvent = NULL;

         Uniform uniformSpc(first, second, myState->gen);
         randNum = uniformSpc();
         if( randNum < cacheHitRatio){
            receiver = fanOutHandles[0];
            myState->lastOutput = 0;
         }
         else{
            receiver = fanOutHandles[1];
            myState->lastOutput = 1;
         }

         newEvent = new MemRequest(sendTime, sendTime + myState->localDelay,
                                   this, receiver);

         newEvent->setStartTime(recvEvent->getStartTime());
         newEvent->setProcessor(recvEvent->getProcessor());               

         receiver->receiveEvent(newEvent);
      }
   }
}

State*
SMMPForkObject::allocateState() {
  return new SMMPForkState();
}

void
SMMPForkObject::deallocateState( const State *state ){
   delete state;
}

void
SMMPForkObject::reclaimEvent(const Event *event){
   delete event;
}
