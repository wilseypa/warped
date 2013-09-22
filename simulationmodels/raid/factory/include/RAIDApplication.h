#ifndef RAID_APPLICATION
#define RAID_APPLICATION

#include "Application.h"
#include "IntVTime.h"

class RAIDApplication : public Application {
public:
  RAIDApplication( string inputFileName, int numObjects );

  int getNumberOfSimulationObjects(int mgrId) const;

  const PartitionInfo *getPartitionInfo( unsigned int numberOfProcessorsAvailable );
  
  int finalize();

  void registerDeserializers();
  
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
};

#endif
