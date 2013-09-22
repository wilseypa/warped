#ifndef SMMPJOINSTATE_HH
#define SMMPJOINSTATE_HH

#include "State.h"

class SMMPJoinState : public State {
public:
 
   SMMPJoinState(){}; 
   ~SMMPJoinState(){};

   void copyState(const State *toCopy){};

   unsigned int getStateSize() const { return sizeof(SMMPJoinState); }
};

#endif
