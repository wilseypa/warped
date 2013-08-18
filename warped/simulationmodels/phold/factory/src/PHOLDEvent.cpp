#include "../include/PHOLDEvent.h"
#include <warped/SerializedInstance.h>

void
PHOLDEvent::serialize( SerializedInstance *addTo ) const{
  Event::serialize( addTo );
  addTo->addUnsigned(numberOfHops);
  addTo->addUnsigned(eventNumber);
}

Serializable*
PHOLDEvent::deserialize(SerializedInstance* instance){
  VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
  VTime *receiveTime = dynamic_cast<VTime *>(instance->getSerializable());
  unsigned int senderSimManID = instance->getUnsigned();
  unsigned int senderSimObjID = instance->getUnsigned();
  unsigned int receiverSimManID = instance->getUnsigned();
  unsigned int receiverSimObjID = instance->getUnsigned();  
  unsigned int eventId = instance->getUnsigned();
  unsigned int numHops = instance->getUnsigned();
  unsigned int evtNum = instance->getUnsigned();

  ObjectID sender(senderSimObjID, senderSimManID);
  ObjectID receiver(receiverSimObjID, receiverSimManID);

  PHOLDEvent *event = new PHOLDEvent( *sendTime,
                                      *receiveTime,
                                      sender,
                                      receiver,
                                      eventId,
                                      numHops,
                                      evtNum );
  delete sendTime;
  delete receiveTime;

  return event;
}

bool
PHOLDEvent::eventCompare(const Event* event) {
  PHOLDEvent *pev = (PHOLDEvent*) event;

  return ( compareEvents( this, event ) &&
           numberOfHops == pev->numberOfHops &&
           eventNumber == pev->eventNumber );
}

std::ostream&
operator<< (std::ostream& os, const PHOLDEvent& pe) {
  os << (const Event&)pe;
  os << " Event Number " << pe.eventNumber
     << " Number of Hops " << pe.numberOfHops << "\n";
  return os;
}
