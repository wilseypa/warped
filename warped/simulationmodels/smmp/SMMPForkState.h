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
