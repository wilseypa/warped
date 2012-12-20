#ifndef AND_GATE_APPLICATION
#define AND_GATE_APPLICATION

#include <warped/Application.h>
#include <warped/IntVTime.h>
#include <sstream>

using namespace std;
using std::string;
using std::stringstream;

class NInputAndGateApplication : public Application {
public:
  NInputAndGateApplication();

  int initialize( vector<string> &arguments);

  int getNumberOfSimulationObjects(int mgrId) const;

  const PartitionInfo *getPartitionInfo( unsigned int numberOfProcessorsAvailable);

  int finalize();

  void registerDeserializers();

  string getCommandLineParameters() const {return "foo"; }

  const VTime &getPositiveInfinity(){return IntVTime::getIntVTimePositiveInfinity();}
  const VTime &getZero(){ return IntVTime::getIntVTimeZero();}
  const VTime &getTime(string &){return IntVTime::getIntVTimeZero();}
  
private:
  ArgumentParser &getArgumentParser();

  unsigned int numObjects;
  string inputFileName;
  stringstream ObjectId;
  string sObjectId;
  string objNameTemp;

  string objName;
  int numberOfOutputs; // Number of output objects
  string destObjName;
  int inputPortNumber;
  int outputPortNumber;
  int numberOfInputs;
  int objDelay;
  string temp_output;
};

#endif

