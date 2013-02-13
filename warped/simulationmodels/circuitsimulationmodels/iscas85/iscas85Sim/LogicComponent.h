#ifndef LOGIC_COMPONENT_H
#define LOGIC_COMPONENT_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "LogicEvent.h"
#include <warped/SimulationObject.h>
#include <iostream>
#include <cstdlib>
using std::string;
using std::vector;


/** The LogicComponent base class.

    All components (objects) are derived from the LogicComponent base class.
*/
class LogicComponent : public SimulationObject {
public:
   
   /**@name Public Class Methods of LogicComponent. */
   //@{

   /// Default Destructor
   virtual ~LogicComponent();

   /// initialize this object
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

   void sendEvent(const int outputBitValue);
   
   //@} // End of Public Class Methods of LogicComponent.
  
protected:

   /**@name Protected Class Attributes of LogicComponent. */
   //@{
   
   /// the name of this object
   string myObjectName;

   /// number of outputs 
   int numberOfOutputs;

   /// vector of destination object names
   vector<string> *outputNames;

   /// vector of destination object input port numbers
   vector<int> *destinationInputPorts;

   /// vector of number of fanout objects on each port
   /// vector<int> *fanOuts;

   /// delay of this object
   int delay;

   /// set of destination object handles
   SimulationObject **outputHandles;

   //@} // End of Protected Class Attributes of LogicComponent.

   /**@name Protected Class Methods of LogicComponent. */
   //@{
   
   /// Default Constructor
   LogicComponent(string &objectName, const int numOutputs,
                  vector<string> *outputs, vector<int> *destInputPorts,
                  int objectDelay);

   //@} // End of Protected Class Methods of LogicComponent.
};

#endif
