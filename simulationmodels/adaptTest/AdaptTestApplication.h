#ifndef ADAPT_TEST_APPLICATION
#define ADAPT_TEST_APPLICATION

#include <warped/Application.h>
#include <warped/IntVTime.h>

class AdaptTestApplication : public Application {
public:
  AdaptTestApplication( unsigned int initNumObjects,
                        unsigned int numStragglers,
		        string initOutputMode,
		        bool initAdaptiveState );

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
  unsigned int numStragglers;
  string outputMode;
  bool adaptiveState;
};

#endif

