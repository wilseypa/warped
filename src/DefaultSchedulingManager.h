#ifndef DEFAULT_SCHEDULING_MANAGER_H
#define DEFAULT_SCHEDULING_MANAGER_H


#include "SchedulingManager.h"          // for SchedulingManager
#include "warped.h"

class Event;
class SimulationConfiguration;
class SimulationObject;
class TimeWarpSimulationManager;
class VTime;

/** The DefaultSchedulingManager */
class DefaultSchedulingManager :  public SchedulingManager {
public:
    DefaultSchedulingManager(TimeWarpSimulationManager* initSimulationManager);
    ~DefaultSchedulingManager();

    const Event* peekNextEvent();

    const VTime& getLastEventScheduledTime();

    void configure(SimulationConfiguration&) {}

private:
    void setLastScheduledEventTime(const VTime& newTime);

    TimeWarpSimulationManager* mySimulationManager;
    const VTime* lastScheduledTime;
};

#endif
