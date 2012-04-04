#ifndef DTTIMEWARPMULTISET_H_
#define DTTIMEWARPMULTISET_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include <set>
#include <list>
#include "DTTimeWarpEventSet.h"
#include "AtomicState.h"
#include "NegativeEvent.h"
#include "EventFunctors.h"
#include "DTTimeWarpSimulationManager.h"

using std::multiset;
using std::list;

class Event;
class NegativeEvent;
class DTTimeWarpSimulationManager;
//class DTTimeWarpMultiSetSchedulingManager;
//class SimulationObject;

class DTTimeWarpMultiSet: public DTTimeWarpEventSet {
public:
	DTTimeWarpMultiSet(DTTimeWarpSimulationManager *initSimulationManager);
	~DTTimeWarpMultiSet();

	/**
	 * rollback a particular objects Event Queue
	 * @param
	 * @return nothing.
	 */
	void rollback(SimulationObject *simObj, const VTime &rollbackTime,
			int threadId);

	/** To get total pending message in the InputEventQueue for all Objects
	 * @return Pending EventCount
	 */
	int getMessageCount(int threadId);
    bool isScheduleQueueEmpty(int threadId);

	/** Remove and return the next event in the event set.
	 *  @return The removed event.
	 */
	const Event *getEvent(SimulationObject *simObj, int threadId);

	/** Remove and return the next event in the event set.
	 *  @return The removed event.
	 */
	const Event *getEventWhileRollback(SimulationObject *simObj, int threadId);

	/** Remove and return the next event in the event set.
	 *  @return The removed event.
	 */
	const Event* getEventIfStraggler(SimulationObject *simObj, int threadId);

	/** Remove and return the next event in the event set.
	 *  @return The removed event.
	 */
	const Event *getEvent(SimulationObject *simObj, const VTime &minimumTime,
			int threadId);

	/** Return a reference to the next event in the event set.
	 * 	@return A reference to the next event in the event set.
	 */
	const Event *peekEvent(SimulationObject *simObj, int threadId);

	const Event *peekEventCoastForward(SimulationObject *simObj, int threadId);

	/** Return a reference to the next event in the event set.
	 * 	@return A reference to the next event in the event set.
	 */
	const Event *peekEvent(SimulationObject *simObj, const VTime &minimumTime,
			int threadId);

	/** Insert an event into the event set.
	 * 	@return The Insert Result
	 */
	bool insert(const Event*, int threadId);

	/** To get total pending message in the InputEventQueue for all Objects
	 *  @return Pending EventCount
	 */
	int getQueueEventCount(int objId);

	/** To print the complete Input event Queue
	 * @return nothing
	 */
	void printAll();

	void getunProcessedLock(int threadId, int objId);

	void releaseunProcessedLock(int threadId, int objId);

	void getProcessedLock(int threadId, int objId);

	void releaseProcessedLock(int threadId, int objId);

	void getremovedLock(int threadId, int objId);

	void releaseremovedLock(int threadId, int objId);

	bool handleAntiMessage(SimulationObject *simObj,
			const NegativeEvent* negativeEvent, int threadId);

	// Might need to polymorp this function with different Time parameter
	void fossilCollect(SimulationObject *simObj,
			const VTime &fossilCollectTime, int threadId);
	void fossilCollect(SimulationObject *simObj, int fossilCollectTime,
			int threadId);
	void fossilCollect(const Event *, int threadId);

	void fillProcessed(SimulationObject *simObj);

	void getScheduleQueueLock(int threadId);

	void releaseScheduleQueueLock(int threadId);

	void getObjectLock(int threadId, int objId);

	void releaseObjectLock(int threadId, int objId);

	bool isObjectScheduled(int objId);

	bool isObjectScheduledBy(int threadId, int objId);

	void updateScheduleQueueAfterExecute(int objId, int threadId);

	bool inThePast(const Event *, int threadId);

	/**
	 Remove all events from the event set. Used to restore state after
	 a catastrophic rollback while using optimistic fossil collection.
	 */
	void ofcPurge(int threadId);

	/** Overriden from Configurable */
	void configure(SimulationConfiguration &configure) {
	}

	//A Temp Function to find min of Schedule Queue, will be replaced by GVT calc Function
	const VTime* nextEventToBeScheduledTime(int threadID);

	//Function to get Minimum of all Unprocessed and executing Events
	const VTime &getMinimumOfAll(int threadId);

	//Peek the first Event and Lock the Unprocessed Queue for that Object
	const Event* peekEventLockUnprocessed(SimulationObject *simObj,
			int threadId);
	//Peek the first Event and Lock the Unprocessed Queue for that Object while Coasting
	const Event* peekEventLockUnprocessed(SimulationObject *simObj,
			const VTime &minimumTime, int threadId);
	const VTime* getMinEventTime(unsigned int threadId, unsigned int objId);

	// Release all the object locks during a catastrophic rollback.
	void releaseObjectLocksRecovery();

private:
	AtomicState** unprocessedQueueAtomicState;
	AtomicState** processedQueueAtomicState;
	AtomicState** removedQueueAtomicState;

	//	typedef std::multiset<const Event*, receiveTimeLessThanEventIdLessThan>* MS;
	//	MS *unProcessedQueueArray;

	/// Queues to hold the unprocessed Events for each simObj.
	vector<multiset<const Event*, receiveTimeLessThanEventIdLessThan>*>
			unProcessedQueue;

	/// Iterator for the Events in Multiset.
	multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator
			unProcessedQueueIterator;

	/// Queues to hold the processed Events for each simObj.
	vector<vector<const Event*>*> processedQueue;

	/// Queues to hold the removed events for each simObj.
	vector<vector<const Event*>*> removedEventQueue;

	/// Iterators for each thread
	typedef vector<const Event*>::iterator vIterate;
	vIterate *vectorIterator;
	typedef multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator
			mIterate;
	mIterate *multisetIterator;

	//Lowest event position pointer
	vector<multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator>
			lowestObjectPosition;
	///Schedule Queue
	multiset<const Event*, receiveTimeLessThanEventIdLessThan> *scheduleQueue;
	///Schedule Queue Lock
	AtomicState* scheduleQueueLock;
	///Object Status Lock
	AtomicState** objectStatusLock;

	/// The handle to the simulation manager.
	DTTimeWarpSimulationManager* mySimulationManager;

	//number of objects associated with the manager
	int objectCount;
};

#endif /* DTTIMEWARPMULTISET_H_ */
