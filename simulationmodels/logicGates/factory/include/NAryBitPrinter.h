#ifndef NARY_BIT_PRINTER_H
#define NARY_BIT_PRINTER_H

#include "warped.h"
#include "LogicComponent.h"
#include "SimulationStream.h"

/** The NAryBitPrinter class.

    The NAryBitPrinter class represents a n-ary bit printer object.
*/
class NAryBitPrinter : public LogicComponent {
public:
   /**@name Public Class Methods of NAryBitPrinter. */
   //@{

   /// Default Constructor
   NAryBitPrinter(string &objectName, const int numOutputs,
                  vector<string> *outputs, vector<int> *destInputPorts,
                  vector<int> *fanOutSize, int numberOfBits,
                  bool print = false);

   /// Default Destructor
   ~NAryBitPrinter();
  
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

   /// set the delay of this component
   void setDelay(const int);

   /// get the delay of this component
   int getDelay() const;
  
   //@} // End of Public Class Methods of NAryBitPrinter.

protected:
   
   /**@name Protected Class Attributes of NAryBitPrinter. */
   //@{

   /// number of bits
   int numberOfBits;

   /// print flag
   bool printFlag;

   /// handle to the output stream
   SimulationStream *myStream;

   /// a trigger vector that keeps track of when to print
   vector<int> *triggerVector;
   
   //@} // End of Protected Class Attributes of NAryBitPrinter.

private:

   /**@name Private Class Attributes of NAryBitPrinter. */
   //@{

   /// update the output of this component
   void updateOutput();

   /// invoke ourselves
   void sendSelfTrigger();

   //@} // End of Private Class Attributes of NAryBitPrinter. 
};

#endif
