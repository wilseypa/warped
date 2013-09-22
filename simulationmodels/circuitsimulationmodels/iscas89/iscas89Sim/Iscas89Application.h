#ifndef ISCAS85_APPLICATION
#define ISCAS85_APPLICATION

#include <Application.h>
#include <IntVTime.h>

class Iscas89Application : public Application{
public:

  Iscas89Application(string inputFileName, string testCaseFileName, int numObjects);

  int getNumberOfSimulationObjects(int mgrId) const;

  const PartitionInfo *getPartitionInfo( unsigned int numberOfProcessorsAvailable);

  int finalize();

  void registerDeserializers();

  const VTime &getPositiveInfinity(){return IntVTime::getIntVTimePositiveInfinity();}

  const VTime &getZero(){return IntVTime::getIntVTimeZero();}

  const VTime &getTime(string &){return IntVTime::getIntVTimeZero();}

private:
  unsigned int numObjects;

  string inputFileName;

  string testCaseFileName;
};

#endif

