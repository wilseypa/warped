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
