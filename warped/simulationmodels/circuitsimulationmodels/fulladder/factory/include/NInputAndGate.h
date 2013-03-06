#ifndef N_INPUT_AND_GATE_H
#define N_INPUT_AND_GATE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "NInputGate.h"

/** The NInputAndGate class.

    This class represents an N input and gate.
*/
class NInputAndGate : public NInputGate {
public:
   
   /**@name Public Class Methods of NInputAndGate. */
   //@{

   /// Default constructor
   NInputAndGate(string &objectName, const int numInputs,/*const int numInValue,*/const int numOutputs,
                 vector<string> *outputs, vector<int> *destInputPorts,int objectDelay);

   /// Default Destructor
   ~NInputAndGate(){}

   /// compute the outputs of this gate
   int computeOutput(const int* inputBits);
  
   //@} // End of Public Class Methods of NInputAndGate.
};

#endif
