#ifndef DTSTATEMANAGER_H_
#define DTSTATEMANAGER_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "Configurable.h"
#include "EventId.h"

class State;
class SimulationObject;

/** The DTStateManager abstract base class.

 This is the abstract base class for all state manager implementations in
 the kernel. Different state manager implementations MUST be derived from
 this abstract base class.

 */
class DTStateManager: virtual public Configurable {
public:
	/**@name Public Class Methods of DTStateManager. */
	//@{

	/// Virtual Destructor
	virtual ~DTStateManager() {
	}
	;

	/// return the state saving period
	virtual unsigned int getStatePeriod(int threadID) = 0;

	/** Save the state of the object at the specified time.

	 This is a pure virtual function that has to be overridden.

	 @param VTime Time at which the state is saved.
	 @param SimulationObject The object whose state is saved.
	 */
	virtual void saveState(const VTime &, SimulationObject*, int threadID) = 0;

	/** Restore the state of the object at the specified time.

	 This is a pure virtual function that has to be overridden.

	 @param VTime State-restoring time.
	 @param SimulationObject The object whose state should be restored.
	 @return time of restored state
	 */
	virtual const VTime &restoreState(const VTime&, SimulationObject*,
			int threadID) = 0;

	/** Fossil collect old state info for this object.

	 This is a pure virtual function that has to be overridden.

	 @param VTime Fossil collect upto this time.
	 @param SimulationObject Object whose states are gbg collected.
	 @return lowest time-stamped state in state queue
	 */
	virtual const VTime &fossilCollect(SimulationObject*, const VTime&,
			int threadID) = 0;

	/** Fossil collect old state info for this object.

	 This is a pure virtual function that has to be overridden.

	 @param int Fossil collect upto this time.
	 @param SimulationObject Object whose states are gbg collected.
	 @return lowest time-stamped state in state queue
	 */
	virtual const VTime &fossilCollect(SimulationObject*, int, int threadID) = 0;

	/** Print state queue.

	 This is a pure virtual function that has to be overridden.

	 @param currentTime Print statequeue upto current time.
	 @param SimulationObject Object whose statequeue is printed.
	 */
	virtual void printStateQueue(const VTime &currentTime, SimulationObject*,
			ostream &, int threadID) = 0;

	/**
	 Remove all states from the state queues. Used to restore state after
	 a catastrophic rollback while using optimistic fossil collection.
	 */
	virtual void ofcPurge(int threadID) = 0;

	/**
	 Remove all states for the specified object. Used to restore state after
	 a catastrophic rollback while using optimistic fossil collection.
	 */
	virtual void ofcPurge(unsigned int, int threadID) = 0;

	//@} // End of Public Class Methods of DTStateManager.

protected:

	/**@name Protected Class Methods of DTStateManager. */
	//@{

	/// Default Constructor.
	DTStateManager() {
	}
	;

	//@} // End of Protected Class Methods of DTStateManager.
};

#endif /* DTSTATEMANAGER_H_ */
