#ifndef SCHEDULING_MANAGER_FACTORY_H
#define SCHEDULING_MANAGER_FACTORY_H


#include "Configurer.h"                 // for Configurer
#include "warped.h"

class Configurable;
class Event;
class SchedulingManager;
class SimulationConfiguration;
class SimulationObject;
class TimeWarpEventSetFactory;
class TimeWarpSimulationManager;

/** The SchedulingManagerFactory class.

This is the class interface for the various scheduler
implementations that are possible in the Simulation kernel. The
specific implementation is hidden from the other
components of the simulation kernel by the aggregation of the
scheduler implemetations in this class interface ( factory
pattern).  All scheduler implementations must be derived from the
SchedulingManager abstract base class.

*/
class SchedulingManagerFactory : virtual public Configurer {
public:

    /**@name Public Class Methods of SchedulingManagerFactory. */
    //@{

    /** Constructor.

    @param simMgr Handle to the simulation manager.
    */
    SchedulingManagerFactory();

    /// Destructor.
    virtual ~SchedulingManagerFactory();

    /** Configure the scheduler.

    @param configurationMap Set of configuration options.
    */
    Configurable* allocate(SimulationConfiguration& configuration,
                           Configurable* parent) const;

    static const SchedulingManagerFactory* instance();

    //@} // End of SchedulingManagerFactory Public Class Methods

protected:

    /**@name Protected Class Attributes of SchedulingManagerFactory. */
    //@{

    //@} // End of Protected Class Attributes of SchedulingManagerFactory
};

#endif

