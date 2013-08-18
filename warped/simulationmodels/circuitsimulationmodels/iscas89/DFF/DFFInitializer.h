#ifndef DFF_INI
#define DFF_INI

#include <warped/SimulationObject.h>
#include "LogicEvent.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class DFFInitializer : public SimulationObject {
public:
  /**@name Public Class Methods of LogicComponent.*/
  //@{
  /// Default Constructor
  DFFInitializer(string &objectName, const int numOutPuts,
                 vector<string>* outputs, vector<int> *desInputPorts);

  /// Default Destructor 
  virtual ~DFFInitializer();

  /// initialize the object
  virtual void initialize();

  /// finish up 
  virtual void finalize(); 

  /// execute an event
  virtual void executeProcess();

  /// allocate state for the kernel
  virtual State* allocateState();

  /// deallocate state for the kernel
  virtual void deallocateState(const State* state);

  /// delete this event 
  virtual void reclaimEvent(const Event *event);
  
  /// report any errors in the simulation 
  virtual void reportError(const string& msg, SEVERITY level);

  /// get the number of outputs of this component
  virtual int getNumberOfOutputs() const { return numberOfOutputs; }

  /// get the name of the object
  virtual const string &getName() const {return thisObjectName; }

  //@}// End of Public Class Methods of DFFInitializer

private:
  /**@name Private Class Attributes of DFFInitializer. */
  //@{
  
  /// the name of this object
  string thisObjectName;

  /// number of outputs
  int numberOfOutputs; 

  /// vector of destination object names
  vector<string>* outPuts;

  /// vector of destination input port numbers
  vector<int>* Ports;

  /// set of destination objects handls
  SimulationObject** outputHandles;

  //@} // End of Private Class Attribute of DFFInitializer.

};

#endif
