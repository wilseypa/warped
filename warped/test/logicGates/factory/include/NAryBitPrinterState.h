#ifndef NARY_BIT_PRINTER_STATE_H
#define NARY_BIT_PRINTER_STATE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "State.h"

/** The NAryBitPrinterState class.

    This class represents the state of the NAryBitPrinter object. Since
    this state has a pointer member data, it is derived from
    State.
*/
class NAryBitPrinterState : public State {
public:
   
   /**@name Public Class Methods of NAryBitPrinterState. */
   //@{

   /// Default Constructor
   NAryBitPrinterState(int numBits){
      numberOfBits = numBits;
      bitVector = new int[numBits];
   }

   /// Default Destructor
   ~NAryBitPrinterState(){
      delete bitVector;
   }
  
   /// return the size of this class to the caller
   const int getSize() const {
      return sizeof(NAryBitPrinterState);
   }

   /// copy the state properly
   void copyState(const State* oldState){
      this->numberOfBits = ((NAryBitPrinterState *)oldState)->numberOfBits;
      for(int i = 0; i < numberOfBits; i++){
         this->bitVector[i] = ((NAryBitPrinterState *)oldState)->bitVector[i];
      }
   }
   
   
   //@} // End of Public Class Methods of NAryBitPrinterState.
  
   /**@name Public Class Attributes of NAryBitPrinterState. */
   //@{

   /// vector of bits
   int *bitVector;

   /// number of bits to print
   int numberOfBits;

   //@} // End of Public Class Attributes of NAryBitPrinterState.
};

#endif
