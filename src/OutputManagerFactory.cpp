
#include <iostream>                     // for operator<<, endl, etc
#include <string>                       // for allocator, operator==, etc

#include "AdaptiveOutputManager.h"      // for DynamicOutputManager
#include "AggressiveOutputManager.h"    // for AggressiveOutputManager
#include "Configurable.h"               // for Configurable
#include "LazyOutputManager.h"          // for LazyOutputManager
#include "OutputManagerFactory.h"
#include "SimulationConfiguration.h"    // for SimulationConfiguration
#include "ThreadedAggressiveOutputManager.h"
#include "ThreadedDynamicOutputManager.h"
#include "ThreadedLazyOutputManager.h"  // for ThreadedLazyOutputManager
#include "ThreadedTimeWarpSimulationManager.h"
#include "TimeWarpSimulationManager.h"  // for TimeWarpSimulationManager
#include "WarpedDebug.h"                // for debugout
#include "warped.h"                     // for OutputMgrType::ADAPTIVEMGR, etc

using std::cerr;
using std::endl;
using std::string;

OutputManagerFactory::OutputManagerFactory() {
}

OutputManagerFactory::~OutputManagerFactory() {
}

// configure this  factory to instantiate the chosen output manager
Configurable*
OutputManagerFactory::allocate(SimulationConfiguration& configuration,
                               Configurable* parent) const {

    Configurable* retval = 0;
    TimeWarpSimulationManager* mySimulationManager =
        dynamic_cast<TimeWarpSimulationManager*>(parent);
    ASSERT(mySimulationManager != 0);

    std::string antiMessages = configuration.get_string({"TimeWarp", "OutputManager", "AntiMessages"},
                                                        "Default");
    std::string simulationType = configuration.get_string({"Simulation"}, "Sequential");
    std::string outputManagerType = configuration.get_string({"TimeWarp", "OutputManager", "Type"},
                                                             "Aggressive");

    if (antiMessages == "One") {
        mySimulationManager->setOneAntiMsg(true);
    }

    // the following cases are possible:
    // (1) AggressiveOutputManager
    // (2) LazyOutputManager
    // (3) DynamicOutputManager
    if (outputManagerType == "Aggressive") {
        retval = new AggressiveOutputManager(mySimulationManager);
        mySimulationManager->setOutputMgrType(AGGRMGR);
        debug::debugout << "an Aggressive Output Manager" << endl;
    } else if (outputManagerType == "Lazy") {
        retval = new LazyOutputManager(mySimulationManager);
        mySimulationManager->setOutputMgrType(LAZYMGR);
        debug::debugout << "a Lazy Output Manager" << endl;
    } else if (outputManagerType == "Dynamic") {
        retval = new DynamicOutputManager(mySimulationManager);
        mySimulationManager->setOutputMgrType(ADAPTIVEMGR);
        debug::debugout << "an Dynamic Output Manager" << endl;
    } else {
        const string error = "Unknown OutputManager choice \""
                             + outputManagerType + "\" encountered.";
        mySimulationManager->shutdown(error);
    }

    if (simulationType == "ThreadedTimeWarp") {
        if (outputManagerType == "Aggressive") {
            retval = new ThreadedAggressiveOutputManager(
                dynamic_cast<ThreadedTimeWarpSimulationManager*>(parent));
            mySimulationManager->setOutputMgrType(AGGRMGR);
            debug::debugout << "a Dynamic Threaded Aggressive Output Manager"
                            << endl;
        } else if (outputManagerType == "Lazy") {
            retval = new ThreadedLazyOutputManager(
                dynamic_cast<ThreadedTimeWarpSimulationManager*>(parent));
            mySimulationManager->setOutputMgrType(LAZYMGR);
            debug::debugout << "a Lazy Output Manager" << endl;
        } else if (outputManagerType == "Dynamic") {
            int filterDepth = configuration.get_int({"TimeWarp", "OutputManager", "FilterDepth"}, 16);
            double aggr2lazy = configuration.get_double({"TimeWarp", "OutputManager", "AggrToLazyRatio"}, 0.5);
            double lazy2aggr = configuration.get_double({"TimeWarp", "OutputManager", "LazyToAggrRatio"}, 0.2);
            double thirdThreshold = configuration.get_double({"TimeWarp", "OutputManager", "ThirdThreshold"},
                                                             0.1);

            retval = new ThreadedDynamicOutputManager(
                dynamic_cast<ThreadedTimeWarpSimulationManager*>(parent),
                filterDepth, aggr2lazy, lazy2aggr, thirdThreshold);
            mySimulationManager->setOutputMgrType(ADAPTIVEMGR);
            debug::debugout << "an Dynamic Output Manager" << endl;
        } else {
            const string error = "Unknown OutputManager choice \""
                                 + outputManagerType + "\" encountered.";
            mySimulationManager->shutdown(error);
        }
    }
    return retval;
}

const OutputManagerFactory*
OutputManagerFactory::instance() {
    static OutputManagerFactory* singleton = new OutputManagerFactory();
    return singleton;
}
