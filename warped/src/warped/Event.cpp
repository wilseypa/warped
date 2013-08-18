
#include "Event.h"
#include "EventId.h"
#include "SerializedInstance.h"
#include "OptFossilCollManager.h"

// No longer using these for optimistic fossil collection.
//bool Event::usingOptFossilCollMan = false;
//OptFossilCollManager *Event::myOptFosColMan = NULL;

Event::~Event(){}

ostream&
operator<<(ostream& os, const Event& event) {
   os << "sender: " << event.getSender() 
      << " receiver: " << event.getReceiver()
      << " sendTime: " << event.getSendTime()
      << " receiveTime: " << event.getReceiveTime()
      << " eventId: " << event.getEventId();
   return(os);
}

bool
operator==(const Event &eve1, const Event &eve2){
  // NOTE: This is used to uniquely identify events for use
  //       in STL containers. Do not use this for comparing
  //       events in lazy cancellation or similar areas. Use
  //       compareEvents instead.
  bool retval = eve1.getSender() == eve2.getSender() &&
                eve1.getEventId() == eve2.getEventId();
  return retval;
}

bool 
Event::compareEvents( const Event *a, const Event *b ){
  bool retval = true;

  if( a->getSender() != b->getSender() ||
      a->getSendTime() != b->getSendTime() ||
      a->getReceiver() != a->getReceiver() ||
      a->getReceiveTime() != b->getReceiveTime() ){
    retval = false;
  }

  return retval;
}

void 
Event::serialize( SerializedInstance *addTo ) const {
  addTo->addSerializable( &getSendTime() );
  addTo->addSerializable( &getReceiveTime() );
  addTo->addUnsigned( getSender().getSimulationManagerID() );
  addTo->addUnsigned( getSender().getSimulationObjectID() );
  addTo->addUnsigned( getReceiver().getSimulationManagerID() );
  addTo->addUnsigned( getReceiver().getSimulationObjectID() );
  addTo->addUnsigned( getEventId().val );
}

void *
Event::operator new(size_t size){
  // No longer overloading the new operator for optimistic fossil collection.
  /*if(!usingOptFossilCollMan){
    return ::operator new(size);
  }
  else{
    return myOptFosColMan->newEvent(size);
  }*/
  ::operator new(size);
}

void
Event::operator delete(void *toDelete){
  // No longer overloading the delete operator for optimistic fossil collection.
  /*if(!usingOptFossilCollMan){
    ::operator delete(toDelete);
  }
  else{
    myOptFosColMan->deleteEvent(toDelete);
  }*/
  ::operator delete(toDelete);
}
