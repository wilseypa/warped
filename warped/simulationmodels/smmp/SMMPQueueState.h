#ifndef SMMPQUEUESTATE_HH
#define SMMPQUEUESTATE_HH

#include "State.h"

class SMMPQueueState : public State {
public:
   SMMPQueueState(){}

   ~SMMPQueueState(){}
  
   void copyState(const State *toCopy){}

   unsigned int getStateSize() const { return sizeof(SMMPQueueState); }
};

#endif
