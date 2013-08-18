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
