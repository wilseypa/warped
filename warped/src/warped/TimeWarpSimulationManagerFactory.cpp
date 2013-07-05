// See copyright notice in file Copyright in the root directory of this archive.

#include "TimeWarpSimulationManagerFactory.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"
#include "ThreadedTimeWarpSimulationManager.h"
using std::cerr;
using std::endl;

TimeWarpSimulationManagerFactory::TimeWarpSimulationManagerFactory(){}

TimeWarpSimulationManagerFactory::~TimeWarpSimulationManagerFactory(){
  // myTimeWarpSimulationManager will be deleted by the end user - the
  // TimeWarpSimulationManager
}

Configurable *
TimeWarpSimulationManagerFactory::allocate(
		SimulationConfiguration &configuration, Configurable *parent) const {
	TimeWarpSimulationManager *retval = 0;

	if (configuration.simulationTypeIs("ThreadedTimeWarp")) {
		//Count the number of threads, if none specified try reading the proc file
		unsigned int numberOfWorkerThreads = 0;
		//Specify the synchronization mechanism
		string syncMechanism = "(none)";
		//Specify the load balancing option
		string loadBalancing = "(none)";
		//Specify the load balancing metric
		string loadBalancingMetric = "(none)";
		//Specify the load balancing trigger 
		string loadBalancingTrigger = "(none)";
		//Read the load balancing variance threshold
		double loadBalancingVarianceThresh = 0.0;
		//Read the specified interval for normal load balancing
		unsigned int loadBalancingNormalInterval = 0;
		//Read the specified threshold for normal load balancing
		unsigned int loadBalancingNormalThresh = 0;
		//Read the specified interval for relaxed load balancing
		unsigned int loadBalancingRelaxedInterval = 0;
		//Read the specified threshold for relaxed load balancing
		unsigned int loadBalancingRelaxedThresh = 0;
		//Specify the schedule queue scheme
		string scheduleQScheme = "(none)";
		//Specify the schedule queue causality type
		string causalityType = "(none)";
		//Count the number of schedule queues, if none specified try reading the proc file
		unsigned int scheduleQCount = 0;

		if (!configuration.getWorkerThreadCount(numberOfWorkerThreads)) {
			//numberOfWorkerThreads = GetProcCount(); // need to implement this GetProcCount for Threaded Simulator
			cerr << "Number of Threads has not been mentioned in the file!" << endl;
		}
		if ( (syncMechanism = configuration.getSyncMechanism()) == "(none)" ) {
			cerr << "Synchronization mechanism has not been mentioned in the file!" << endl;
		}
		if ( (loadBalancing = configuration.getLoadBalancing()) == "(none)" ) {
			cerr << "Load Balancing option has not been mentioned in the file!" << endl;
		}
		if ( (loadBalancingMetric = configuration.getLoadBalancingMetric()) == "(none)" ) {
			cerr << "Load balancing metric has not been mentioned in the file!" << endl;
		}
		if ( (loadBalancingTrigger = configuration.getLoadBalancingTrigger()) == "(none)" ) {
			cerr << "Load balancing trigger has not been mentioned in the file!" << endl;
		}
		if (!configuration.getLoadBalancingVarianceThresh(loadBalancingVarianceThresh)) {
			cerr << "Load balancing variance threshold has not been mentioned in the file!" << endl;
		}
		if (!configuration.getLoadBalancingNormalInterval(loadBalancingNormalInterval)) {
			cerr << "Load balancing normal interval has not been mentioned in the file!" << endl;
		}
		if (!configuration.getLoadBalancingNormalThresh(loadBalancingNormalThresh)) {
			cerr << "Load balancing normal thresh has not been mentioned in the file!" << endl;
		}
		if (!configuration.getLoadBalancingRelaxedInterval(loadBalancingRelaxedInterval)) {
			cerr << "Load balancing relaxed interval has not been mentioned in the file!" << endl;
		}
		if (!configuration.getLoadBalancingRelaxedThresh(loadBalancingRelaxedThresh)) {
			cerr << "Load balancing relaxed threshold has not been mentioned in the file!" << endl;
		}
		if ( (scheduleQScheme = configuration.getScheduleQScheme()) == "(none)" ) {
			cerr << "Schedule Queue scheme has not been mentioned in the file!" << endl;
		}
		if ( (causalityType = configuration.getCausalityType()) == "(none)" ) {
			cerr << "Schedule Queue causality type has not been mentioned in the file!" << endl;
		}
		if (!configuration.getScheduleQCount(scheduleQCount)) {
			cerr << "Number of schedule queues has not been mentioned in the file!" << endl;
		}
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

