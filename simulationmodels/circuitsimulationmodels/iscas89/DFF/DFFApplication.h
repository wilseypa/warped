#ifndef DFF_APPLICATION
#define DFF_APPLICATION

#include "Application.h"
#include "IntVTime.h"

class DFFApplication : public Application{
public:

  DFFApplication(string inputFileName, int numObjects);

  int getNumberOfSimulationObjects(int mgrId) const;

  const PartitionInfo *getPartitionInfo( unsigned int numberOfProcessorsAvailable);

  int finalize();

  void registerDeserializers();

  const VTime &getPositiveInfinity(){return IntVTime::getIntVTimePositiveInfinity();}

  const VTime &getZero(){ return IntVTime::getIntVTimeZero();}

  const VTime &getTime(string &){return IntVTime::getIntVTimeZero();}

private:

  unsigned int numObjects;

  string inputFileName;

};

#endif

