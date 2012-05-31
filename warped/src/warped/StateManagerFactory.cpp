// See copyright notice in file Copyright in the root directory of this archive.

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
#include <utils/Debug.h>

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
#ifdef USE_TIMEWARP
	ThreadedStateManager *retvalue = 0;
#endif

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

	unsigned int statePeriod = 0;
#if USE_TIMEWARP
	if (configuration.simulationTypeIs("ThreadedTimeWarp")) {
		if (configuration.stateManagerTypeIs("PERIODIC")) {
			configuration.getStatePeriod(statePeriod);
			retvalue = new ThreadedPeriodicStateManager(
					dynamic_cast<ThreadedTimeWarpSimulationManager *> (parent),
					statePeriod);
			mySimulationManager->setStateMgrType(STATICSTATE);
			utils::debug << "("
					<< mySimulationManager->getSimulationManagerID()
					<< ") configured a Threaded Periodic State Manager with period = "
					<< statePeriod << endl;
			return retvalue;
		} else if (configuration.stateManagerTypeIs("ADAPTIVE")) {
			configuration.getStatePeriod(statePeriod);
			retvalue = new ThreadedCostAdaptiveStateManager(
					dynamic_cast<ThreadedTimeWarpSimulationManager *> (parent));
			mySimulationManager->setStateMgrType(ADAPTIVESTATE);
			utils::debug << "("
					<< mySimulationManager->getSimulationManagerID()
					<< ") configured an Adaptive State Manager with period = "
					<< statePeriod << endl;
			return retvalue;
		} else {
			mySimulationManager->shutdown(
					"Unknown StateManager choice \""
							+ configuration.getStateManagerType() + "\"");
		}
	}
#endif
	if (configuration.stateManagerTypeIs("PERIODIC")) {
		configuration.getStatePeriod(statePeriod);
		retval = new PeriodicStateManager(mySimulationManager, statePeriod);
		mySimulationManager->setStateMgrType(STATICSTATE);
		utils::debug << "(" << mySimulationManager->getSimulationManagerID()
				<< ") configured a Periodic State Manager with period = "
				<< statePeriod << endl;
	} else if (configuration.stateManagerTypeIs("ADAPTIVE")) {
		configuration.getStatePeriod(statePeriod);
		retval = new CostAdaptiveStateManager(mySimulationManager);
		mySimulationManager->setStateMgrType(ADAPTIVESTATE);
		utils::debug << "(" << mySimulationManager->getSimulationManagerID()
				<< ") configured an Adaptive State Manager with period = "
				<< statePeriod << endl;
	} else {
		mySimulationManager->shutdown(
				"Unknown StateManager choice \""
						+ configuration.getStateManagerType() + "\"");
	}
	return retval;
}

const StateManagerFactory *
StateManagerFactory::instance() {
	static const StateManagerFactory *singleton = new StateManagerFactory();

	return singleton;
}

