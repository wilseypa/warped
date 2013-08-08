#ifndef PHOLD_APPLICATION
#define PHOLD_APPLICATION

// See copyright notice in file Copyright in the root directory of this archive.

#include <warped/Application.h>
#include <warped/IntVTime.h>

class PHOLDApplication : public Application {
public:
  PHOLDApplication();

  int initialize( vector<string> &arguments );

  int getNumberOfSimulationObjects(int mgrId) const;

  const PartitionInfo *getPartitionInfo( unsigned int numberOfProcessorsAvailable );
  
  int finalize();

  void registerDeserializers();
  
  string getCommandLineParameters() const { return "foo"; }

  const VTime &getPositiveInfinity(){ return IntVTime::getIntVTimePositiveInfinity(); }
  const VTime &getZero(){ return IntVTime::getIntVTimeZero(); }
  const VTime &getTime(string &time){
    IntVTime *vtime = new IntVTime( atoi(time.c_str()) );
    return *vtime;
  }

private:
  vector<SimulationObject *> *getSimulationObjects();

  string inputFileName;
  int numObjects;
  int numLPs;
};

#endif
