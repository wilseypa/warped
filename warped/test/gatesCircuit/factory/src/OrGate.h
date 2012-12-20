#ifndef OR_GATE_H
#define OR_GATE_H

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

/** The OrGate class.
 */

class OrGate : public SimulationObject
{
 public:
  /// Default constructor
  OrGate(string &objectName, 
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
  ~OrGate(){}
  
 private:
  string myObjectName;
  int numberInputs ;
  int numberObjects ;
  vector<string> vsOutputGates;
  vector<int> *viOutputPorts;
  vector<int> *viFanOut;
  int gateObjectDelay;  
  
  vector<SimulationObject *> outputHandles;
};

#endif
