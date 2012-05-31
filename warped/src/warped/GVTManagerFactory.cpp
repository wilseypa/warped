// See copyright notice in file Copyright in the root directory of this archive.

#include "GVTManagerFactory.h"
#include "MatternGVTManager.h"
#include "ThreadedMatternGVTManager.h"
#include "SimulationConfiguration.h"
#include "TimeWarpConfigurationManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include <utils/ConfigurationScope.h>
#include <utils/Debug.h>

GVTManagerFactory::GVTManagerFactory() {
}

GVTManagerFactory::~GVTManagerFactory() {
	// myGVTManager will be deleted by the end user - the
	// TimeWarpSimulationManager
}

Configurable *
GVTManagerFactory::allocate(SimulationConfiguration &configuration,
		Configurable *parent) const {
	Configurable *retval = 0;
	TimeWarpSimulationManager *mySimulationManager =
			dynamic_cast<TimeWarpSimulationManager *> (parent);
	ASSERT( mySimulationManager != NULL );
	// the following cases are possible:

	// (1) GVTManager is Mattern. In this is the case, we need to find
	//     a GVT estimation period (if any; defaults to 1). Then
	//     instantiate the MatternGVTStateManager with a state period (if
	//     one is found).
#if USE_TIMEWARP
	if (configuration.simulationTypeIs("ThreadedTimeWarp")) {

		if (configuration.gvtManagerTypeIs("MATTERN")) {
			unsigned int gvtPeriod = 1;
			configuration.getGVTPeriod(gvtPeriod);
			retval = new ThreadedMatternGVTManager(
					dynamic_cast<ThreadedTimeWarpSimulationManager *> (parent),
					gvtPeriod);
			utils::debug << "("
			<< mySimulationManager->getSimulationManagerID()
			<< ") configured a Mattern GVT Manager with period = "
			<< gvtPeriod << endl;
			return retval;
		} else {
			mySimulationManager->shutdown("Unknown GVTManager choice \""
					+ configuration.getGVTManagerType() + "\"");
		}
	}
#endif

	if (!(configuration.simulationTypeIs("DTTimeWarp"))) {
		if (configuration.gvtManagerTypeIs("MATTERN")) {
			unsigned int gvtPeriod = 1;
			configuration.getGVTPeriod(gvtPeriod);
			retval = new MatternGVTManager(mySimulationManager, gvtPeriod);
			utils::debug << "("
					<< mySimulationManager->getSimulationManagerID()
					<< ") configured a Mattern GVT Manager with period = "
					<< gvtPeriod << endl;
		}

		else {
			mySimulationManager->shutdown(
					"Unknown GVTManager choice \""
							+ configuration.getGVTManagerType() + "\"");
		}

		return retval;
	}
}
const GVTManagerFactory *
GVTManagerFactory::instance() {
	static GVTManagerFactory *singleton = new GVTManagerFactory();

	return singleton;
}

