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

#ifndef SMMPJOINOBJECT_HH
#define SMMPJOINOBJECT_HH

#include <iostream>
#include "SidTypes.h"
#include "SimulationObject.h"

class SMMPJoinObject : public SimulationObject {
public:
  
  SMMPJoinObject(string myName, string initDest);
  ~SMMPJoinObject();

  void initialize();
  void finalize();

  void executeProcess();
  void setJoinDestination(string destination, int delay);
  
  State *allocateState();
  void deallocateState( const State* state );
  
  void reclaimEvent(const Event *event);
  const string &getName() const { return myObjectName; }
  
private:
   /// The name of the object.
   string myObjectName;

   /// The name of the destination object.
   string dest;

   /// Used for the delay.
   int joinDelay;
};

#endif 
