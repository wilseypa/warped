#ifndef N_INPUT_GATE_STATE_H
#define N_INPUT_GATE_STATE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <warped/FlatState.h>

class NInputGateState : public FlatState {
public:
   /**@name Public Class Methods of NInputGateState. */
   //@{

   /// Default constructor
   NInputGateState(int numberOfInputBits){
      inputBits = new int[numberOfInputBits];
      numEvents = 0;
   }

   /// Default destructor
   ~NInputGateState(){};

   /// return the size of this object state
   const int getSize() const {
      return sizeof(NInputGateState);
   }
   unsigned int getStateSize() const {};
 
   //@} // End of Public Class Methods of NInputGateState.

   /**@name Public Class Attributes of NInputGateState. */
   //@{

   /// input bits
   int *inputBits;

   /// output bit
   int outputBit1;

   int numEvents;
   
   //@} // End of Public Class Attributes of NInputGateState.
};

#endif


