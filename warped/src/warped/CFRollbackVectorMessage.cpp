// See copyright notice in file Copyright in the root directory of this archive.

#include "CFRollbackVectorMessage.h"
#include "DeserializerManager.h"

void 
CFRollbackVectorMessage::serialize( SerializedInstance *serialized ) const {
  KernelMessage::serialize( serialized );
  serialized->addUnsigned(numSimulationManagers);
  for (std::vector<int>::const_iterator it(myData.begin()); it != myData.end(); ++it) {
    serialized->addInt(*it);
  }
}

Serializable *
CFRollbackVectorMessage::deserialize( SerializedInstance *data ){
  unsigned int sender = data->getUnsigned();
  unsigned int dest = data->getUnsigned();
  unsigned int incNum = data->getUnsigned();

  unsigned int nSimMgrs = data->getUnsigned();
  std::vector<int> d(nSimMgrs);
  for (int i = 0; i < nSimMgrs; ++i) {
    d[i] = data->getInt();
  }

  CFRollbackVectorMessage *retMsg = new CFRollbackVectorMessage(sender, dest, nSimMgrs);
  retMsg->setData(d);
  retMsg->setIncarnationNumber(incNum);

  return retMsg;
}

void 
CFRollbackVectorMessage::registerDeserializer(){
  DeserializerManager::instance()->registerDeserializer( dataType(),
							 &deserialize );
}

const string&
CFRollbackVectorMessage::dataType() {
  static const string dt = "CFRollbackVectorMessage";
  return dt;
}
