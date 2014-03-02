
#include <stddef.h>                     // for NULL
#include <ostream>                      // for operator<<, basic_ostream, etc
#include <string>                       // for allocator, operator==, etc

#include "ChebyFossilCollManager.h"     // for ChebyFossilCollManager
#include "Configurable.h"               // for Configurable
#include "OptFossilCollManagerFactory.h"
#include "SetObject.h"                  // for ostream
#include "SimulationConfiguration.h"    // for SimulationConfiguration
#include "ThreadedChebyFossilCollManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "TimeWarpSimulationManager.h"  // for TimeWarpSimulationManager
#include "WarpedDebug.h"                // for debugout
#include "warped.h"                     // for ASSERT
using std::string;

OptFossilCollManagerFactory::OptFossilCollManagerFactory() {}

OptFossilCollManagerFactory::~OptFossilCollManagerFactory() {
    // myFossilManager will be deleted by the end user - the
    // TimeWarpSimulationManager
}

Configurable*
OptFossilCollManagerFactory::allocate(SimulationConfiguration& configuration,
                                      Configurable* parent) const {
    Configurable* retval = NULL;
    TimeWarpSimulationManager* mySimulationManager =
        dynamic_cast<TimeWarpSimulationManager*>(parent);
    ThreadedTimeWarpSimulationManager* myThreadedSimulationManager =
        dynamic_cast<ThreadedTimeWarpSimulationManager*>(parent);

    ASSERT(mySimulationManager != NULL);
    // the following cases are possible:

    // (1) Manager is Cheby.
    // (2) None is used.

    std::string optFossilCollManagerType = configuration.get_string(
    {"TimeWarp", "OptFossilCollManager", "Type"}, "None");
    std::string simulationType = configuration.get_string({"Simulation"}, "Sequential");
    if (optFossilCollManagerType == "Cheby") {
        int checkpointPeriod = configuration.get_int({"TimeWarp", "OptFossilCollManager", "CheckpointTime"},
                                                     1000);
        int minSamples = configuration.get_int({"TimeWarp", "OptFossilCollManager", "MinimumSamples"},
                                               64);
        int maxSamples = configuration.get_int({"TimeWarp", "OptFossilCollManager", "MaximumSamples"},
                                               100);
        int defaultLength = configuration.get_int({"TimeWarp", "OptFossilCollManager", "DefaultLength"},
                                                  2000);
        double riskFactor = configuration.get_double({"TimeWarp", "OptFossilCollManager", "AcceptableRisk"},
                                                     0.99);

        if (simulationType == "ThreadedTimeWarp") {
            retval = new ThreadedChebyFossilCollManager(myThreadedSimulationManager,
                                                        checkpointPeriod, minSamples, maxSamples, defaultLength,
                                                        riskFactor);
            myThreadedSimulationManager->setOptFossilColl(true);

        } else {
            retval = new ChebyFossilCollManager(mySimulationManager,
                                                checkpointPeriod, minSamples, maxSamples, defaultLength,
                                                riskFactor);
            mySimulationManager->setOptFossilColl(true);
            debug::debugout << "("
                            << mySimulationManager->getSimulationManagerID()
                            << ") configured a Cheby Optimistic Fossil Collection Manager "
                            << "with checkpoint interval: " << checkpointPeriod
                            << ", and risk factor: " << riskFactor << endl;
        }
    } else if (optFossilCollManagerType == "None") {
        retval = NULL;
    } else {
        mySimulationManager->shutdown(
            "Unknown FossilManager choice \"" + optFossilCollManagerType + "\"");
    }

    return retval;
}

const OptFossilCollManagerFactory*
OptFossilCollManagerFactory::instance() {
    static OptFossilCollManagerFactory* singleton = new OptFossilCollManagerFactory();

    return singleton;
}
