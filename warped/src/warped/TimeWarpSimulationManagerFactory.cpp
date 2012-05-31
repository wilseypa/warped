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
		if (!configuration.getWorkerThreadCount(numberOfWorkerThreads)) {
			//numberOfWorkerThreads = GetProcCount(); // need to implement this GetProcCount for Threaded Simulator
			cerr << "Number of Threads has not been mentioned in the file!" << endl;
		}
		ThreadedTimeWarpSimulationManager *retvalue = 0;
        retvalue = new ThreadedTimeWarpSimulationManager(numberOfWorkerThreads, (Application *) parent);
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

