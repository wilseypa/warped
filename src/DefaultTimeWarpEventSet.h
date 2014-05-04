#ifndef DEFAULT_TIME_WARP_EVENT_SET_H
#define DEFAULT_TIME_WARP_EVENT_SET_H


#include <deque>                        // for deque
#include <iosfwd>                       // for ostream
#include <string>                       // for string
#include <vector>                       // for vector

#include "DefaultObjectID.h"            // for OBJECT_ID
#include "DefaultTimeWarpEventContainer.h"
#include "TimeWarpEventSet.h"           // for string, TimeWarpEventSet
#include "warped.h"

class Event;
class NegativeEvent;
class VTime;

using std::deque;
using std::string;
using std::vector;

class DefaultTimeWarpEventContainer;
class SimulationConfiguration;
class SimulationObject;
class TimeWarpEventSetFactory;
class TimeWarpSimulationManager;

/** The default implementation of TimeWarpEventSet.  This implementation
    works in the following manner.  Events are kept in two groups,
    unprocessed and processed.  Processed events are not explicitly sorted
    (although they maybe be by virtue of the order of their insertion),
    unprocessed events are sorted on demand.  That is, they are inserted in
    arbitrary order but when events are requested they are sorted if
    needed.
*/
class DefaultTimeWarpEventSet : public TimeWarpEventSet {
public:
    DefaultTimeWarpEventSet(TimeWarpSimulationManager* initSimManager,
                            bool usingOneAntiMsg);

    ~DefaultTimeWarpEventSet();

    bool insert(const Event* event);

    bool handleAntiMessage(SimulationObject* eventFor,
                           const NegativeEvent* cancelEvent);

    const Event* getEvent(SimulationObject*);

    const Event* getEvent(SimulationObject*, const VTime&);

    const Event* peekEvent(SimulationObject*);

    const Event* peekEvent(SimulationObject*, const VTime&);

    void fossilCollect(SimulationObject*, const VTime&);

    void fossilCollect(SimulationObject*, int);

    void fossilCollect(const Event*);

    void configure(SimulationConfiguration&) {}

    void rollback(SimulationObject*, const VTime&);

    void ofcPurge();

    void debugDump(const string& objectName, std::ostream& os);

private:
    DefaultTimeWarpEventContainer& getEventContainer(const OBJECT_ID* objectID);

    vector<DefaultTimeWarpEventContainer*> events;

    TimeWarpSimulationManager* mySimulationManager;
};

#endif
