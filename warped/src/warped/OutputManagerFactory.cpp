
#include "Event.h"
#include "SimulationObject.h"
#include "OutputManager.h"
#include "AggressiveOutputManager.h"
#include "LazyOutputManager.h"
#include "AdaptiveOutputManager.h"
#include "OutputManagerFactory.h"
#include "SimulationConfiguration.h"
#include "TimeWarpSimulationManager.h"
#include "ThreadedOutputManager.h"
#include "ThreadedAggressiveOutputManager.h"
#include "ThreadedLazyOutputManager.h"
#include "ThreadedDynamicOutputManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include <WarpedDebug/WarpedDebug.h>

using std::cerr;
using std::endl;

OutputManagerFactory::OutputManagerFactory() {
}

OutputManagerFactory::~OutputManagerFactory() {
}

// configure this  factory to instantiate the chosen output manager
Configurable *
OutputManagerFactory::allocate(SimulationConfiguration &configuration,
		Configurable *parent) const {

	Configurable *retval = 0;
	TimeWarpSimulationManager *mySimulationManager =
			dynamic_cast<TimeWarpSimulationManager *> (parent);
	ASSERT( mySimulationManager != 0 );

	if (configuration.antiMessagesIs("ONE")) {
		mySimulationManager->setOneAntiMsg(true);
	}

	// the following cases are possible:

	// (1) AggressiveOutputManager
	// (2) LazyOutputManager
	// (3) DynamicOutputManager
	if (configuration.outputManagerIs("AGGRESSIVE")) {
		retval = new AggressiveOutputManager(mySimulationManager);
		mySimulationManager->setOutputMgrType(AGGRMGR);
		debug::debugout << "an Aggressive Output Manager" << endl;
	} else if (configuration.outputManagerIs("LAZY")) {
		retval = new LazyOutputManager(mySimulationManager);
		mySimulationManager->setOutputMgrType(LAZYMGR);
		debug::debugout << "a Lazy Output Manager" << endl;
	} else if (configuration.outputManagerIs("DYNAMIC")) {
		retval = new DynamicOutputManager(mySimulationManager);
		mySimulationManager->setOutputMgrType(ADAPTIVEMGR);
		debug::debugout << "an Dynamic Output Manager" << endl;
	} else {
		const string error = "Unknown OutputManager choice \""
				+ configuration.getOutputManagerType() + "\" encountered.";
		mySimulationManager->shutdown(error);
	}

#if USE_TIMEWARP
	if (configuration.simulationTypeIs("ThreadedTimeWarp")) {
		if (configuration.outputManagerIs("AGGRESSIVE")) {
			retval = new ThreadedAggressiveOutputManager(
					dynamic_cast<ThreadedTimeWarpSimulationManager *> (parent));
			mySimulationManager->setOutputMgrType(AGGRMGR);
			debug::debugout << "a Dynamic Threaded Aggressive Output Manager"
					<< endl;
		} else if (configuration.outputManagerIs("LAZY")) {
			retval = new ThreadedLazyOutputManager(
					dynamic_cast<ThreadedTimeWarpSimulationManager *> (parent));
			mySimulationManager->setOutputMgrType(LAZYMGR);
			debug::debugout << "a Lazy Output Manager" << endl;
		} else if (configuration.outputManagerIs("DYNAMIC")) {
			unsigned int filterDepth = 16;
			double aggr2lazy = 0.5;
			double lazy2aggr = 0.2;
			double thirdThreshold = 0.1;
			configuration.getDynamicFilterDepth(filterDepth);
			configuration.getAggressive2Lazy(aggr2lazy);
			configuration.getLazy2Aggressive(lazy2aggr);
			configuration.getThirdThreshold(thirdThreshold);
			retval = new ThreadedDynamicOutputManager(
					dynamic_cast<ThreadedTimeWarpSimulationManager *> (parent),
					filterDepth, aggr2lazy, lazy2aggr, thirdThreshold);
			mySimulationManager->setOutputMgrType(ADAPTIVEMGR);
			debug::debugout << "an Dynamic Output Manager" << endl;
		} else {
			const string error = "Unknown OutputManager choice \""
					+ configuration.getOutputManagerType() + "\" encountered.";
			mySimulationManager->shutdown(error);
		}
	}
#endif

	return retval;
}

const OutputManagerFactory *
OutputManagerFactory::instance() {
	static OutputManagerFactory *singleton = new OutputManagerFactory();
	return singleton;
}
