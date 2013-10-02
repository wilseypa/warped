
#ifndef ThreadedLazyOutputManager_H_
#define ThreadedLazyOutputManager_H_

/*
 #include <set>
 #include <list>
 */
#include "EventFunctors.h"
#include "ThreadedOutputManagerImplementationBase.h"

/*using std::multiset;
 using std::list;*/

class Event;
class SimulationObject;
class OutputManager;

/** The ThreadedLazyOutputManager class.

 This class implements a lazy cancellation scheme as a part of
 its output manager functionality.

 */
class ThreadedLazyOutputManager: public ThreadedOutputManagerImplementationBase {
public:

    /**@name Public Class Methods of ThreadedThreadedLazyOutputManager. */
    //@{

    /** Constructor.

     @param simMgr Handle to the simulation manager.
     */
    ThreadedLazyOutputManager(ThreadedTimeWarpSimulationManager* simMgr);

    /// Destructor.
    ~ThreadedLazyOutputManager();

    virtual void configure(SimulationConfiguration&) {
    }

    /** Copies the output events after the rollback time to the lazy
     cancellation queue. Removes those events from the output queue.

     @param rollbackTime The time to which the output events are rolled back.
     @param object A pointer to the object who experienced rollback.
     */
    virtual void rollback(SimulationObject* object, const VTime& rollbackTime,
                          int threadId);

    /** Returns true if the event is the same as a previously generated event.
     Returns false otherwise.

     This method calls checkLazyCancelEvent, handleCancelEvents, and insert
     in that order.

     @param event The regenerated event to be evaluated.
     */
    bool lazyCancel(const Event* event, int threadId);

    /** Updats the gvt Queue whenever there is a change in the lazyQueue

     @param objectID The object for which the lazyQueue is modified
     @param threadId The thread working on that object
     */
    //  void lazyMinQueueUpdate(unsigned int objectID, int threadId);

    /** This should be called when there are no more events to process in the
     event set. At that point there is no way to regenerate events so just
     send out the anti-messages for anything left in the lazy queues.

     @param time All events before this time will be removed.
     */
    //  void emptyLazyQueues(const VTime &time, int threadId);

    /** This should be called when there are no more events to process for the
     simulation object. At that point there is no way to regenerate events so just
     send out the anti-messages for anything left in the lazy queue of the
     simulation object.

     @param object The simulation object for which events will be removed.
     @param time All events before this time will be removed.
     */
    virtual void emptyLazyQueue(SimulationObject* object, const VTime& time,
                                int threadId);

    /** Sets the compare mode of the event compare function. This is used
     by the adaptive output manager so that events can be compared without
     inserting them into the event set.

     @param obj The sender simulation object of the event.
     @param mode True if actually using lazy cancellation. False if only
     using the compare for aggressive cancellation.
     */
    void setCompareMode(SimulationObject* obj, bool mode);

    /** Returns the compare mode.

     @param obj The simulation object to get compare mode for.
     @return bool True if comparing and inserting (using lazy cancellation).
     */
    bool getCompareMode(SimulationObject* obj);

    /** Returns the lowest receive time of all events in all lazy queues.
     Used by the GVTManager to calculate GVT.

     @return VTime The lowest receive time.
     */
    //  const VTime &getLazyQMinTime(int threadId);

    /** Returns the lowest receive time of all events in the lazy queue
     of the given object. Used by the GVTManager to calculate GVT.

     @param objectID The id of the simulation object.
     @return VTime The lowest receive time.
     */
    //  const VTime &getLazyQMinTime(const unsigned int objectID, int threadId);

    /**
     Remove all output events. Used to restore state after
     a catastrophic rollback while using optimistic fossil collection.
     */
    virtual void ofcPurge(int threadId);

    //  void getLazyMinQueueLock(int threadId);

    //  void releaseLazyMinQueueLock(int threadId);

    //@} // End of Public Class Methods of ThreadedLazyOutputManager.

    // For testing
    void printAll();

protected:

    /**@name Protected Class Methods of ThreadedLazyOutputManager. */
    //@{

    /** Returns true if the event is the same as a previously generated event.
     Returns false otherwise.

     This method adds events to be cancelled to a queue but does not actually
     cancel the events. To cancel those events, call handleCancelEvents after
     calling this method.

     This method does not add the event being examined to the output queue
     unless there is a lazy hit. Call the insert method to add the event to
     the output queue for a lazy miss.

     @param event The regenerated event to be evaluated.
     */
    bool checkLazyCancelEvent(const Event* event, int threadId);

    /** Sends out antimessages for the events that exist in the cancel events
     queue.
     */
    void handleCancelEvents(int threadId);

    /** Sends out antimessages for specified object.
     */
    void handleCancelEvents(SimulationObject* object, int threadId);

    /** The lazy cancellation event queue. This stores the events that have a
     time greater the straggler time. They are to be compared to regenerated events.
     */
    //vector<const Event*> *lazyCancelEvents;

    /** List of events to be cancelled. Items are added during the lazyCancel check and then
     cancelled in handleCancelEvents.
     */
    vector<vector<const Event*> *> eventsToCancel;

    /** Used to keep track of the number of lazy hits for the round of cancellation.
     */
    vector<int*> lazyHitCount;

    /* Used to keep track of the number of lazy misses for the round of cancellation.
     */
    vector<int*> lazyMissCount;

    /* Contains the lazy cancellation queues for the objects contained in this logical
     process.
     */
    vector<vector<const Event*> *> lazyQueues;

    /* True for normal lazy cancellation operation.
     False when just using checkLazyCancelEvent for the comparison (no insert).
     */
    vector<bool*> compareAndInsertMode;

    //Lowest event position pointer to point to position of lowest Time Stamped Event (for each object)
    // in the Schedule Queue.
    //  vector<multiset<const Event*, sendTimeLessThanEventIdLessThan>::iterator> lowestObjectPosition;

    ///Schedule Queue: used for GVT calculation
    //  multiset<const Event*, sendTimeLessThanEventIdLessThan> *lazyMinQueue;

    ///Schedule Queue Lock
    //  AtomicState* lazyMinQueueLock;

    //@} // End of Protected Class Methods of ThreadedLazyOutputManager.
};

#endif /* ThreadedLazyOutputManager_H_ */
