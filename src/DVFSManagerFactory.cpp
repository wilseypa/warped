
#include "WarpedDebug.h"
#include "DVFSManagerFactory.h"
#include "SharedDVFSManager.h"
#include "DistributedDVFSManager.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"

#include <string>

using std::cout;
using std::cerr;
using std::endl;

DVFSManagerFactory::DVFSManagerFactory() {}

DVFSManagerFactory::~DVFSManagerFactory() {}

// allocate a new manager of the type given in the configuration file
Configurable*
DVFSManagerFactory::allocate(SimulationConfiguration& configuration,
                             Configurable* parent) const {

    TimeWarpSimulationManager* mySimulationManager =
        dynamic_cast<TimeWarpSimulationManager*>(parent);
    ASSERT(mySimulationManager);

    std::string dvfsManagerType = configuration.get_string({"TimeWarp", "DVFSManager", "Type"},
                                                           "None");
    if (dvfsManagerType == "None") {
        return NULL;
    }

    std::string metric = configuration.get_string({"TimeWarp", "DVFSManager", "UsefulWorkMetric"},
                                                  "Rollbacks");
    std::string al = configuration.get_string({"TimeWarp", "DVFSManager", "Algorithm"},
                                              "Fixed");
    bool dvfsDebugPrint = configuration.get_bool({"TimeWarp", "DVFSManager", "DebugPrint"}, false);
    int p = configuration.get_int({"TimeWarp", "DVFSManager", "Period"}, 1);
    int firsize = configuration.get_int({"TimeWarp", "DVFSManager", "FIRSize"}, 16);
    double threshold = configuration.get_double({"TimeWarp", "DVFSManager", "Threshold"}, 0.1);

    DVFSManager::UsefulWorkMetric uwm;
    if (metric == "Rollbacks")
    { uwm = DVFSManager::ROLLBACKS; }
    else if (metric == "RollbackFraction")
    { uwm = DVFSManager::ROLLBACK_FRACTION; }
    else {
        stringstream err;
        err << "DVFSManager: UsefulWorkMetric " << metric << " is invalid. "
            << "Valid metrics are Rollbacks | RollbackFraction. Aborting simulation."
            << endl;
        mySimulationManager->shutdown(err.str());
    }

    DVFSManager::Algorithm alg;
    if (al == "Fixed")
    { alg = DVFSManager::FIXED; }
    else if (al == "Performance")
    { alg = DVFSManager::PERFORMANCE; }
    else if (al == "Power")
    { alg = DVFSManager::POWER; }
    else if (al == "Hybrid")
    { alg = DVFSManager::HYBRID; }
    else {
        stringstream err;
        err << "DVFSManager: Algorithm " << al << " is invalid. "
            << "Valid options are Fixed | Performance | Power | Hybrid. "
            << "Aborting simulation." << endl;
        mySimulationManager->shutdown(err.str());
    }

    if (dvfsManagerType == "Shared" || dvfsManagerType == "Distributed") {
        cout << "("
             << mySimulationManager->getSimulationManagerID()
             << ") configured a " << dvfsManagerType << " DVFS Manager" << endl
             << "Algorithm: " << al << endl
             << "Period: " << p << endl
             << "FIR Size: " << firsize << endl
             << "Useful work metric: " << metric << endl
             << "Threshold: " << threshold << endl;
        if (dvfsDebugPrint)
        { cout <<  "Writing trace to csv" << endl; }

        if (dvfsManagerType == "Shared")
            return new SharedDVFSManager(mySimulationManager,
                                         p,
                                         firsize,
                                         alg,
                                         dvfsDebugPrint,
                                         uwm,
                                         threshold);

        return new DistributedDVFSManager(mySimulationManager,
                                          p,
                                          firsize,
                                          alg,
                                          dvfsDebugPrint,
                                          uwm,
                                          threshold);
    }
    stringstream err;
    cerr << "DVFSManager: invalid type '" << dvfsManagerType << "'." << endl
         << "Valid types are None | Shared | Distributed. Aborting simulation.";
    mySimulationManager->shutdown(err.str());
}

const DVFSManagerFactory*
DVFSManagerFactory::instance() {
    static DVFSManagerFactory* singleton = new DVFSManagerFactory();
    return singleton;
}
