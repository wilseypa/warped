#ifndef N_INPUT_NOR_GATE_H
#define N_INPUT_NOR_GATE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "NInputGate.h"

/** The NInputAndGate class.

    This class represents an N input nor gate.
*/
class NInputNorGate : public NInputGate {
public:
   
   /**@name Public Class Methods of NInputAndGate. */
   //@{

   /// Default constructor
   NInputNorGate(string &objectName, const int numInputs,/*const int numInValue,*/const int numOutputs,
                 vector<string> *outputs, vector<int> *destInputPorts,
                 int objectDelay);
   /// Default Destructor
   ~NInputNorGate(){}

   /// compute the outputs of this gate
   int computeOutput(const int* inputBits);
  
   //@} // End of Public Class Methods of NInputAndGate.
};

#endif
