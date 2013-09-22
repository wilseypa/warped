#include "PingEvent.h"
#include <cstdlib>
#include <warped/SerializedInstance.h>
#include <warped/ObjectID.h>

void 
PingEvent::serialize( SerializedInstance *addTo ) const {
  Event::serialize( addTo );
  addTo->addString( myOwner );
}

Serializable *
PingEvent::deserialize( SerializedInstance *instance ){
  VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
  VTime *receiveTime = dynamic_cast<VTime *>(instance->getSerializable());
  unsigned int senderSimManID = instance->getUnsigned();
  unsigned int senderSimObjID = instance->getUnsigned();
  unsigned int receiverSimManID = instance->getUnsigned();
  unsigned int receiverSimObjID = instance->getUnsigned();
  unsigned int eventId = instance->getUnsigned();
  string owner = instance->getString();

  ObjectID sender(senderSimObjID, senderSimManID);
  ObjectID receiver(receiverSimObjID, receiverSimManID);

  PingEvent *event = new PingEvent( *sendTime,
				    *receiveTime,
				    sender,
				    receiver,
				    eventId,
				    owner );
  delete sendTime;
  delete receiveTime;

  return event;
}

bool
PingEvent::eventCompare( const Event* event ){
  if( compareEvents( this, event ) == true ){
    if( myOwner == dynamic_cast<const PingEvent *>(event)->getOwner() ){
      return true;
    }
  }
  return false;
}
