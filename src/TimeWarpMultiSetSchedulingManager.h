#ifndef TIME_WARP_MULTISET_SCHEDULING_MANAGER_H
#define TIME_WARP_MULTISET_SCHEDULING_MANAGER_H


#include "warped.h"
#include "SchedulingManager.h"

class SimulationObject;
class TimeWarpSimulationManager;

/** This scheduling manager is to be used with the TimeWarpMultiSet. */
class TimeWarpMultiSetSchedulingManager :  public SchedulingManager {
public:
    TimeWarpMultiSetSchedulingManager(TimeWarpSimulationManager* initSimulationManager);
    ~TimeWarpMultiSetSchedulingManager();

    const Event* peekNextEvent();

    const VTime& getLastEventScheduledTime();

    void configure(SimulationConfiguration&) {}

private:
    void setLastScheduledEventTime(const VTime& newTime);

    TimeWarpSimulationManager* mySimulationManager;
    const VTime* lastScheduledTime;
};

#endif
