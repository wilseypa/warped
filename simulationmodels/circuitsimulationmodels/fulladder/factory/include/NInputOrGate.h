#ifndef N_INPUT_OR_GATE_H
#define N_INPUT_OR_GATE_H

#include "NInputGate.h"

/** The NInputOrGate class.
  This class represents an N input and gate.
*/

class NInputOrGate : public NInputGate {
public:
   
  /**@name Public Class Methods of NInputOrGate. */
  //@{

  /// Default constructor
  NInputOrGate(string &objectName, const int numInputs,/*const int numInValue,*/const int numOutputs,
               vector<string> *outputs, vector<int> *destInputPorts,int objectDelay);
  /// Default Destructor
  ~NInputOrGate(){}

  /// compute the outputs of this gate
  int computeOutput(const int* inputBits);
  
  //@} // End of Public Class Methods of NInputOrGate.
};

#endif
