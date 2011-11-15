// See copyright notice in file Copyright in the root directory of this archive.

#include "CFRollbackAverageMessage.h"
#include "DeserializerManager.h"

void 
CFRollbackAverageMessage::serialize( SerializedInstance *serialized ) const {
  KernelMessage::serialize( serialized );
  serialized->addInt(myRound);
  serialized->addDouble(myRollbackAverage);
  serialized->addUnsigned(myRollbackMin);
  serialized->addUnsigned(myRollbackMax);
}

Serializable *
CFRollbackAverageMessage::deserialize( SerializedInstance *data ){
  unsigned int sender = data->getUnsigned();
  unsigned int dest = data->getUnsigned();
  unsigned int incNum = data->getUnsigned();

  int round = data->getInt();
  double avg = data->getDouble();
  unsigned int min = data->getUnsigned();
  unsigned int max = data->getUnsigned();

  CFRollbackAverageMessage *retMsg = new CFRollbackAverageMessage(sender, dest, round, avg, min, max);
  retMsg->setIncarnationNumber(incNum);

  return retMsg;
}

void 
CFRollbackAverageMessage::registerDeserializer(){
  DeserializerManager::instance()->registerDeserializer( dataType(),
							 &deserialize );
}

const string&
CFRollbackAverageMessage::dataType() {
  static const string dt = "CFRollbackAverageMessage";
  return dt;
}

void
CFRollbackAverageMessage::update(unsigned int rollbacks, int numLPs) {
  myRollbackMax = MAX_FUNC(myRollbackMax, rollbacks);
  myRollbackMin = MIN_FUNC(myRollbackMin, rollbacks);
  myRollbackAverage += (double)rollbacks / (double)numLPs;
}
