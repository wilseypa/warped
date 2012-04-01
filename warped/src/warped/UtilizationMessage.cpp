// See copyright notice in file Copyright in the root directory of this archive.

#include "UtilizationMessage.h"
#include "DeserializerManager.h"

void 
UtilizationMessage::serialize( SerializedInstance *serialized ) const {
  KernelMessage::serialize( serialized );
  serialized->addUnsigned(numSimulationManagers);
  std::vector<double>::const_iterator it = myData.begin();
  for (; it != myData.end(); ++it) {
    serialized->addDouble(*it);
  }
  serialized->addInt(myRound);
}

Serializable *
UtilizationMessage::deserialize( SerializedInstance *data ){
  unsigned int sender = data->getUnsigned();
  unsigned int dest = data->getUnsigned();
  unsigned int incNum = data->getUnsigned();

  unsigned int nSimMgrs = data->getUnsigned();
  std::vector<double> d(nSimMgrs);
  for (int i = 0; i < nSimMgrs; ++i) {
    d[i] = data->getDouble();
  }
  MessageRound round = static_cast<MessageRound>(data->getInt());

  UtilizationMessage *retMsg =
          new UtilizationMessage(sender, dest, nSimMgrs, round);
  retMsg->setData(d);
  retMsg->setIncarnationNumber(incNum);

  return retMsg;
}

void 
UtilizationMessage::registerDeserializer(){
  DeserializerManager::instance()->registerDeserializer( dataType(),
							 &deserialize );
}

const string&
UtilizationMessage::dataType() {
  static const string dt = "UtilizationMessage";
  return dt;
}
