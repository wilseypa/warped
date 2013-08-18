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
