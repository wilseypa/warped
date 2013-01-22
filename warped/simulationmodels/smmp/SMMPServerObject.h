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

#ifndef SMMPSERVEROBJECT_HH
#define SMMPSERVEROBJECT_HH

#include <iostream>
#include "../rnd/Normal.h"
#include "../rnd/Poisson.h"
#include "../rnd/Binomial.h"
#include "../rnd/Uniform.h"
#include "../rnd/NegExp.h"
#include "../rnd/Random.h"

#include "SidTypes.h"
#include "SimulationObject.h"

#define SERVERDELAY 20

class SMMPServerObject : public SimulationObject {
public:
  SMMPServerObject(string initName, string initDest, double initSeed);
  ~SMMPServerObject();
  
  void initialize();
  void finalize();

  void executeProcess();
  void setServerDistribution(distribution_t distribution, double firstParameter,
                             double secondParameter = 0, double (*func)() = NULL);  
  
  // User defined function which will be used for variable delays that
  // Depend upon some state variable.
  double (*userFunction)();
  
  State *allocateState();
  void deallocateState( const State* state );
  
  void reclaimEvent(const Event *event);
  const string &getName() const { return myObjectName; }

private:
   /// The name of this object.
   string myObjectName;

   /// The name of the destination object.
   string dest;

   /// The type of distribution for this object. In this
   /// case, it will always be fixed.
   distribution_t serverDist;

   /// The increment in time from the send time to the receive time.
   double delay;

   /// For this simulation, 'first' just indicates the time between
   /// the send and receive times.
   double first;

   /// Used for other queueing simulations. Not used here.
   double second;

   /// The seed value for random generation.
   double seed;
};

#endif 
