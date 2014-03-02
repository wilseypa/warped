#ifndef TIME_WARP_SENDER_QUEUE_H
#define TIME_WARP_SENDER_QUEUE_H

#include <functional>                   // for equal_to, binary_function
#include <list>                         // for list
#include <unordered_map>                // for unordered_map
#include <vector>                       // for vector

#include "DefaultObjectID.h"            // for OBJECT_ID
#include "Event.h"                      // for Event
#include "InputSetObject.h"
#include "ObjectID.h"                   // for hashObjectID, ObjectID
#include "SenderQueueContainer.h"
#include "TimeWarpCentralizedEventSet.h"
#include "VTime.h"                      // for VTime
#include "warped.h"                     // for findMode

class ScheduleListContainer;
class SenderQueueContainer;
class SimulationConfiguration;


using std::equal_to;
using std::binary_function;

class Event;
class SimulationObject;
class TimeWarpSimulationManager;

/** The TimeWarpSenderQueue concrete class.

    This is a concrete class implements Sender queues for TimeWarp
    Kernel. The insert operations are mostly and usually reduced to
    Sender operations instead of normal insert in this queue
    implementations.

*/
class TimeWarpSenderQueue : public TimeWarpCentralizedEventSet {
public:

    /**@name Public Class Methods of TimeWarpSenderQueue. */
    //@{

    /// Default Constructor
    TimeWarpSenderQueue(TimeWarpSimulationManager* initSimulationManager);

    /// Virtual Destructor
    virtual ~TimeWarpSenderQueue();

    /** Insert an event into the event set.

        @param Event The event to be inserted.
    */
    bool insert(Event* event, SimulationObject* object);

    /** Cancel out positive message corresponding to the anti message.

        @param cancelEvent The antimessage.
        @param object The object who receives the antimessage.
    */
    void handleAntiMessage(Event* cancelEvent, SimulationObject* object);

    /** Remove an event from the event set.

        @param Event The event to be removed.
        @param findMode Option for how to find the event.
    */
    virtual void remove(Event*, findMode, SimulationObject*);

    /** Remove and return the next event in the event set.

        @return The removed event.
    */
    virtual Event* getEvent(SimulationObject*);

    /** Return a reference to the next event in the event set.

        @return A reference to the next event in the event set.
    */
    virtual Event* peekEvent(SimulationObject*);

    /** Get a reference to some event in the event set.

        @param Event The event to find.
        @param findMode Option for how to find the event.
        @return A reference to the found event.
    */
    virtual Event* find(const VTime&, findMode, SimulationObject*);

    /** Fossil collect the event set upto a given time.

        @param VTime Time upto which to fossil collect.
    */
    virtual void fossilCollect(const VTime&, SimulationObject*);

    void configure(SimulationConfiguration&) {}

    //@} // End of Public Class Methods of TimeWarpSenderQueue.

protected:
    /**@name Protected Class Methods of TimeWarpSenderQueue. */
    //@{

    //map of SimulationObjectID and individual sender queues
    std::unordered_map<OBJECT_ID, SenderQueueContainer*, hashObjectID,
        equal_to<OBJECT_ID> >* senderQMap;

    //map of SimulationObjectID and individual processedQ per receiver
    std::unordered_map<OBJECT_ID, list<Event*>*, hashObjectID,
        equal_to<OBJECT_ID> >* processedQMap;

    list<Event*>* sortedUnProcessedQ;

    vector<ScheduleListContainer*>* scheduleList;

    bool isInThePast(const Event* event);

    //@} // End of Protected Class Methods of TimeWarpSenderQueue.

private:
    Event* peekedEvent;
    int noOfCommittedEvents;
    bool makeHeapFlag;
    bool popHeapFlag;
    TimeWarpSimulationManager* mySimulationManager;
};

typedef std::unordered_map<OBJECT_ID, SenderQueueContainer*, hashObjectID,
        equal_to<OBJECT_ID> > SENDERQMAP;

class SenderQElementLessThan : public binary_function<Event*, Event*, bool> {
public:
    bool operator()(const Event* const& lhs, const Event* const& rhs) {
        bool retval;
        if (lhs->getReceiveTime() < rhs->getReceiveTime()) {
            retval = true;
        } else if (lhs->getReceiveTime() == rhs->getReceiveTime()) {
            retval = lhs->getReceiver() < rhs->getReceiver();
        } else {
            retval = false;
        }
        return retval;
    }
};
/*
class equalTo_EventSetIterators {
public:
  bool operator()(const multiset<SetObject<Event> >::iterator& lhs,
          const multiset<SetObject<Event> >::iterator& rhs) const {
    return (*lhs == *rhs);
  }
};

class lessThan_EventSetIterators {
public:
  bool operator()(const multiset<SetObject<Event> >::iterator& lhs,
          const multiset<SetObject<Event> >::iterator& rhs) const {
    return (*lhs < *rhs);
  }
};
*/
#endif
