#ifndef SMMP_APPLICATION
#define SMMP_APPLICATION

#include <warped/Application.h>
#include <warped/IntVTime.h>

class SMMPApplication : public Application {
public:
  SMMPApplication( string inputFileName, int numObjects );

  int getNumberOfSimulationObjects(int mgrId) const;

  const PartitionInfo *getPartitionInfo( unsigned int numberOfProcessorsAvailable );
  
  int finalize();

  void registerDeserializers();
  
  const VTime &getPositiveInfinity(){ return IntVTime::getIntVTimePositiveInfinity(); }
  const VTime &getZero(){ return IntVTime::getIntVTimeZero(); }
  const VTime &getTime(string &){ return IntVTime::getIntVTimeZero(); }

private:
  unsigned int numObjects;
  string inputFileName;
};

#endif
