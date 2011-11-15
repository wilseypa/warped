#ifndef CF_ROLLBACK_AVERAGE_MESSAGE_H
#define CF_ROLLBACK_AVERAGE_MESSAGE_H
// See copyright notice in file Copyright in the root directory of this archive.

#include "KernelMessage.h"

/**
   Allows a vector of integers to be passed among nodes
*/

class CFRollbackAverageMessage : public KernelMessage {
public:
  CFRollbackAverageMessage(unsigned int source,
			    unsigned int dest,
			    int round,
			    double avg,
			    unsigned int min,
			    unsigned int max) :
    KernelMessage(source, dest),
    myRound(round),
    myRollbackAverage(avg),
    myRollbackMin(min),
    myRollbackMax(max)
  {}
  
  void serialize( SerializedInstance * ) const;
  static Serializable *deserialize( SerializedInstance *data );

  const string &getDataType() const { return dataType(); }
  const int getRound() const { return myRound; }
  double getRollbackAverage() const { return myRollbackAverage; }
  unsigned int getRollbackMin() const { return myRollbackMin; }
  unsigned int getRollbackMax() const { return myRollbackMax; }

  void update(unsigned int rollbacks, int numLPs);

  static const string& dataType();

  static void registerDeserializer();

private:
  /**
     Default constructor - to be used only by the deserializer.
  */
  CFRollbackAverageMessage() :
    myRound(0),
    myRollbackAverage(0.f)
  {}

  const int myRound;
  double myRollbackAverage;
  unsigned int myRollbackMax;
  unsigned int myRollbackMin;
};

#endif //CF_ROLLBACK_AVERAGE_MESSAGE_H
