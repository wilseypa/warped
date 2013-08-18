#ifndef NARY_BIT_GENERATOR_STATE_H
#define NARY_BIT_GENERATOR_STATE_H

#include "FlatState.h"

class NAryBitGeneratorState : public FlatState {
public:

   /**@name Public Class Methods of NAryBitGeneratorState. */
   //@{

   /// Default constructor
   NAryBitGeneratorState(){}

   /// Default destructor
   ~NAryBitGeneratorState(){}

   /// return the size of this object state
   const int getSize() const {
      return sizeof(NAryBitGeneratorState);
   }
 
   //@} // End of Public Class Methods of NAryBitGeneratorState.

   /**@name Public Class Attributes of NAryBitGeneratorState. */
   //@{

   int counterState;
   int iteration;

   //@} // End of Public Class Attributes of NAryBitGeneratorState.
};

#endif
