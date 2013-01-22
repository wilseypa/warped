// Copyright (c) The University of Cincinnati.  
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF 
// THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.
// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

#ifndef MEMSOURCESTATE_HH
#define MEMSOURCESTATE_HH

#include "State.h"
#include "controlkit/IIRFilter.h"
#include "IntVTime.h"

class MemSourceState : public State {
public:
   MemSourceState(): 
      numMemRequests(0),
      maxMemRequests(0),
      oldDelay(0),
      localdelay(0){
   };

   ~MemSourceState(){};

   void copyState(const State *toCopy){
      ASSERT(toCopy != NULL);
      const MemSourceState *copy = static_cast<const MemSourceState*>(toCopy);
      maxMemRequests = copy->maxMemRequests;
      numMemRequests = copy->numMemRequests;
      oldDelay = copy->oldDelay;
      localdelay = copy->localdelay;
      filter = copy->filter;
   }

   unsigned int getStateSize() const { return sizeof(MemSourceState); }

   /// The number of tokens that the object will produce.
   int maxMemRequests;

   /// The number of tokens produced so far.
   int numMemRequests;

   /// The previous length of time required to satisfy memory request.
   IntVTime oldDelay;

   /// The delay of the source object.
   int localdelay;

   /// Filter used to keep track of the lengths of memory request times.
   IIRFilter<double> filter;
};

#endif
