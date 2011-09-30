// See copyright notice in file Copyright in the root directory of this archive.

#include "TimeWarpSimulationManagerFactory.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"
#include "dynamic/DTTimeWarpSimulationManager.h"
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

	unsigned int numberOfSimulationManagers = 0;
	if (!configuration.getNumberOfSimulationManagers(numberOfSimulationManagers)) {
		cerr << "Could not find number of simulation managers!" << endl;
		abort();
	}
	if (configuration.simulationTypeIs("DTTimeWarp")) {
		//Count the number of threads, if none specified try reading the proc file
		unsigned int numberOfWorkerThreads = 0;
		if (!configuration.getWorkerThreadCount(numberOfWorkerThreads)) {
			//numberOfWorkerThreads = GetProcCount(); // need to implement this GetProcCount for DT Simulator
			cerr << "Number of Threads has not been mentioned in the file!" << endl;
		}
		DTTimeWarpSimulationManager *retvalue = 0;
		retvalue = new DTTimeWarpSimulationManager(numberOfSimulationManagers,
				numberOfWorkerThreads, (Application *) parent);
		return retvalue;
	} else {
		retval = new TimeWarpSimulationManager(numberOfSimulationManagers,
				(Application *) parent);
	}

	return retval;
}

const TimeWarpSimulationManagerFactory *
TimeWarpSimulationManagerFactory::instance(){
  static TimeWarpSimulationManagerFactory *singleton = new TimeWarpSimulationManagerFactory();

  return singleton;
}

