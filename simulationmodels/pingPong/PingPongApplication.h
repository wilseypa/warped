#ifndef PING_PONG_APPLICATION
#define PING_PONG_APPLICATION

#include <Application.h>
#include <IntVTime.h>

class PingPongApplication : public Application {
public:
  PingPongApplication( unsigned int initNumObjects, 
		       unsigned int numEventsPerObject,
		       unsigned int initNumBallsAtOnce,
		       bool initRandomDelays );

  int getNumberOfSimulationObjects(int mgrId) const;

  const PartitionInfo *getPartitionInfo( unsigned int numberOfProcessorsAvailable );
  
  int finalize();

  void registerDeserializers();
  
  const VTime &getPositiveInfinity(){ return IntVTime::getIntVTimePositiveInfinity(); }
  const VTime &getZero(){ return IntVTime::getIntVTimeZero(); }
  const VTime &getTime(string &){ return IntVTime::getIntVTimeZero(); }

private:
  vector<SimulationObject *> *getSimulationObjects();

  unsigned int numObjects;
  unsigned int numEventsPerObject;
  unsigned int numBallsAtOnce;
  bool randomDelays;
};

#endif
