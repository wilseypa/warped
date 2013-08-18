#ifndef N_INPUT_XOR_GATE_H
#define N_INPUT_XOR_GATE_H

#include "NInputGate.h"

/** The NInputXorGate class.

    This class represents an N input and gate.
*/
class NInputXorGate : public NInputGate {
public:
   
   /**@name Public Class Methods of NInputXorGate. */
   //@{

   /// Default constructor
   NInputXorGate(string &objectName, const int numInputs,/*const int numInValue,*/const int numOutputs,
                 vector<string> *outputs, vector<int> *destInputPorts,int objectDelay);

   /// Default Destructor
   ~NInputXorGate(){}

   /// compute the outputs of this gate
   int computeOutput(const int* inputBits);
  
   //@} // End of Public Class Methods of NInputXorGate.
};

#endif
