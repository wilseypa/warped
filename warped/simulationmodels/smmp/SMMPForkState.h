#ifndef SMMPFORKSTATE_HH
#define SMMPFORKSTATE_HH

#include "../rnd/MLCG.h"
#include "State.h"

class SMMPForkState : public State {
public:

   SMMPForkState(){
      gen = NULL;
      lastOutput = 0;
      localDelay = 0;
   }

   ~SMMPForkState(){
      delete gen;
   }

   void copyState(const State *toCopy){
      const SMMPForkState *copy = static_cast<const SMMPForkState*>(toCopy);
      delete gen;
      gen = new MLCG(*(copy->gen));
      lastOutput = copy->lastOutput;
      localDelay = copy->localDelay;
   }

   unsigned int getStateSize() const { return sizeof(SMMPForkState); }

   MLCG *gen ;
   int lastOutput;
   int localDelay;
};

#endif
