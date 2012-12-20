#ifndef NOT_GATE_H
#define NOT_GATE_H

// See copyright notice in file Copyright in the root directory of this archive.

//#include "../include/NInputGate.h"

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

class NotGate : public SimulationObject
{
 public:
  
  /**@name Public Class Methods of NInputAndGate. */
  //@{
  
  /// Default constructor
  NotGate(string &objectName, 
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
  ~NotGate(){}
  
  /// compute the outputs of this gate
  //   int computeOutput(const int* inputBits);
  
  //@} // End of Public Class Methods of NInputAndGate.
  
 private:
  string myObjectName;
  int numberInputs ;
  int numberObjects ;
  vector<string> vsOutputGates;
  vector<int> *viOutputPorts;
  vector<int> *viFanOut;
  int gateObjectDelay;  
  
  vector<SimulationObject *> outputHandles;

  //int iInputBits[1];
  //int result = 1;
};

#endif
