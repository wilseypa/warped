#ifndef ADAPT_TEST_APPLICATION
#define ADAPT_TEST_APPLICATION

// See copyright notice in file Copyright in the root directory of this archive.

#include <warped/Application.h>
#include <warped/IntVTime.h>

class AdaptTestApplication : public Application {
public:
  AdaptTestApplication( unsigned int initNumObjects,
                        unsigned int numStragglers,
		        string initOutputMode,
		        bool initAdaptiveState );

  int initialize( vector<string> &arguments );

  int getNumberOfSimulationObjects(int mgrId) const;

  const PartitionInfo *getPartitionInfo( unsigned int numberOfProcessorsAvailable );
  
  int finalize();

  void registerDeserializers();
  
  string getCommandLineParameters() const { return "foo"; }

  const VTime &getPositiveInfinity(){ return IntVTime::getIntVTimePositiveInfinity(); }
  const VTime &getZero(){ return IntVTime::getIntVTimeZero(); }
  const VTime &getTime(string &){ return IntVTime::getIntVTimeZero(); }

private:
  ArgumentParser &getArgumentParser();

  vector<SimulationObject *> *getSimulationObjects();

  unsigned int numObjects;
  unsigned int numStragglers;
  string outputMode;
  bool adaptiveState;
};

#endif

