#ifndef SIMULATION_OBJECT_ADAPTER_H
#define SIMULATION_OBJECT_ADAPTER_H

#include "SimulationObject.h"

class SimulationObjectAdapter : public SimulationObject {
public:
  SimulationObjectAdapter(){}
  ~SimulationObjectAdapter(){}
  const string &getName() const { 
    static string retval = "";
    return retval; 
  }
  SimulationObject *getObjectHandle( const string& objectToGet ) const{ return 0; }
  void initialize(){}
  void finalize(){}
  void executeProcess(){}
  State* allocateState(){ return 0 ;}
  void deallocateState( const State *state ){}
  SerializedInstance* serializeEvent( Event* event ){ return 0; }
  void reclaimEvent( const Event* event ){}
};

#endif
