#ifndef PERIODIC_STATE_MANAGER_H
#define PERIODIC_STATE_MANAGER_H


#include "StateManagerImplementationBase.h"

class SimulationConfiguration;
class SimulationObject;
class TimeWarpSimulationManager;
class VTime;

/** The PeriodicStateManager class.

    The PeriodicStateManager class is an implementation of the
    abstract StateManager class. It implements a state manager that
    saves states Periodicly depending on a state period that is an
    input parameter. By default, this state period is one (i.e. save
    state for every event execution).

*/
class PeriodicStateManager : public StateManagerImplementationBase {
public:

    /**@name Public Class Methods of PeriodicStateManager. */
    //@{

    /** Constructor.

        @param numObjects Number of simulation objects.
        @param period State saving period (default = 0).
    */
    PeriodicStateManager(TimeWarpSimulationManager* simMgr,
                         unsigned int period = 0);

    /// Destructor
    ~PeriodicStateManager();

    /** Rollback and restore the state of the object at the specified time.

        @param rollbackTime Rollback to this time and restore state.
        @param SimulationObject The object whose state should be restored.
        @return the time of restored state
     */
    const VTime& restoreState(const VTime& rollbackTime, SimulationObject* object);

    /** Save the state of the object at the specified time.

        @param currentTime Current time of state saving.
        @param object Simulation object for which to save the state.
    */
    void saveState(const VTime& currentTime, SimulationObject* object);

    void configure(SimulationConfiguration&) {}

    //@} // End of Public Class Methods of PeriodicStateManager.

};

#endif
