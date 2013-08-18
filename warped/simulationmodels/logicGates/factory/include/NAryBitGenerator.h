#ifndef NARY_BIT_GENERATOR_H
#define NARY_BIT_GENERATOR_H

#include "warped.h"
#include "LogicComponent.h"
#include "SimulationStream.h"

/** The NAryBitGenerator class.

    The NAryBitGenerator class represents a n-ary bit generator object.
*/
class NAryBitGenerator : public LogicComponent {
public:
   /**@name Public Class Methods of NAryBitGenerator. */
   //@{

   /// Default Constructor
   NAryBitGenerator(string &objectName, const int numOutputs,
                    vector<string> *outputs, vector<int> *destInputPorts,
                    vector<int> *fanOutSize, int numIterations, int numBits,
                    int objectDelay, bool print=false);

   /// Default Destructor
   ~NAryBitGenerator() {}
  
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

   /// reclaim this event
   void reclaimEvent(const Event *event);
   
   /// report any errors in the simulation
   void reportError(const string& msg, SEVERITY level);

   //@} // End of Public Class Methods of 

protected:
   /**@name Protected Class Attributes of NAryBitGenerator. */
   //@{

   /// number of bits to generate
   int numberOfBits;

   /// maximum count
   int maxCount;

   /// maximum iteration count
   int maxIterations;

   /// handle to simulation stream
   SimulationStream *myStream;

   /// boolean flag indicating print status
   bool printFlag;
   
   //@} // End of Protected Class Attributes of NAryBitGenerator.

private:
   /**@name Private Class Methods of NAryBitGenerator. */
   //@{

   /// update the output of this component
   void updateOutput();

   //@} // End of Private Class Methods of NAryBitGenerator.
};

#endif
