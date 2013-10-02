#ifndef CENTRALIZED_EVENTSET_SCHEDULING_MANAGER_H
#define CENTRALIZED_EVENTSET_SCHEDULING_MANAGER_H


#include "SchedulingManager.h"

class SimulationManager;
class TimeWarpCentralizedEventSet;
class SimulationObject;

/** The SchedulingManager abstract base class.

    This is the abstract base class for all centralized eventset
    scheduler implementations in the kernel. Different scheduler
    implementations must be derived from this abstract class.

*/
class CentralizedEventSetSchedulingManager : public SchedulingManager {
public:
    /**@name Public Class Methods of SchedulingManager. */
    //@{

    /// Virtual Destructor.
    virtual ~CentralizedEventSetSchedulingManager();

    /** Initialize the scheduler.

        @param eventSet Set of events that constitute the input-queue.
        @param simObjArray Vector of simulation objects.
    */
    virtual void initialize(TimeWarpEventSet* eventSet,
                            vector<SimulationObject*>* simObjArray);

    //@} // End of Public Class Methods of SchedulingManager.
protected:

    /**@name Protected Class Methods of SchedulingManager. */
    //@{

    /// Default Constructor.
    CentralizedEventSetSchedulingManager(SimulationManager* simMgr);

    /// Handle to my simulation manager.
    SimulationManager* mySimulationManager;

    /// Handle to the input queue.
    TimeWarpCentralizedEventSet* myEventSetHandle;

    /// The scheduler's list of objects.
    vector<SimulationObject*>* simulationObjectsArray;

    /// the timestamp of the last event that was scheduled.
    const VTime* lastScheduledEventTime;

    //@} // End Protected Class Attributes of SchedulingManagerImplementationBase.

};

#endif //ifdef CENTRALIZED_EVENTSET_SCHEDULING_MANAGER_H
