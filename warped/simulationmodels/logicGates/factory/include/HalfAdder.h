#ifndef HALF_ADDER_H
#define HALF_ADDER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "LogicComponent.h"

/** The HalfAdder class.

    The HalfAdder class represents a half adder object.
*/
class HalfAdder : public LogicComponent {
public:
   /**@name Public Class Methods of HalfAdder. */
   //@{

   /// Default Constructor
   HalfAdder(string &objectName, const int numOutputs,
             vector<string> *outputs, vector<int> *destInputPorts,
             vector<int> *fanOutSize, int objectDelay);

   /// Default Destructor
   ~HalfAdder(){}
  
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

   /// report any errors in the simulation
   void reportError(const string& msg, SEVERITY level);

   /// reclaim/delete this event
   void reclaimEvent(const Event *event);
   
   /// update the output of this component
   void updateOutput();
  
   //@} // End of Public Class Methods of HalfAdder.

};

#endif
