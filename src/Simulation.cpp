
#include "Simulation.h"
#include "SequentialConfigurationManager.h"
#include "TimeWarpConfigurationManager.h"
#include "SimulationConfiguration.h"
#include "Application.h"
#include "WarpedDebug.h"

#include <iostream>
#include <stdexcept>

Simulation::Simulation(Application* initApplication) : myApplication(initApplication) {
}

Simulation*
Simulation::instance(SimulationConfiguration* configuration,
                     Application* userApplication) {
    // global access point to this singleton class
    static Simulation* singleton = 0;
    if (singleton == 0) {
        singleton = new Simulation(userApplication);
        if (configuration == 0) {
            throw std::runtime_error("Must provide a non-null configuration if userApplication is not null");
        }
        singleton->configure(*configuration);
    }
    return singleton;
}

void
Simulation::configure(SimulationConfiguration& configuration) {
    // Decide which Simulation manager to use based on the configuraiton

    std::string simulationType = configuration.get_string({"Simulation"}, "Sequential");

    if (simulationType == "Sequential") {
        myConfigurationManager = new SequentialConfigurationManager(myApplication);
        debug::debugout << "Configured a SequentialSimulationManager" << std::endl;
    } else if (simulationType == "TimeWarp"
               || simulationType == "ThreadedTimeWarp") {
        myConfigurationManager = new TimeWarpConfigurationManager(myApplication);
        debug::debugout << "Configured a TimeWarpSimulationManager" << std::endl;
    } else {
        std::cerr << "Unknown Simulation type \"" << simulationType << "\"" << std::endl;
        exit(-1);
    }

    myConfigurationManager->configure(configuration);
}

SimulationManager*
Simulation::getSimulationManager() {
    ASSERT(myConfigurationManager != 0);
    return myConfigurationManager->getSimulationManager();
}


// report an error condition depending on the severity level
void
Simulation::reportError(const string& msg, const SEVERITY level) {
    switch (level) {
    case NOTE:
        std::cout << "Severity Level: NOTE" << std::endl;
        std::cout << msg << std::endl;
        break;
    case WARNING:
        std::cout << "Severity Level: WARNING" << std::endl;
        std::cout << msg << std::endl;
        break;
    case ERROR:
        std::cout << "Severity Level: ERROR" << std::endl;
        std::cout << msg << std::endl;
        exit(-1);
        break;
    case ABORT:
        std::cout << "Severity Level: ABORT" << std::endl;
        std::cout << msg << std::endl;
        abort();
        break;
    default:
        break;
    };
}

void
Simulation::initialize() {
    getSimulationManager()->initialize();
}

void
Simulation::simulate(const VTime& simulateUntil) {
    getSimulationManager()->simulate(simulateUntil);
}

void
Simulation::finalize() {
    getSimulationManager()->finalize();
    cleanUp();
}

const VTime&
Simulation::getCommittedTime() {
    return getSimulationManager()->getCommittedTime();
}

const VTime&
Simulation::getNextEventTime() {
    return getSimulationManager()->getNextEventTime();
}

bool
Simulation::simulationComplete() {
    return getSimulationManager()->simulationComplete();
}

extern "C" {
    /**
       Simply here to have something to search for from configure scripts.
    */
    char isWarped() { return 0; }
}
