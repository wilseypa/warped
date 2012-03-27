#ifndef CF_ROLLBACK_VECTOR_MESSAGE_H
#define CF_ROLLBACK_VECTOR_MESSAGE_H
// See copyright notice in file Copyright in the root directory of this archive.

#include "KernelMessage.h"

/**
   Allows a vector of integers to be passed among nodes
*/

class UtilizationMessage : public KernelMessage {
public:
  UtilizationMessage(unsigned int source,
			    unsigned int dest, int nSimMgrs) :
    KernelMessage(source, dest),
    numSimulationManagers(nSimMgrs),
    myData(nSimMgrs)
  {}
  
  void serialize( SerializedInstance * ) const;
  static Serializable *deserialize( SerializedInstance *data );

  const string &getDataType() const { return dataType(); }
  void getData(std::vector<double>& data) const { data = myData; }
  void setData(std::vector<double>& data) { myData = data; }

  static const string& dataType();

  static void registerDeserializer();

private:
  /**
     Default constructor - to be used only by the deserializer.
  */
  UtilizationMessage() :
    numSimulationManagers(0),
    myData(0)
  {}

  const unsigned int numSimulationManagers;
  std::vector<double> myData;
};

#endif //CF_ROLLBACK_VECTOR_MESSAGE_H
