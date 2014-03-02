#ifndef TIME_WARP_MULTI_SET_H
#define TIME_WARP_MULTI_SET_H


#include <iosfwd>                       // for ostream
#include <list>                         // for list
#include <set>                          // for multiset, etc
#include <vector>                       // for vector

#include "EventFunctors.h"
#include "NegativeEvent.h"
#include "TimeWarpEventSet.h"           // for TimeWarpEventSet

class SimulationConfiguration;
class SimulationObject;
class VTime;

using std::multiset;
using std::list;

class Event;
class NegativeEvent;
class TimeWarpSimulationManager;

/** The TimeWarpMultiSet class.

STL multiset implementation of the event set.
*/
class TimeWarpMultiSet : public TimeWarpEventSet {
public:

    /**@name Public Class Methods of TimeWarpMultiSet. */
    //@{

    /// Default Constructor.
    TimeWarpMultiSet(TimeWarpSimulationManager* initSimulationManager);

    /// Destructor.
    virtual ~TimeWarpMultiSet();

    /** Insert an event into the event set.

    @param event The event to be inserted.
    */
    virtual bool insert(const Event* event);

    /** Cancel out positive message corresponding to the anti message.

    @param cancelEvent The antimessage.
    @param object The object who receives the antimessage.
    */
    virtual bool handleAntiMessage(SimulationObject* object,
                                   const NegativeEvent* cancelEvent);

    /** Remove and return the next event in the event set.

    @return The removed event.
    */
    virtual const Event* getEvent(SimulationObject* object);

    /** Remove and return the next event in the event set.

    @return The removed event.
    */
    virtual const Event* getEvent(SimulationObject* object, const VTime& minimumTime);

    /** Return a reference to the next event in the event set.

    @return A reference to the next event in the event set.
    */
    virtual const Event* peekEvent(SimulationObject* object);

    /** Return a reference to the next event in the event set.

    @return A reference to the next event in the event set.
    */
    virtual const Event* peekEvent(SimulationObject* object, const VTime& minimumTime);

    /** Fossil collect the event set upto a given time.
        @param fossilCollectTime Time upto which to fossil collect.
        @return the number of events that was fossil collected.
    */
    virtual void fossilCollect(SimulationObject* object,
                               const VTime& fossilCollectTime);

    /** Fossil collect the event set upto a given time.
        @param fossilCollectTime Time upto which to fossil collect.
        @return the number of events that was fossil collected.
    */
    virtual void fossilCollect(SimulationObject* object,
                               int fossilCollectTime);

    /** Remove and delete an event from the event set.

    @param Event The event to be removed.
    */
    virtual void fossilCollect(const Event*);

    virtual void rollback(SimulationObject* object, const VTime& rollbackTime);

    /**
       Remove all events from the event set. Used to restore state after
       a catastrophic rollback while using optimistic fossil collection.
    */
    virtual void ofcPurge();

    /** Determines if the event is in the past based on the receive time,
        then sender and the event id.
        @return bool True if in the past.
    */
    virtual bool inThePast(const Event*);

    /// Print the event set.
    void print(std::ostream& out);

    /** Overriden from Configurable */
    void configure(SimulationConfiguration& configure) {}

    virtual double getEfficiency() {
        return static_cast<double>(myNumCommittedEvents) /
               myNumExecutedEvents;
    }

    virtual unsigned int getNumEventsExecuted() { return myNumExecutedEvents; }
    virtual unsigned int getNumEventsRolledBack() { return myNumRolledBackEvents; }
    virtual unsigned int getNumEventsCommitted() { return myNumCommittedEvents; }

    //@} // End of Public Class Methods of TimeWarpMultiSet.

protected:

    /**@name Protected Class Attributes of TimeWarpMultiSet. */
    //@{

    /// A sorted queue of the lowest event for each object. Used to determine which
    /// object and event to process next.
    multiset<const Event*, receiveTimeLessThanEventIdLessThan> lowestObjEvents;

    /// Each object has its own sorted queue of event contained here.
    vector<multiset<const Event*, receiveTimeLessThanEventIdLessThan>*> unprocessedObjEvents;

    /// The iterator pointing to an object's corresponding event in lowestObjEvents.
    vector<multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator> lowObjPos;

    /// The iterator to the position of the last insertion for each object.
    vector<multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator> insertObjPos;

    /// Queues to hold the processed Events for each object.
    vector<vector<const Event*>*> processedObjEvents;

    /// Queues to hold the removed events for each object.
    vector<vector<const Event*>*> removedEvents;

    /// The handle to the simulation manager.
    TimeWarpSimulationManager* mySimulationManager;

    unsigned int myNumCommittedEvents;
    unsigned int myNumRolledBackEvents;
    unsigned int myNumExecutedEvents;

    //@} // End of Protected Class Attributes of TimeWarpMultiSet.
};

#endif
