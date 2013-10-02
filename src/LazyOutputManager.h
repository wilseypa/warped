#ifndef LAZY_OUTPUT_MANAGER_H
#define LAZY_OUTPUT_MANAGER_H


#include <fstream>
#include "warped.h"
#include "OutputManagerImplementationBase.h"

class Event;
class SimulationObject;
class OutputManager;

/** The LazyOutputManager class.

    This class implements a lazy cancellation scheme as a part of
    its output manager functionality.

*/
class LazyOutputManager : public OutputManagerImplementationBase {
public:

    /**@name Public Class Methods of LazyOutputManager. */
    //@{

    /** Constructor.

        @param simMgr Handle to the simulation manager.
    */
    LazyOutputManager(TimeWarpSimulationManager* simMgr);

    /// Destructor.
    virtual ~LazyOutputManager();

    virtual void configure(SimulationConfiguration&) {}

    /** Copies the output events after the rollback time to the lazy
        cancellation queue. Removes those events from the output queue.

        @param rollbackTime The time to which the output events are rolled back.
        @param object A pointer to the object who experienced rollback.
    */
    virtual void rollback(SimulationObject* object,  const VTime& rollbackTime);

    /** Returns true if the event is the same as a previously generated event.
        Returns false otherwise.

        This method calls checkLazyCancelEvent, handleCancelEvents, and insert
        in that order.

        @param event The regenerated event to be evaluated.
    */
    bool lazyCancel(const Event* event);

    /** This should be called when there are no more events to process in the
        event set. At that point there is no way to regenerate events so just
        send out the anti-messages for anything left in the lazy queues.

        @param time All events before this time will be removed.
    */
    virtual void emptyLazyQueues(const VTime& time);

    /** This should be called when there are no more events to process for the
        simulation object. At that point there is no way to regenerate events so just
        send out the anti-messages for anything left in the lazy queue of the
        simulation object.

        @param object The simulation object for which events will be removed.
        @param time All events before this time will be removed.
    */
    virtual void emptyLazyQueue(SimulationObject* object, const VTime& time);

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
    const VTime& getLazyQMinTime();

    /** Returns the lowest receive time of all events in the lazy queue
        of the given object. Used by the GVTManager to calculate GVT.

        @param objectID The id of the simulation object.
        @return VTime The lowest receive time.
    */
    const VTime& getLazyQMinTime(const unsigned int objectID);

    /**
       Remove all output events. Used to restore state after
       a catastrophic rollback while using optimistic fossil collection.
    */
    virtual void ofcPurge();

    //@} // End of Public Class Methods of LazyOutputManager.

protected:
    /**@name Protected Class Methods of LazyOutputManager. */
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
    bool checkLazyCancelEvent(const Event* event);

    /** Sends out antimessages for the events that exist in the cancel events
        queue.
    */
    void handleCancelEvents();

    /** Sends out antimessages for specified object.
    */
    void handleCancelEvents(SimulationObject* object);

    /** The lazy cancellation event queue. This stores the events that have a
        time greater the straggler time. They are to be compared to regenerated events.
    */
    //vector<const Event*> *lazyCancelEvents;

    /** List of events to be cancelled. Items are added during the lazyCancel check and then
        cancelled in handleCancelEvents.
    */
    vector< vector<const Event*> *> eventsToCancel;

    /** Used to keep track of the number of lazy hits for the round of cancellation.
    */
    vector<int> lazyHitCount;

    /* Used to keep track of the number of lazy misses for the round of cancellation.
    */
    vector<int> lazyMissCount;

    /* Contains the lazy cancellation queues for the objects contained in this logical
       process.
    */
    vector< vector<const Event*> *> lazyQueues;

    /* True for normal lazy cancellation operation.
       False when just using checkLazyCancelEvent for the comparison (no insert).
    */
    vector<bool> compareAndInsertMode;

    //@} // End of Protected Class Methods of LazyOutputManager.
};

#endif
