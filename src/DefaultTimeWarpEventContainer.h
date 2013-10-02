#ifndef DEFAULTTIMEWARPEVENTCONTAINER_H_
#define DEFAULTTIMEWARPEVENTCONTAINER_H_


#include "EventFunctors.h"
#include <deque>
#include <vector>
#include <iostream>

using std::deque;
using std::vector;
using std::endl;

class SimulationObject;
class NegativeEvent;

/**
   This class manages the set of events for a single SimulationObject.
   It is the heart of the TimeWarpEventSet implementation, but it
   is invisible to the interface so it is a private part of that class
   and not contained elsewhere.
*/
class DefaultTimeWarpEventContainer {
public:
    DefaultTimeWarpEventContainer() : sortStatus(SORTED) {}
    ~DefaultTimeWarpEventContainer() {}

    /**
       Adds the event to the unprocessed queue.

       @param Event The event to insert.
    */
    void insert(const Event* newEvent);

    /**
       Sorts the unprocessed queue.
    */
    void sortByReceiveTimes() {
        std::sort(unprocessedEvents.begin(),
                  unprocessedEvents.end(),
                  receiveTimeLessThanEventIdLessThan());
        sortStatus = SORTED;
    }

    /**
       Removes the event from the event set that corresponds to the
       negative event and deletes the event.

       @param NegativeEvent The event to cancel.
       @return bool True if the event was removed.
    */
    virtual bool handleAntiMessage(SimulationObject* reclaimer,
                                   const NegativeEvent* eventToRemove);

    /**
       Return the next event (with respect to receive time), leaves it in
       the set.
    */
    const Event* nextEvent();

    /**
       Return the next event (with respect to receive time), and removes it
       in the set.

       @return Event The next event to process.
    */
    const Event* getNextEvent();

    /**
       Puts all processed events with a receive time greater than or equal
       to the given time in the unprocessed queue.

       @param VTime The rollback time.
    */
    void rollback(const VTime&);

    /**
       For debugging purposes.
    */
    void debugDump(std::ostream&) const;

    /**
       Clears the processedEvents vector of events at or before
       collectTime.
       @param object The object that will be used to reclaim the events.
       @param collectTime Events in the processed queue at or before this
       time will be reclaimed.
    */
    void fossilCollect(SimulationObject* object, const VTime& collectTime);

    /**
       Clears the processedEvents vector of events at or before
       collectTime.
       @param object The object that will be used to reclaim the events.
       @param collectTime Events in the processed queue at or before this
       time will be reclaimed.
    */
    void fossilCollect(SimulationObject* object, int collectTime);

    /**
       Delete a single event from the output queue.

       @param toRemove The event to remove.
    */
    void fossilCollect(const Event* toRemove);

    /**
       Remove all events from the event set. Used to restore state after
       a catastrophic rollback while using optimistic fossil collection.
       @param object The simulation object to purge events for.
    */
    void ofcPurge(SimulationObject* object);

    /**
       Determines if an event is truly in the past based on the receive time,
       the event sender and the event id.

       @param Event The event to determine future or past.
       @return bool True if it is in the past.
    */
    bool isInThePast(const Event*);

protected:
    deque<const Event*> unprocessedEvents;
    vector<const Event*> processedEvents;
    vector<const Event*> removedEvents;
    deque<const NegativeEvent*> negativeEvents;
    enum SortStatus { SORTED, UNSORTED } sortStatus;
};

#endif /* DEFAULTTIMEWARPEVENTCONTAINER_H_ */
