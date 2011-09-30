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
