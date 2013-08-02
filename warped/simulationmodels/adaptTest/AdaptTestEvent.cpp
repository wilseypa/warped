// See copyright notice in file Copyright in the root directory of this archive.

#include "AdaptTestEvent.h"
#include <cstdlib>
#include <warped/SerializedInstance.h>

void 
AdaptTestEvent::serialize( SerializedInstance *addTo ) const {
  Event::serialize( addTo );
}

Serializable *
AdaptTestEvent::deserialize( SerializedInstance *instance ){
  VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
  VTime *receiveTime = dynamic_cast<VTime *>(instance->getSerializable());
  unsigned int senderSimManID = instance->getUnsigned();
  unsigned int senderSimObjID = instance->getUnsigned();
  unsigned int receiverSimManID = instance->getUnsigned();
  unsigned int receiverSimObjID = instance->getUnsigned();
  unsigned int eventId = instance->getUnsigned();

  ObjectID sender(senderSimObjID, senderSimManID);
  ObjectID receiver(receiverSimObjID, receiverSimManID);

  AdaptTestEvent *event = new AdaptTestEvent( *sendTime,
				              *receiveTime,
				              sender,
				              receiver,
				              eventId );
  delete sendTime;
  delete receiveTime;

  return event;
}

bool
AdaptTestEvent::eventCompare( const Event* event ){
  return compareEvents( this, event );
}
