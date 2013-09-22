
#include "MatternGVTMessage.h"
#include "DeserializerManager.h"

const string &
MatternGVTMessage::getMatternGVTMessageType(){
  static string matternGVTMessageType = "MatternGVTMessage";
  return matternGVTMessageType;
}

void 
MatternGVTMessage::serialize( SerializedInstance *serialized ) const {
  KernelMessage::serialize( serialized );
  serialized->addSerializable( lastScheduledEventTime );
  serialized->addSerializable( minimumTimeStamp );
  serialized->addUnsigned( numMessagesInTransit );
}

Serializable *
MatternGVTMessage::deserialize( SerializedInstance *data ){
  unsigned int sender = data->getUnsigned();
  unsigned int dest = data->getUnsigned();
  unsigned int incNum = data->getUnsigned();
  VTime *lastScheduled = dynamic_cast<VTime *>( data->getSerializable() );
  VTime *minimum = dynamic_cast<VTime *>( data->getSerializable() );
  unsigned int numMessagesInTransit = data->getUnsigned();

  MatternGVTMessage *retMsg = new MatternGVTMessage( sender, 
                                                     dest, 
                                                     *lastScheduled, 
                                                     *minimum,
                                                     numMessagesInTransit );
  retMsg->setIncarnationNumber(incNum);

  delete lastScheduled;
  delete minimum;

  return retMsg;
}

void 
MatternGVTMessage::registerDeserializer(){
  DeserializerManager::instance()->registerDeserializer( getMatternGVTMessageType(),
							 &deserialize );
}
