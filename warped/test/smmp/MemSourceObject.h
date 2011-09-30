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

#ifndef MEMSOURCEOBJECT_HH
#define MEMSOURCEOBJECT_HH

#include <iostream>

#include "SimulationObject.h"
#include "MemEvents.h"
#include "controlkit/IIRFilter.h"

/*
   The MemSourceObject Class.

   This class represents a processor. It generates memory requests
   and receives requests that have been satisfied.

*/
class MemSourceObject : public SimulationObject  {
public:
  MemSourceObject(string initName, int max);
  ~MemSourceObject();

  void initialize();
  void finalize();
  
  void executeProcess();
  
  State *allocateState();
  void deallocateState( const State* state );
  
  void reclaimEvent(const Event *event);

  const string &getName() const;

  /** Set the destination object. 

      @param dest The name of the simulation object.
  */
  void setDestination(string dest);

  /** Get the destination object name

      @return string The name of the simulation object.
  */
  string getDestination();
  
private:
  /// This is the name used to represent the object.
  string objectName;

  /// The total number of requests the processor will generate.
  int maxMemRequests;

  /// The object that requests will initially be sent to.
  string destObjName;
};

#endif
