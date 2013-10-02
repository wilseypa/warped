#include "DefaultTimeWarpEventSet.h"
#include "TimeWarpEventSetFactory.h"

#include <iostream>

#include "TimeWarpMultiSet.h"
#include "TimeWarpMultiSetOneAntiMsg.h"
#include "TimeWarpSimulationManager.h"
#include "TimeWarpSenderQueue.h"
#include "SimulationConfiguration.h"
#include "SchedulingData.h"

#include "ThreadedTimeWarpMultiSet.h"
#include "ThreadedTimeWarpSimulationManager.h"


Configurable*
TimeWarpEventSetFactory::allocate(SimulationConfiguration& configuration,
                                  Configurable* parent) const {
    TimeWarpEventSet* retval = 0;
    TimeWarpSimulationManager* mySimulationManager =
        dynamic_cast<TimeWarpSimulationManager*>(parent);

    ASSERT(mySimulationManager != 0);

    std::string antiMessages = configuration.get_string({"TimeWarp", "OutputManager", "AntiMessages"},
                                                        "Default");
    std::string eventListType = configuration.get_string({"EventList", "Type"}, "Default");
    std::string simulationType = configuration.get_string({"Simulation"}, "Sequential");

    bool usingOneAntiMessageOpt = false;
    if (antiMessages == "One") {
        usingOneAntiMessageOpt = true;
    }

    if (simulationType == "ThreadedTimeWarp") {
        eventListType = configuration.get_string({"TimeWarp", "EventList", "Type"}, "Default");
        if (eventListType == "MultiSet") {
            ThreadedTimeWarpEventSet* retvalue = 0;
            retvalue = new ThreadedTimeWarpMultiSet(
                dynamic_cast<ThreadedTimeWarpSimulationManager*>(parent));
            debug::debugout << "(" << mySimulationManager->getSimulationManagerID()
                            << ") configured an Dynamic Threaded MultiSet as the event set"
                            << std::endl;
            return retvalue;
        } else {
            mySimulationManager->shutdown("Event list type \"" + eventListType
                                          + "\" is not a valid choice.");
        }
    }

    if (eventListType == "Default") {
        retval = new DefaultTimeWarpEventSet(mySimulationManager,
                                             usingOneAntiMessageOpt);
        debug::debugout << "(" << mySimulationManager->getSimulationManagerID()
                        << ") configured a DefaultTimeWarpEventSet as the event set"
                        << std::endl;
    } else if (eventListType == "MultiSet") {
        if (usingOneAntiMessageOpt) {
            retval = new TimeWarpMultiSetOneAntiMsg(mySimulationManager);
            debug::debugout << "("
                            << mySimulationManager->getSimulationManagerID()
                            << ") configured a TimeWarpMultiSetOneAntiMsg as the event set"
                            << std::endl;
        } else {
            retval = new TimeWarpMultiSet(mySimulationManager);
            debug::debugout << "("
                            << mySimulationManager->getSimulationManagerID()
                            << ") configured a TimeWarpMultiSet as the event set"
                            << std::endl;
        }
    } else {
        mySimulationManager->shutdown("Event list type \"" + eventListType
                                      + "\" is not a valid choice.");
    }

    return retval;
}

const TimeWarpEventSetFactory*
TimeWarpEventSetFactory::instance() {
    static const TimeWarpEventSetFactory* retval =
        new TimeWarpEventSetFactory();
    return retval;
}
