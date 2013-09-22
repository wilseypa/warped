//-*-c++-*-
#ifndef SMMPFORKOBJECT_HH
#define SMMPFORKOBJECT_HH

#include <iostream>
#include "../rnd/Uniform.h"
#include "SimulationObject.h"
#include <vector>
using namespace std;

class SMMPForkObject : public SimulationObject {
public:
  
  SMMPForkObject(string myName, double initSeed, double cHitRat,
                 vector<string> outNames);
  ~SMMPForkObject();
  
  void initialize();
  void finalize(); 
  void executeProcess();
  
  State *allocateState();
  void deallocateState( const State* state );
  
  void reclaimEvent(const Event *event);

  const string &getName() const { return myObjectName; }

private:
  
  /// Seed value for random number generation.
  double seed;

  /// The cache hit ratio for the processor. Ranges from 0 to 1.
  double cacheHitRatio;
  
  /// Number of outputs.
  int numFanOut;
  
  /// Names of the outputs.
  vector<string> fanOutNames;

  /// Object handles for the outputs.
  vector<SimulationObject *> fanOutHandles;

  /// Object name.
  string myObjectName;
};

#endif
