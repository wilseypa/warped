#ifndef N_INPUT_AND_GATE_H
#define N_INPUT_AND_GATE_H

// See copyright notice in file Copyright in the root directory of this archive.

//#include "NInputGate.h"

//#include <warped/SimulationObject.h>
//#include <warped/warped.h>

#include <warped/SimulationObject.h>
#include <warped/warped.h>

#include <vector>
#include <string>
#include <sstream>
#include <string>

using namespace std;

using std::stringstream;
using std::string;
using std::vector;

/** The NInputAndGate class.
    
    This class represents an N input and gate.
*/

class NInputAndGate : public SimulationObject {
 public:
  
  /**@name Public Class Methods of NInputAndGate. */
  //@{
  
  /// Default constructor
  NInputAndGate(string &objectName, 
		const int numInputs, 
		const int numOutputs,
		vector<string> outputs, 
		vector<int> *destInputPorts,
		vector<int> *fanOutSize, 
		int objectDelay);

  void initialize();
  virtual void executeProcess();
  virtual void finalize();

  State *allocateState();
  void deallocateState ( const State* state );
  
  void reclaimEvent ( const Event *event );
  void reportError ( const string& , SEVERITY );
  
  const string &getName() const 
  {
    return myObjectName;
  }
  
  /// Default Destructor
  ~NInputAndGate(){}
  
  /// compute the outputs of this gate
  //  int computeOutput(const int* inputBits);
  int FLAG_bit;
  int x ;
  int iRecvEvent;
  
  string sTempObjectName; 
  string sRecvObjectName;
  //  bool bStartSimulation;
  /* 
     I have to check the below given case that why private member is generating 
     an error.
  */
  /*pplication.o -MD -MP -MF .deps/NInputAndGateApplication.Tpo -c -o NInputAndGateApplication.o `test -f 'logicGates/factory/src/NInputAndGateApplication.cpp' || echo './'`logicGates/factory/src/NInputAndGateApplication.cpp
    In file included from logicGates/factory/src/NInputAndGateApplication.cpp:2:0:
    logicGates/factory/src/../include/NInputAndGate.h:78:14: error: ISO C++ forbids initialization of member ‘FLAG’ [-fpermissive]
    logicGates/factory/src/../include/NInputAndGate.h:78:14: error: making ‘FLAG’ static [-fpermissive]
    logicGates/factory/src/../include/NInputAndGate.h:78:14: error: ISO C++ forbids in-class initialization of non-const static member ‘FLAG’
  */
 private:
  string myObjectName;
  int numberInputs ;
  int numObjects;
  vector<string> outputs_send;
  vector<int> *viOutputPorts;
  vector<int> *viFanOut;
  int gateObjectDelay;  
  
  vector<SimulationObject *> outputHandles;
  //@} // End of Public Class Methods of NInputAndGate.
  //ifstream configfile;
  //  string inputFileName;
};

#endif
