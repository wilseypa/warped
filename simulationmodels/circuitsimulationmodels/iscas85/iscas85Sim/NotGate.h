#ifndef NOT_GATE_H
#define NOT_GATE_H

#include "NInputGate.h"

/** The NInputOrGate class.

    This class represents a not gate.
*/
class NotGate : public NInputGate {
public:
   
   /**@name Public Class Methods of NInputOrGate. */
   //@{

   /// Default constructor
   NotGate(string &objectName, const int numInputs,/*const int numInValue,*/const int numOutputs,
                 vector<string> *outputs, vector<int> *destInputPorts,
                 int objectDelay);
   /// Default Destructor
   ~NotGate(){}

   /// compute the outputs of this gate
   int computeOutput(const int* inputBits);
  
   //@} // End of Public Class Methods of NInputOrGate.
};

#endif
