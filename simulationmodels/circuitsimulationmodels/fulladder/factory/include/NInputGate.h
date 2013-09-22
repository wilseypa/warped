#ifndef N_INPUT_GATE_H
#define N_INPUT_GATE_H

#include "LogicComponent.h"

/** The NInputGate abstract base class

    The NInputGate abstract base class represents the abstract
    interface of all N input gates. Any N input gate must be
    derived from this base class. This class cannot be instantiated.
*/

class NInputGate : public LogicComponent {
public:

   /**@name Public Class Methods of NInputGate. */
   //@{

   /// Default Destructor
   virtual ~NInputGate();

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

   /// report any errors in the simulation
   virtual void reportError(const string& msg, SEVERITY level);

   /// get the name of the object
   virtual const string &getName() const;

   /// delete this event
   virtual void reclaimEvent(const Event *event);
   
   /// update the output of this component
   virtual void updateOutput();
   /// compute the output of this component
   virtual int computeOutput(const int*) = 0;

   //@} // End of Public Class Methods of NInputGate.

protected:

   /**@name Protected Class Methods of NInputGate. */
   //@{
   
   NInputGate(string &objectName, const int numInputs,/*const int numInValue,*/const int numOutputs,
              vector<string> *outputs, vector<int> *destInputPorts,int objectDelay);

   //@} // End of Protected Class Methods of NInputGate.
   
   /**@name Protected Class Attributes of NInputGate. */
   //@{

   /// number of input bits
   int numberOfInputs;
   /// record times of reading files
   int numInputValue;     
   //@} // End of Public Class Methods of NInputGate.
};

#endif

