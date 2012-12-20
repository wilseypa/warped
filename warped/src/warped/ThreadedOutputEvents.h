// See copyright notice in file Copyright in the root directory of this archive.

#ifndef THREADEDOUTPUTEVENTS_H_
#define THREADEDOUTPUTEVENTS_H_

#include "LockState.h"
#include "warped.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include <vector>
#include <algorithm>
#include <list>
#include <fstream>
#include "SetObject.h"
using std::vector;
using std::list;
using std::ofstream;

class Event;
class VTime;

/** The ThreadedOutputEvents class.

 This class is a container for output events of a single
 simulation object.
 */
class ThreadedOutputEvents {
public:

	/**@name Public Class Methods of ThreadedOutputEvents. */
	//@{

	/** Constructor
	 @param simMgr A handle to the simulation manager.
	 */
	ThreadedOutputEvents(ThreadedTimeWarpSimulationManager *simMgr);

	/// Destructor.
	~ThreadedOutputEvents();

	/** Returns the events sent at or after the time. Does not remove them
	 from the container.

	 @param searchTime Time to use.
	 @return vector The vector of the events sent at or after the search time.
	 */
	vector<const Event *> *getEventsSentAtOrAfter(const VTime &searchTime,
			int threadID);

	/** Returns the events sent at or after the search time and removes them from
	 this container.

	 @param searchTime Time to use.
	 @return vector The vector of the events sent at or after the search time.
	 */
	vector<const Event *> *getEventsSentAtOrAfterAndRemove(
			const VTime &searchTime, int threadID);

	/** Inserts the event into the container.
	 @param Event The event to insert.
	 */
	void insert(const Event *newEvent, int threadID);

	/** Reclaim any events with a send time less than the
	 garbage collect time.

	 @param gCollectTime The time to fossil collect at.
	 */
	void fossilCollect(const VTime &gCollectTime, int threadID);

	/** Reclaim any events with a send time less than the
	 garbage collect time. Uses an integer time instead of VTime.
	 Used for optimistic fossil collection.

	 @param gCollectTime The time to fossil collect at.
	 */
	void fossilCollect(int gCollectTime, int threadID);

	/** Reclaims only a single event.

	 @param toRemove The event to reclaim.
	 */
	void fossilCollectEvent(const Event *toRemove, int threadID);

	/** Returns the event with the lowest time stamp that
	 also has the given size in bytes. Used for optimistic
	 fossil collection.

	 @param size The number of bytes of the event.
	 @return Event The oldest event.
	 */
	const Event *getOldestEvent(unsigned int size, int threadID);

	/** Removes the event from the container.

	 @param toRemove The event to remove.
	 */
	void remove(const Event *toRemove, int threadID);

	/** Removes events from the container.

	 @param toRemove A vector of events to remove.
	 */
	void remove(const vector<const Event *> &toRemove, int threadID);

	/** Used in optimistic fossil collection. This function saves certain
	 necessary events to the given output file.

	 @param outFile The output file.
	 @param saveTime The time used to determine which event to write
	 to the output file.
	 */
	void saveOutputCheckpoint(ofstream* outFile, unsigned int saveTime,
			int threadID);

	/** Used in optimistic fossil collection. Removes all events from the container.
	 */
	void ofcPurge(int threadID);

	//@} // End of Public Class Methods of ThreadedOutputEvents.

	bool getLocalLock(int threadId);
	bool getRemoteLock(int threadId);
	bool getRemovedLock(int threadId);

	bool releaseLocalLock(int threadId);
	bool releaseRemoteLock(int threadId);
	bool releaseRemovedLock(int threadId);

private:

	/**@name Private Class Methods of ThreadedOutputEvents. */
	//@{

	/// The events with the sender and receiver simulation object on this
	/// simulation manager.
	vector<SetObject<Event>*> outputEventsLocal;

	/// The events with the receiver simulation object on a different
	/// simulation manager.
	vector<const Event *> outputEventsRemote;

	/// This list holds events that have been removed.
	list<const Event *> removedEventsRemote;

	/// An iterator,
	vector<const Event *>::iterator eventToRemove;

	/// True if the event to remove was actually removed.
	bool removedEventToRemove;

	/// A handle to the simulation manager.
	ThreadedTimeWarpSimulationManager *mySimulationManager;

	///Lock Definitions
	LockState* localQueueLock;
	LockState* remoteQueueLock;
	LockState* removedListLock;

	//@} // End of Private Class Methods of ThreadedOutputEvents.
};

#endif /* ThreadedOUTPUTEVENTS_H_ */
