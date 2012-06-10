#ifndef THREADEDAGGRESSIVEOUTPUTMANAGER_H_
#define THREADEDAGGRESSIVEOUTPUTMANAGER_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include <fstream>
#include "warped.h"
#include "ThreadedOutputManagerImplementationBase.h"

class Event;
class SimulationObject;
class OutputManager;

/** The ThreadedAggressiveOutputManager class.

 This class implements an aggressive cancellation scheme as a part of
 its output manager functionality.

 */
class ThreadedAggressiveOutputManager: public ThreadedOutputManagerImplementationBase {
public:

	/**@name Public Class Methods of ThreadedAggressiveOutputManager. */
	//@{

	/** Constructor.

	 @param simMgr Handle to the simulation manager.
	 */
	ThreadedAggressiveOutputManager(ThreadedTimeWarpSimulationManager *simMgr);

	/// Destructor.
	~ThreadedAggressiveOutputManager();

	/** Send out anti-messages.

	 @param rollbackTime Bound for how many antimessages will be sent.
	 @param object A pointer to the object who experienced rollback.
	 */
	void rollback(SimulationObject *object, const VTime &rollbackTime,
			int threadID);

	virtual void configure(SimulationConfiguration &) {
	}

	//@} // End of Public Class Methods of ThreadedAggressiveOutputManager.

};

#endif /* THREADEDAGGRESSIVEOUTPUTMANAGER_H_ */
