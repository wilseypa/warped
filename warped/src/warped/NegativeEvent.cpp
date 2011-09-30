// See copyright notice in file Copyright in the root directory of this archive.

#include "Serializable.h"
#include "SerializedInstance.h"
#include "ObjectID.h"
#include "EventId.h"
#include "DeserializerManager.h"
#include "NegativeEvent.h"
//#include "OptFossilCollManager.h"

void
NegativeEvent::serialize( SerializedInstance *addTo ) const {
  addTo->addSerializable( sendTime );
  addTo->addSerializable( receiveTime );
  addTo->addUnsigned( sender->getSimulationManagerID() );
  addTo->addUnsigned( sender->getSimulationObjectID() );
  addTo->addUnsigned( receiver->getSimulationManagerID() );
  addTo->addUnsigned( receiver->getSimulationObjectID() );
  addTo->addUnsigned( id->val );
}

Serializable *
NegativeEvent::deserialize( SerializedInstance *instance ){
  VTime *sendingTime = dynamic_cast<VTime *>(instance->getSerializable());
  VTime *recvTime = dynamic_cast<VTime *>(instance->getSerializable());
  unsigned int senderSimManID = instance->getUnsigned();
  unsigned int senderSimObjID = instance->getUnsigned();
  unsigned int receiverSimManID = instance->getUnsigned();
  unsigned int receiverSimObjID = instance->getUnsigned();
  unsigned int eventId = instance->getUnsigned();

  ObjectID sendObj(senderSimObjID, senderSimManID);
  ObjectID recvObj(receiverSimObjID, receiverSimManID);

  NegativeEvent *negEvent = new NegativeEvent( *sendingTime,
                                               *recvTime,
                                               sendObj,
                                               recvObj,
                                               eventId);
  delete sendingTime;
  delete recvTime;

  return negEvent;
}

void 
NegativeEvent::registerDeserializer(){
  DeserializerManager::instance()->registerDeserializer( getNegativeEventDataType(),
                                                         &NegativeEvent::deserialize );
}

ostream&
operator<<(ostream& os, const NegativeEvent& event) {
   os << "sender: " << event.getSender() 
      << " receiver: " << event.getReceiver()
      << " sendTime: " << event.getSendTime()
      << " receiveTime: " << event.getReceiveTime()
      << " eventId: " << event.getEventId();
   return(os);
}

bool 
NegativeEvent::eventCompare( const Event *a ){
  bool retval = true;

  if( a->getSender() != this->getSender() ||
      a->getEventId() != this->getEventId() ){
    retval = false;
  }

  return retval;
}

// No longer using these for optimistic fossil collection.
/*
void *
NegativeEvent::operator new(size_t size){
  if(!usingOptFossilCollMan){
    return ::operator new(size);
  }
  else{
    return myOptFosColMan->newNegativeEvent(size);
  }
}

void
NegativeEvent::operator delete(void *toDelete){
  if(!usingOptFossilCollMan){
    ::operator delete(toDelete);
  }
  else{
    myOptFosColMan->deleteNegativeEvent(toDelete);
  }
}*/
