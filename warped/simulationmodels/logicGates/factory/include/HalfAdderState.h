#ifndef HALF_ADDER_STATE_H
#define HALF_ADDER_STATE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "FlatState.h"

/** The HalfAdderState class.

    This class represents the state of the HalfAdder object. Since
    this state has no pointer member data, it is derived from
    FlatState.
*/
class HalfAdderState : public FlatState {
public:
   
   /**@name Public Class Methods of HalfAdderState. */
   //@{

   /// Default Constructor
   HalfAdderState(){}

   /// Default Destructor
   ~HalfAdderState() {}
  
   /// return the size of this class to the caller
   const int getSize() const {
      return sizeof(HalfAdderState);
   }
   
   //@} // End of Public Class Methods of HalfAdderState.

   /**@name Public Class Attributes of HalfAdderState. */
   //@{

   /// the first input
   int inputBit1;

   /// the second input
   int inputBit2;

   /// the first output
   int outputBit1;

   /// the second output
   int outputBit2;

   //@} // End of Public Class Attributes of HalfAdderState.
};

#endif
