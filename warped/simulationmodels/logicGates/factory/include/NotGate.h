#ifndef NOT_GATE_H
#define NOT_GATE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "LogicComponent.h"

/** The NotGate class

    The NotGate class represents the implementation of a not gate or
    inverter.
*/
class NotGate : public LogicComponent {
public:

   /**@name Public Class Methods of NotGate. */
   //@{

   /// Default Destructor
   NotGate(string &objectName, const int numOutputs,
           vector<string> *outputs, vector<int> *destInputPorts,
           vector<int> *fanOutSize, int objectDelay);
   
   /// Default Destructor
   ~NotGate();
   
   /// initialize this object
   void initialize();
   
   /// finish up
   void finalize();
   
   /// execute an event
   void executeProcess();
   
   /// allocate state for the kernel
   State* allocateState();

   /// deallocate state for the kernel
   void deallocateState(State* state);

   /// fossil collect this event
   void reclaimEvent(const Event *event);
   
   /// report any errors in the simulation
   void reportError(const string& msg, SEVERITY level);

};

#endif
