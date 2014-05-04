
#include <string>                       // for string, operator==, etc

#include "SimulationConfiguration.h"    // for SimulationConfiguration
#include "ThreadedTimeWarpSimulationManager.h"
#include "TimeWarpSimulationManager.h"  // for TimeWarpSimulationManager
#include "TimeWarpSimulationManagerFactory.h"

class Application;
class Configurable;

TimeWarpSimulationManagerFactory::TimeWarpSimulationManagerFactory() {}

TimeWarpSimulationManagerFactory::~TimeWarpSimulationManagerFactory() {
    // myTimeWarpSimulationManager will be deleted by the end user - the
    // TimeWarpSimulationManager
}

Configurable*
TimeWarpSimulationManagerFactory::allocate(
    SimulationConfiguration& configuration, Configurable* parent) const {
    TimeWarpSimulationManager* retval = 0;

    std::string simulationType = configuration.get_string({"Simulation"}, "Sequential");

    if (simulationType == "ThreadedTimeWarp") {
        //Count the number of threads, if none specified try reading the proc file
        int numberOfWorkerThreads = configuration.get_int({"TimeWarp", "ThreadControl", "WorkerThreadCount"},
                                                          4);
        //Specify the synchronization mechanism
        std::string syncMechanism = configuration.get_string({"TimeWarp", "ThreadControl", "SyncMechanism"},
                                                             "Mutex");
        //Specify the worker thread migration option
        bool workerThreadMigration = configuration.get_bool({"TimeWarp", "ThreadControl", "WorkerThreadMigration"},
                                                            false);
        //Specify the schedule queue scheme
        std::string scheduleQScheme = configuration.get_string({"TimeWarp", "Scheduler", "ScheduleQScheme"},
                                                               "MultiSet");
        //Specify the schedule queue causality type
        std::string causalityType = configuration.get_string({"TimeWarp", "Scheduler", "CausalityType"},
                                                             "Strict");
        //Count the number of schedule queues, if none specified try reading the proc file
        int scheduleQCount = configuration.get_int({"TimeWarp", "Scheduler", "ScheduleQCount"}, 2);

        ThreadedTimeWarpSimulationManager* retvalue = 0;
        retvalue = new ThreadedTimeWarpSimulationManager( numberOfWorkerThreads, syncMechanism, 
                                                          workerThreadMigration, scheduleQScheme, 
                                                          causalityType, scheduleQCount, 
                                                          (Application*) parent );
        return retvalue;
    } else {
        retval = new TimeWarpSimulationManager((Application*) parent);
    }

    return retval;
}

const TimeWarpSimulationManagerFactory*
TimeWarpSimulationManagerFactory::instance() {
    static TimeWarpSimulationManagerFactory* singleton = new TimeWarpSimulationManagerFactory();

    return singleton;
}

