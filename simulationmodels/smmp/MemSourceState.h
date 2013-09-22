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
