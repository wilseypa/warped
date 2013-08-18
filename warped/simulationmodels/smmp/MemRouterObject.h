#ifndef MEMROUTEROBJECT_HH
#define MEMROUTEROBJECT_HH

#include "SimulationObject.h"

/* The MemRouterObject Class.
   
   This class takes incoming memory requests and routes them back to
   main source processor object that sent the request.

*/
class MemRouterObject : public SimulationObject  {
public:
  MemRouterObject(string initName);
  ~MemRouterObject();

  void initialize();
  void finalize();
  
  void executeProcess();
  
  State *allocateState();
  void deallocateState( const State* state );
  
  void reclaimEvent(const Event *event);

  const string &getName() const;

private:
   string objectName;
};

#endif
