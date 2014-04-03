
#include <iostream>                     // for operator<<, cout, ostream
#include <stdexcept>                    // for runtime_error
#include <string>                       // for operator==, basic_string, etc

#include "Application.h"                // for Application
#include "CommunicationManagerFactory.h"
#include "Configurable.h"               // for Configurable
#include "Configurer.h"                 // for string
#include "GVTManagerFactory.h"          // for GVTManagerFactory
#include "OutputManagerFactory.h"       // for OutputManagerFactory
#include "SchedulingManagerFactory.h"   // for SchedulingManagerFactory
#include "SimulationConfiguration.h"    // for SimulationConfiguration
#include "Spinner.h"                    // for spinForever
#include "StateManagerFactory.h"        // for StateManagerFactory
#include "ThreadedTimeWarpSimulationManager.h"
#include "TimeWarpConfigurationManager.h"
#include "TimeWarpEventSetFactory.h"    // for TimeWarpEventSetFactory
#include "TimeWarpSimulationManager.h"  // for TimeWarpSimulationManager
#include "TimeWarpSimulationManagerFactory.h"

using std::string;

class SimulationManager;

TimeWarpConfigurationManager::TimeWarpConfigurationManager(Application* initApplication) :
    mySimulationManager(0), myEventSetFactory(0), myStateFactory(0), myGVTFactory(0),
    myCommunicationFactory(0), mySchedulingFactory(0), myOutputFactory(0),
    myApplication(initApplication) {}

TimeWarpConfigurationManager::~TimeWarpConfigurationManager() {
    delete myEventSetFactory;
    delete myStateFactory;
    delete myGVTFactory;
    delete myCommunicationFactory;
    delete mySchedulingFactory;
    delete myOutputFactory;
    delete mySimulationManager;
    // the handles to the eventSetManager, stateManager, GVTManager,
    // CommunicationManager, schedulingManager and the outputManager
    // are deleted by the end user - the TimeWarpSimulationManager.
}

void TimeWarpConfigurationManager::configure(
    SimulationConfiguration& configuration) {
    std::cout << "Initializing simulation...\n";

    const TimeWarpSimulationManagerFactory* twSimMgrFactory =
        TimeWarpSimulationManagerFactory::instance();
    std::string simulationType = configuration.get_string({"Simulation"}, "Sequential");

    if (simulationType == "TimeWarp") {
        mySimulationManager = dynamic_cast<TimeWarpSimulationManager*>
                              (twSimMgrFactory->allocate(configuration, myApplication));
    } else if (simulationType == "ThreadedTimeWarp") {
        mySimulationManager = dynamic_cast<ThreadedTimeWarpSimulationManager*>
                              (twSimMgrFactory->allocate(configuration, myApplication));
    } else {
        throw std::runtime_error("SimType configuration is invalid.");
    }

    mySimulationManager->configure(configuration);
    if (configuration.get_bool({"ParallelDebug", "SpinBeforeSimulationStart"}, false)) {
        Spinner::spinForever();
    }
}

SimulationManager*
TimeWarpConfigurationManager::getSimulationManager() {
    return mySimulationManager;
}
