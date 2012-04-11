#ifndef THREADEDCOSTADAPTIVESTATEMANAGER_HH
#define THREADEDCOSTADAPTIVESTATEMANAGER_HH

// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedAdaptiveStateManagerBase.h"

/** The ThreadedCostAdaptiveStateManager class.

 This class is an implementation of the adaptive state manager base.
 The adaptive algorithm is Fleischman's and Wilsey's heuristic
 State Saving algorithm.

 */

class ThreadedCostAdaptiveStateManager: public ThreadedAdaptiveStateManagerBase {
public:

	/**@name Public Class Methods of ThreadedCostAdaptStateManager. */
	//@{

	/** Constructor.

	 @param simMgr Simulation manager.
	 */
	ThreadedCostAdaptiveStateManager(ThreadedTimeWarpSimulationManager *simMgr);

	// Destructor
	~ThreadedCostAdaptiveStateManager() {
	}
	;

	/** Calculates the new period for the given object.

	 @param object The object for which the new period is calculated.
	 */
	void calculatePeriod(SimulationObject *object);

	/** Sets the coast foward time.

	 @param coastforwardtime This value should be the elapsed time recorded during coast forward phase.
	 */
	void coastForwardTiming(unsigned int id, double coastforwardtime);

	/** Returns the coast forward time.
	 */
	double getCoastForwardTime(unsigned int id);

	/** Saves the state of the object. After the interval defined by eventsBetweenRecalculation
	 is completed, the period is recalulated.

	 @param currentTime The current time to save state.
	 @param object The object for which the state is to be saved.
	 */
	void saveState(const VTime& currentTime, SimulationObject *object,
			int threadId);

	//@} // end of Public Class Methods

protected:

	/**@name Private Class Attributes */
	//@{

	// Number of events between recalculation of the checkpoint.
	vector<long> eventsBetweenRecalculation;

	// Events executed since last checkpoint interval recalculation.
	vector<long> forwardExecutionLength;

	// The value of the cost function.
	vector<double> filteredCostIndex;

	// The previous value of the cost function.
	vector<double> oldCostIndex;

	// Adaption value for period (magnitude of 1 and sign gives direction).
	vector<int> adaptionValue;

	//@} // end of Private Class Attributes
};

#endif
