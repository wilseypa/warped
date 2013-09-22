
#include "TimeWarpSimulationManagerFactory.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"
#include "ThreadedTimeWarpSimulationManager.h"

#include <string>

TimeWarpSimulationManagerFactory::TimeWarpSimulationManagerFactory(){}

TimeWarpSimulationManagerFactory::~TimeWarpSimulationManagerFactory(){
  // myTimeWarpSimulationManager will be deleted by the end user - the
  // TimeWarpSimulationManager
}

Configurable *
TimeWarpSimulationManagerFactory::allocate(
		SimulationConfiguration &configuration, Configurable *parent) const {
	TimeWarpSimulationManager *retval = 0;

    std::string simulationType = configuration.get_string({"Simulation"}, "Sequential");

	if (simulationType == "ThreadedTimeWarp") {
		//Count the number of threads, if none specified try reading the proc file
		int numberOfWorkerThreads = configuration.get_int({"TimeWarp", "ThreadControl", "WorkerThreadCount"}, 4);
		//Specify the synchronization mechanism
		std::string syncMechanism = configuration.get_string({"TimeWarp", "ThreadControl", "SyncMechanism"},
                                                     			"Mutex");
		//Specify the load balancing option
		bool loadBalancing = configuration.get_bool({"TimeWarp", "ThreadControl", "LoadBalancing"},
                                                     			false);
		//Specify the load balancing metric
		std::string loadBalancingMetric = configuration.get_string({"TimeWarp", "ThreadControl", "LoadBalancingMetric"},
                                                     			"EventExecutionRatio");
		//Specify the load balancing trigger 
		std::string loadBalancingTrigger = configuration.get_string({"TimeWarp", "ThreadControl", "LoadBalancingTrigger"},
                                                     			"Rollback");
		//Read the load balancing variance threshold
		double loadBalancingVarianceThresh = configuration.get_double({"TimeWarp", "ThreadControl", "LoadBalancingVarianceThresh"}, 
																		0.2);
		//Read the specified interval for normal load balancing
		int loadBalancingNormalInterval = configuration.get_int({"TimeWarp", "ThreadControl", "LoadBalancingNormalInterval"}, 
																3);
		//Read the specified threshold for normal load balancing
		int loadBalancingNormalThresh = configuration.get_int({"TimeWarp", "ThreadControl", "LoadBalancingNormalThresh"}, 
																5);
		//Read the specified interval for relaxed load balancing
		int loadBalancingRelaxedInterval = configuration.get_int({"TimeWarp", "ThreadControl", "LoadBalancingRelaxedInterval"}, 
																15);
		//Read the specified threshold for relaxed load balancing
		int loadBalancingRelaxedThresh = configuration.get_int({"TimeWarp", "ThreadControl", "LoadBalancingRelaxedThresh"}, 
																2);
		//Specify the schedule queue scheme
		std::string scheduleQScheme = configuration.get_string({"TimeWarp", "Scheduler", "ScheduleQScheme"},
                                                     			"MultiSet");
		//Specify the schedule queue causality type
		std::string causalityType = configuration.get_string({"TimeWarp", "Scheduler", "CausalityType"},
                                                     			"Strict");
		//Count the number of schedule queues, if none specified try reading the proc file
		int scheduleQCount = configuration.get_int({"TimeWarp", "Scheduler", "ScheduleQCount"}, 2);

		ThreadedTimeWarpSimulationManager *retvalue = 0;
        	retvalue = new ThreadedTimeWarpSimulationManager(numberOfWorkerThreads, syncMechanism, 
					loadBalancing, loadBalancingMetric, loadBalancingTrigger, loadBalancingVarianceThresh, 
						loadBalancingNormalInterval, loadBalancingNormalThresh, loadBalancingRelaxedInterval, 
							loadBalancingRelaxedThresh, scheduleQScheme, causalityType, scheduleQCount, (Application *) parent);
		return retvalue;
	} else {
		retval = new TimeWarpSimulationManager((Application *) parent);
	}

	return retval;
}

const TimeWarpSimulationManagerFactory *
TimeWarpSimulationManagerFactory::instance(){
  static TimeWarpSimulationManagerFactory *singleton = new TimeWarpSimulationManagerFactory();

  return singleton;
}

