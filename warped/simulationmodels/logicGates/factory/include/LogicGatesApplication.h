#ifndef LOGIC_GATES_APPLICATION
#define LOGIC_GATES_APPLICATION

#include <warped/Application.h>
#include <warped/IntVTime.h>

class LogicGatesApplication : public Application {
public:
  LogicGatesApplication();
  
  int initialize( vector<string> &arguments);
 
  int getNumberOfSimulationObjects(int mgrId) const;
  
  vector<SimulationObject *>* getSimulationObjects();

  const PartitionInfo *getPartitionInfo( unsigned int numberOfProcessorsAvailable);

  int finalize();

  void registerDeserializers();

  string getCommandLineParameters() const {return "foo"; }

  const VTime &getPositiveInfinity(){return IntVTime::getIntVTimePositiveInfinity();}
  const VTime &getZero(){ return IntVTime::getIntVTimeZero();}
  const VTime &getTime(string &){return IntVTime::getIntVTimeZero();}

private:
  ArgumentParser &getArgumentParser();

  unsigned int mumObjects;
  string inputFileName;
}

#endif 
