
#include <stddef.h>                     // for NULL

#include "Event.h"                      // for Event
#include "TimeWarpEventSet.h"           // for TimeWarpEventSet
#include "TimeWarpMultiSetSchedulingManager.h"
#include "TimeWarpSimulationManager.h"  // for TimeWarpSimulationManager
#include "VTime.h"                      // for VTime
#include "warped.h"                     // for ASSERT

TimeWarpMultiSetSchedulingManager::TimeWarpMultiSetSchedulingManager(TimeWarpSimulationManager*
                                                                     initSimulationManager)
    : mySimulationManager(initSimulationManager),
      lastScheduledTime(initSimulationManager->getZero().clone()) {}

TimeWarpMultiSetSchedulingManager::~TimeWarpMultiSetSchedulingManager() {}

const Event*
TimeWarpMultiSetSchedulingManager::peekNextEvent() {
    TimeWarpEventSet* eventSet = mySimulationManager->getEventSetManager();
    ASSERT(eventSet != 0);

    const Event* earliestEvent = eventSet->peekEvent(NULL);

    return earliestEvent;
}

const VTime&
TimeWarpMultiSetSchedulingManager::getLastEventScheduledTime() {
    const Event* earliestEvent = peekNextEvent();
    if (earliestEvent != 0) {
        lastScheduledTime = &earliestEvent->getReceiveTime();
    } else {
        lastScheduledTime = &mySimulationManager->getPositiveInfinity();
    }
    ASSERT(lastScheduledTime != 0);
    return *lastScheduledTime;
}
