// See copyright notice in file Copyright in the root directory of this archive.

#ifndef THREADEDADAPTIVEOUTPUTMANAGER_H_
#define THREADEDADAPTIVEOUTPUTMANAGER_H_

#include "warped.h"
#include "ThreadedLazyOutputManager.h"
#include "EventFunctors.h"

//#define FILTER_DEPTH 16
//#define AGGRESSIVE_TO_LAZY 0.5
//#define LAZY_TO_AGGRESSIVE 0.2
//#define THIRD_THRESHOLD 0.1

enum cancellationModes {
	Aggressive, Lazy
};

class Event;
class SimulationObject;
class OutputManager;

/** The Threaded DynamicOutputManager class.

 This class implements an dynamic cancellation scheme as a part of
 its output manager functionality.

 */
class ThreadedDynamicOutputManager: public ThreadedLazyOutputManager {
public:

	/**@name Public Class Methods of ThreadedDynamicOutputManager. */
	//@{

	/** Constructor.

	 @param simMgr Handle to the simulation manager.
	 @param useThirdThreshold Default is false, not used.
	 */
	ThreadedDynamicOutputManager(ThreadedTimeWarpSimulationManager *simMgr,
			unsigned int filterDepth, double aggr2lazy, double lazy2aggr,
			double thirdThreshold, bool useThirdThreshold = false);

	// Destructor.
	~ThreadedDynamicOutputManager();

	/** Returns true if the event should be suppressed.
	 Returns false otherwise.

	 When in aggressive cancellation, lazy checks will be performed but
	 events will not be cancelled (they have already been cancelled through
	 aggressive).

	 When in lazy cancellation, the method will perform the same as the
	 lazyCancel method.

	 @param event The event to be checked.
	 */
	bool checkDynamicCancel(const Event *event, int threadId);

	/** Returns current cancellation mode.
	 */
	cancellationModes getCancelMode(int objID, int threadId) {
		return *curCancelMode[objID];
	}

	/** Sets the current cancellation mode.

	 @param mode The cancellation mode to be set.
	 */
	void setCancelMode(cancellationModes mode, int objID, int threadId) {
		*curCancelMode[objID] = mode;
	}

	/** Rolls back the output queue to rollbackTime.

	 Any events with send time greater than rollbackTime are added to the
	 lazy cancellation queue.

	 When in aggressive mode, anti-messages are also sent for those events.

	 @param rollbackTime The time to which the object is rolled back.
	 @param object A pointer to the object who experienced rollback.
	 */
	void rollback(SimulationObject *object, const VTime &rollbackTime,
			int threadId);

	// void emptyLazyQueues(const VTime &time, int theadId);

	void emptyLazyQueue(SimulationObject *object, const VTime &time,
			int threadId);

	/**
	 Remove all output events. Used to restore state after
	 a catastrophic rollback while using optimistic fossil collection.
	 */
	void ofcPurge(int theadId);

	//@} // End of Public Class Methods of ThreadedDynamicOutputManager.

protected:
	/**@name protected Class Methods of ThreadedDynamicOutputManager. */
	//@{

	/** Calculates the hit ratio and determines what mode to use.
	 Also returns the cancellation mode that the calculation determined.

	 @param objID The object ID of the object to set.
	 */
	cancellationModes determinecancellationModes(int objID, int threadId);

	/** The current cancellation mode.
	 */
	vector<cancellationModes*> curCancelMode;

	/** The hit count, how many lazy hits have occured.
	 */
	vector<int*> hitCount;

	/** The hit ratio is used to determine the cancellation mode.
	 */
	vector<float*> hitRatio;

	/** The filter depth determines how many comparisons are used.
	 */
	int filterDepth;

	/** The aggressive to lazy ratio determines when to switch from aggressive cancellation to lazy cancellation.
	 */
	double aggressive_to_lazy;

	/** The lazy to aggressive ratio determines when to switch from lazy cancellation to aggressive cancellation.
	 */
	double lazy_to_aggressive;

	/** when to permanently swithc to aggressive.
	 */
	double third_threshold;

	/** Keeps track of the results past comparisons.
	 */
	vector<vector<int> *> comparisonResults;

	/** Keeps track of index for comparisonResults.
	 */
	vector<int*> curMeasured;

	/** True if permanently in aggressive mode.
	 */
	vector<bool*> permanentlyAggressive;

	/** True if using 3 threshold values instead of 2.
	 */
	bool thirdThreshold;

	//@} // End of Protected Class Methods of DynamicOutputManager.
};

#endif /* THREADEDADAPTIVEOUTPUTMANAGER_H_ */
