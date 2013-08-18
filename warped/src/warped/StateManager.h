#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H


#include "warped.h"
#include "Configurable.h"
#include "EventId.h"

class State;
class SimulationObject;

/** The StateManager abstract base class.

    This is the abstract base class for all state manager implementations in
    the kernel. Different state manager implementations MUST be derived from 
    this abstract base class.  

*/
class StateManager : virtual public Configurable {
public:
   /**@name Public Class Methods of StateManager. */
   //@{

   /// Virtual Destructor
   virtual ~StateManager(){};

   /// return the state saving period
   virtual unsigned int getStatePeriod() = 0;

   /** Save the state of the object at the specified time.
       
       This is a pure virtual function that has to be overridden.

       @param VTime Time at which the state is saved.
       @param SimulationObject The object whose state is saved.
   */
  virtual void saveState( const VTime &, SimulationObject* ) = 0;
   
   /** Restore the state of the object at the specified time.
       
       This is a pure virtual function that has to be overridden.

       @param VTime State-restoring time.
       @param SimulationObject The object whose state should be restored.
       @return time of restored state
   */
   virtual const VTime &restoreState( const VTime&, SimulationObject* ) = 0;

   /** Fossil collect old state info for this object.

       This is a pure virtual function that has to be overridden.

       @param VTime Fossil collect upto this time.
       @param SimulationObject Object whose states are gbg collected.
       @return lowest time-stamped state in state queue
   */
   virtual const VTime &fossilCollect( SimulationObject*, const VTime& ) = 0;

   /** Fossil collect old state info for this object.

       This is a pure virtual function that has to be overridden.

       @param int Fossil collect upto this time.
       @param SimulationObject Object whose states are gbg collected.
       @return lowest time-stamped state in state queue
   */
   virtual const VTime &fossilCollect( SimulationObject*, int ) = 0;

   /** Print state queue.

       This is a pure virtual function that has to be overridden.

       @param currentTime Print statequeue upto current time.
       @param SimulationObject Object whose statequeue is printed.
   */
   virtual void printStateQueue(const VTime &currentTime, SimulationObject*,
                                ostream &) = 0;

   /**
      Remove all states from the state queues. Used to restore state after
      a catastrophic rollback while using optimistic fossil collection.
   */
   virtual void ofcPurge() = 0;

   /**
      Remove all states for the specified object. Used to restore state after
      a catastrophic rollback while using optimistic fossil collection.
   */
   virtual void ofcPurge(unsigned int) = 0;
  
   //@} // End of Public Class Methods of StateManager.

protected:

   /**@name Protected Class Methods of StateManager. */
  //@{
   
   /// Default Constructor.
   StateManager() {};


   //@} // End of Protected Class Methods of StateManager.
};

#endif
