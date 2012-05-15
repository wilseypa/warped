#ifndef CF_ROLLBACK_VECTOR_MESSAGE_H
#define CF_ROLLBACK_VECTOR_MESSAGE_H
// See copyright notice in file Copyright in the root directory of this archive.

#include "KernelMessage.h"

/**
   Allows a vector of integers to be passed among nodes
*/

class UsefulWorkMessage : public KernelMessage {
public:
  enum MessageRound {
    COLLECT,
    SETFREQ,
    CIRCULATECPU
  };

  UsefulWorkMessage(unsigned int source,
                unsigned int dest, int nSimMgrs, MessageRound r) :
    KernelMessage(source, dest),
    numSimulationManagers(nSimMgrs),
    myData(nSimMgrs),
    myRound(r)
  {}
  
  void serialize( SerializedInstance * ) const;
  static Serializable *deserialize( SerializedInstance *data );

  const string &getDataType() const { return dataType(); }
  void getData(std::vector<double>& data) const { data = myData; }
  void setData(std::vector<double>& data) { myData = data; }
  MessageRound getRound() const { return myRound; }
  void setRound(MessageRound mr) { myRound = mr; }

  static const string& dataType();

  static void registerDeserializer();

private:
  /**
     Default constructor - to be used only by the deserializer.
  */
  UsefulWorkMessage() :
    numSimulationManagers(0),
    myData(0),
    myRound(COLLECT)
  {}

  const unsigned int numSimulationManagers;
  std::vector<double> myData;
  MessageRound myRound;
};

#endif //CF_ROLLBACK_VECTOR_MESSAGE_H
