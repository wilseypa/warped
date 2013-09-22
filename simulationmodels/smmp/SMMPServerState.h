#ifndef SMMPSERVERSTATE_HH
#define SMMPSERVERSTATE_HH

#include "../rnd/MLCG.h"
#include "State.h"

class SMMPServerState : public State {
public:
   SMMPServerState(){
      gen = NULL;
   }
   
   ~SMMPServerState(){
      delete gen;
   }

   void copyState(const State *toCopy){
      const SMMPServerState *copy = static_cast<const SMMPServerState*>(toCopy);
      delete gen;
      gen = new MLCG(*(copy->gen));
   }

   unsigned int getStateSize() const { return sizeof(SMMPServerState); }

   MLCG *gen;
};

#endif
