#ifndef STATE_MANAGER_IMPLEMENTATION_BASE_H
#define STATE_MANAGER_IMPLEMENTATION_BASE_H


#include <iosfwd>                       // for ostream
#include <set>                          // for multiset
#include <vector>                       // for std::vector

#include "SetObject.h"                  // for SetObject (ptr only), etc
#include "StateManager.h"               // for StateManager
#include "TimeWarpSimulationManager.h"
#include "warped.h"

class SimulationObject;
class State;
class TimeWarpSimulationManager;
class VTime;

using std::multiset;

/** The StateManagerImplementationBase class.

    The StateManagerImplementationBase class is an implementation of
    the abstract StateManager class. It contains the methods and class
    attributes common to the PeriodicStateManager and the
    AdaptiveStateManager.

*/
class StateManagerImplementationBase : public StateManager {
public:

    friend class OptFossilCollManager;

    /**@name Public Class Methods of StateManagerImplementationBase. */
    //@{

    /** Constructor.

        @param simMgr a handle to the simulation manager
        @param period State-saving period.
    */
    StateManagerImplementationBase(TimeWarpSimulationManager* simMgr,
                                   unsigned int period = 0);

    /// Default Destructor.
    virtual ~StateManagerImplementationBase();

    /** Save the state of the object at the specified time.

        @param currentTime Time at which the state is saved.
        @param object The object whose state is saved.
    */
    virtual void saveState(const VTime& currentTime, SimulationObject* object);

    /// return the state saving period
    unsigned int getStatePeriod();

    /** Returns the array of object state periods.
    */
    std::vector<unsigned int> getObjectStatePeriod();

    /** Rollback and restore the state of the object at the specified time.

        @param rollbackTime Rollback to this time and restore state.
        @param SimulationObject The object whose state should be restored.
        @return the time of restored state
    */
    virtual const VTime& restoreState(const VTime& rollbackTime,
                                      SimulationObject* object);

    /** Fossil collect up to the specified simulation time.

        @param fossilCollectTime Fossil collect upto this time.
        @param object Object whose states are gbg collected.
        @return lowest time-stamped state in state queue
    */
    virtual const VTime& fossilCollect(SimulationObject* object, const VTime& fossilCollectTime);

    /** Fossil collect up to the specified simulation time.

        @param fossilCollectTime Fossil collect upto this time.
        @param object Object whose states are gbg collected.
        @return lowest time-stamped state in state queue
    */
    virtual const VTime& fossilCollect(SimulationObject* object, int fossilCollectTime);

    /** Print state queue.

        @param currentTime Print statequeue upto current time.
        @param object Object whose statequeue is printed.
    */
    virtual void printStateQueue(const VTime& currentTime,
                                 SimulationObject* object, std::ostream& out);

    /**
       Remove all states from the state queues. Used to restore state after
       a catastrophic rollback while using optimistic fossil collection.
    */
    virtual void ofcPurge();

    /**
       Remove all states for the specified object. Used to restore state after
       a catastrophic rollback while using optimistic fossil collection.
    */
    virtual void ofcPurge(unsigned int objId);

    //@} // End of Public Class Methods of StateManagerImplementationBase.

protected:

    /**@name Protected Class Attributes of StateManagerImplementationBase. */
    //@{

    /// handle to the simulation manager
    TimeWarpSimulationManager* mySimulationManager;

    /// The state period - if infrequent, then statePeriod != 0.
    unsigned int statePeriod;

    /// The state saving period of each object
    std::vector<unsigned int> objectStatePeriod;

    /// Time since the last saved state.
    std::vector<int> periodCounter;

    /// A state queue for every object on this simulation manager.
    multiset< SetObject<State> >* myStateQueue;

    //@} // End of Protected Class Attributes of StateManagerImplementationBase.
};

#endif






