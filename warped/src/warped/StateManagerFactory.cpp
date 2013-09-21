#include "StateManagerFactory.h"
#include "PeriodicStateManager.h"
#include "CostAdaptiveStateManager.h"
#include "SimulationConfiguration.h"
#include "State.h"
#include "SimulationObject.h"
#include "TimeWarpSimulationManager.h"
#include "ThreadedPeriodicStateManager.h"
#include "ThreadedCostAdaptiveStateManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "WarpedDebug.h"

#include <string>

StateManagerFactory::StateManagerFactory() {
}

StateManagerFactory::~StateManagerFactory() {
	// myStateManager is deleted by the end user - the
	// TimeWarpSimulationManager
}

Configurable *
StateManagerFactory::allocate(SimulationConfiguration &configuration,
		Configurable *parent) const {
	StateManager *retval = 0;

	ThreadedStateManager *retvalue = 0;

	TimeWarpSimulationManager *mySimulationManager =
			dynamic_cast<TimeWarpSimulationManager *> (parent);
	ASSERT( mySimulationManager != 0 );

	// the following cases are possible:

	// (1) StateManager is InfrequentStateManager. In this is the case,
	//     we need to find a state period (if any; defaults to 1). Then
	//     instantiate the InfrequentStateManager with a state period
	//     (if one is found).

	// (2) StateManager is AdaptiveStateManager. In this case, we just
	//     instantiate the AdaptiveStateManager and go on.

	std::string simulationType = configuration.get_string({"Simulation"}, "Sequential");
	std::string stateManagerType = configuration.get_string({"TimeWarp", "StateManager", "Type"}, 
														"Periodic");
	int stateManagerPeriod = configuration.get_int({"TimeWarp", "StateManager", "Period"}, 10);

	if (simulationType == "ThreadedTimeWarp") {
		if (stateManagerType == "Periodic") {
			retvalue = new ThreadedPeriodicStateManager(
					dynamic_cast<ThreadedTimeWarpSimulationManager *> (parent),
					stateManagerPeriod);
			mySimulationManager->setStateMgrType(STATICSTATE);
			debug::debugout << "("
					<< mySimulationManager->getSimulationManagerID()
					<< ") configured a Threaded Periodic State Manager with period = "
					<< stateManagerPeriod << endl;
			return retvalue;
		} else if (stateManagerType == "Adaptive") {
			retvalue = new ThreadedCostAdaptiveStateManager(
					dynamic_cast<ThreadedTimeWarpSimulationManager *> (parent));
			mySimulationManager->setStateMgrType(ADAPTIVESTATE);
			debug::debugout << "("
					<< mySimulationManager->getSimulationManagerID()
					<< ") configured an Adaptive State Manager with period = "
					<< stateManagerPeriod << endl;
			return retvalue;
		} else {
			mySimulationManager->shutdown("Unknown StateManager choice \"" + stateManagerType + "\"");
		}
	}

	if (stateManagerType == "Periodic") {
		retval = new PeriodicStateManager(mySimulationManager, stateManagerPeriod);
		mySimulationManager->setStateMgrType(STATICSTATE);
		debug::debugout << "(" << mySimulationManager->getSimulationManagerID()
				<< ") configured a Periodic State Manager with period = "
				<< stateManagerPeriod << endl;
	} else if (stateManagerType == "Adaptive") {
		retval = new CostAdaptiveStateManager(mySimulationManager);
		mySimulationManager->setStateMgrType(ADAPTIVESTATE);
		debug::debugout << "(" << mySimulationManager->getSimulationManagerID()
				<< ") configured an Adaptive State Manager with period = "
				<< stateManagerPeriod << endl;
	} else {
		mySimulationManager->shutdown("Unknown StateManager choice \"" + stateManagerType + "\"");
	}
	return retval;
}

const StateManagerFactory *
StateManagerFactory::instance() {
	static const StateManagerFactory *singleton = new StateManagerFactory();

	return singleton;
}

