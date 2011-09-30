// See copyright notice in file Copyright in the root directory of this archive.

#include "GVTUpdateMessage.h"
#include "DeserializerManager.h"

void 
GVTUpdateMessage::serialize( SerializedInstance *addTo ) const {
  KernelMessage::serialize( addTo );
  addTo->addSerializable( newGVT );
}

Serializable *
GVTUpdateMessage::deserialize( SerializedInstance *data ){
  unsigned int source = data->getUnsigned();
  unsigned int dest = data->getUnsigned();
  unsigned int incNum = data->getUnsigned();
  VTime *newGVT = dynamic_cast<VTime *>(data->getSerializable());

  GVTUpdateMessage *retval = new GVTUpdateMessage( source, dest, *newGVT );
  retval->setIncarnationNumber(incNum);
  delete newGVT;

  return retval;
}

const string &
GVTUpdateMessage::getGVTUpdateMessageType(){
  static const string gVTUpdateMessageType = "GVTUpdateMessageType";
  return gVTUpdateMessageType;
}


void GVTUpdateMessage::registerDeserializer(){
  DeserializerManager::instance()->registerDeserializer( getGVTUpdateMessageType(),
							 &deserialize );

}
