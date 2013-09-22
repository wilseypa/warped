
#include "RestoreCkptMessage.h"
#include "DeserializerManager.h"

const string &
RestoreCkptMessage::getRestoreCkptMessageType(){
  static string restoreCkptMessageType = "RestoreCkptMessage";
  return restoreCkptMessageType;
}

void 
RestoreCkptMessage::serialize( SerializedInstance *serialized ) const {
  KernelMessage::serialize( serialized );
  serialized->addInt( checkpointTime );
  serialized->addInt( tokenState );
  serialized->addInt( checkpointConsensus );
}

Serializable *
RestoreCkptMessage::deserialize( SerializedInstance *data ){
  unsigned int sender = data->getUnsigned();
  unsigned int dest = data->getUnsigned();
  unsigned int incNum = data->getUnsigned();
  int ckptTime = data->getInt();
  int tokSt = data->getInt();
  bool ckptCon = data->getInt();

  RestoreCkptMessage *retval = new RestoreCkptMessage(sender, 
                                                      dest, 
                                                      ckptTime, 
                                                      RestoreCkptMessage::RestoreState(tokSt),
                                                      ckptCon);
  retval->setIncarnationNumber(incNum);
  return retval;
}

void 
RestoreCkptMessage::registerDeserializer(){
  DeserializerManager::instance()->registerDeserializer( getRestoreCkptMessageType(),
							 &deserialize );
}
