#ifndef FULLADDER_APPLICATION
#define FULLADDER_APPLICATION

#include <warped/Application.h>
#include <warped/IntVTime.h>

class FullAdderApplication : public Application {
public:
  FullAdderApplication(int numObjects, string inputFileName);

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

