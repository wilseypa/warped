#ifndef NOT_GATE_STATE_H
#define NOT_GATE_STATE_H

#include "FlatState.h"

/** The NotGateState class

    The NotGateState class represents the state of a "not" gate or
    inverter.
*/
class NotGateState : public FlatState {
public:

   /**@name Public Class Methods of NotGateState. */
   //@{

   /// Default constructor
   NotGateState(){}

   /// Default Destructor
  ~NotGateState(){}

   /// return the size of this object state
   const int getSize() const {
      return sizeof(NotGateState);
   }

   //@} // End of Public Class Methods of NotGateState

   /**@name Public Class Attributes of NotGateState. */
   //@{

   int counter;

   //@} // End of Public Class Attributes of NotGateState.
};

#endif
