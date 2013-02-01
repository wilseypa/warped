#ifndef THREADEDSTATEMANAGERIMPLEMENTATIONBASE_H_
#define THREADEDSTATEMANAGERIMPLEMENTATIONBASE_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "SetObject.h"
#include "ThreadedStateManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "LockState.h"

#include <set>
using std::multiset;

/** The ThreadedStateManagerImplementationBase class.

 The ThreadedStateManagerImplementationBase class is an implementation of
 the abstract StateManager class. It contains the methods and class
 attributes common to the PeriodicStateManager and the
 AdaptiveStateManager.

 */
class ThreadedStateManagerImplementationBase: public ThreadedStateManager {
public:

	friend class OptFossilCollManager;

	/**@name Public Class Methods of ThreadedStateManagerImplementationBase. */
	//@{

	/** Constructor.

	 @param simMgr a handle to the simulation manager
	 @param period State-saving period.
	 */
	ThreadedStateManagerImplementationBase(ThreadedTimeWarpSimulationManager *simMgr,
			unsigned int period = 0);

	/// Default Destructor.
	virtual ~ThreadedStateManagerImplementationBase();

	/** Save the state of the object at the specified time.

	 @param currentTime Time at which the state is saved.
	 @param object The object whose state is saved.
	 */
	virtual void saveState(const VTime &currentTime, unsigned int eventNumber,
			SimulationObject* object, const ObjectID senderId, int threadID);

	virtual void updateStateWhileCoastForward(const VTime &currentTime,
			unsigned int eventNumber, SimulationObject* object,const ObjectID senderId, int threadID);

	virtual void saveState(const VTime &currentTime, SimulationObject* object,
			int threadID);

	/// return the state saving period
	unsigned int getStatePeriod(int threadID);

	/** Returns the array of object state periods.
	 */
	vector<unsigned int> getObjectStatePeriod(int threadID);

	/** Rollback and restore the state of the object at the specified time.

	 @param rollbackTime Rollback to this time and restore state.
	 @param SimulationObject The object whose state should be restored.
	 @return the time of restored state
	 */
	virtual const VTime &restoreState(const VTime &rollbackTime,
			SimulationObject* object, int threadID);

	/** Fossil collect up to the specified simulation time.

	 @param fossilCollectTime Fossil collect upto this time.
	 @param object Object whose states are gbg collected.
	 @return lowest time-stamped state in state queue
	 */
	virtual const VTime &fossilCollect(SimulationObject* object,
			const VTime &fossilCollectTime, int threadID);

	/** Fossil collect up to the specified simulation time.

	 @param fossilCollectTime Fossil collect upto this time.
	 @param object Object whose states are gbg collected.
	 @return lowest time-stamped state in state queue
	 */
	virtual const VTime &fossilCollect(SimulationObject* object,
			int fossilCollectTime, int threadID);

	/** Print state queue.

	 @param currentTime Print statequeue upto current time.
	 @param object Object whose statequeue is printed.
	 */
	virtual void printStateQueue(const VTime &currentTime,
			SimulationObject *object, ostream &out, int threadID);

	/**
	 Remove all states from the state queues. Used to restore state after
	 a catastrophic rollback while using optimistic fossil collection.
	 */
	virtual void ofcPurge(int threadID);

	/**
	 Remove all states for the specified object. Used to restore state after
	 a catastrophic rollback while using optimistic fossil collection.
	 */
	virtual void ofcPurge(unsigned int objId, int threadID);

	//@} // End of Public Class Methods of ThreadedStateManagerImplementationBase.

	bool getStateQueueLock(int threadId, int objId);

	bool releaseStateQueueLock(int threadId, int objId);

	void initStateQueueLocks(ThreadedTimeWarpSimulationManager *simMgr);

	const unsigned int getEventIdForRollback(int threadId, int objId);

	const unsigned int  getSenderObjectIdForRollback(int threadId, int objId);

	const unsigned int  getSenderObjectSimIdForRollback(int threadId, int objId);

	// Called to release locks on all the State Queues
	virtual void releaseStateLocksRecovery();

protected:

	/**@name Protected Class Attributes of ThreadedStateManagerImplementationBase. */
	//@{

	/// handle to the simulation manager
	ThreadedTimeWarpSimulationManager *mySimulationManager;

	/// The state period - if infrequent, then statePeriod != 0.
	unsigned int statePeriod;

	/// The state saving period of each object
	vector<unsigned int> objectStatePeriod;

	/// Time since the last saved state.
	vector<int> periodCounter;

	/// A state queue for every object on this simulation manager.
	multiset<SetObject<State> > *myStateQueue;

	LockState** stateQueueLock;

	vector<unsigned int> rollbackEventNumber;

	vector<const VTime*> lastRollbackTime;

	vector<unsigned int> lastRollbackSenderObjectId;

	vector<unsigned int> lastRollbackSenderObjectSimId;

	//@} // End of Protected Class Attributes of ThreadedStateManagerImplementationBase.

private:
	string syncMechanism;

};

#endif /* ThreadedSTATEMANAGERIMPLEMENTATIONBASE_H_ */
