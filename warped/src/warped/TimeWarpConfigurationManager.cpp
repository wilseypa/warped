// See copyright notice in file Copyright in the root directory of this archive.

#include "TimeWarpConfigurationManager.h"
#include "TimeWarpCentralizedEventSetFactory.h"
#include "TimeWarpDecentralizedEventSetFactory.h"
#include "TimeWarpSimulationManagerFactory.h"
#include "SchedulingData.h"
#include "Application.h"
#include "SimulationConfiguration.h"
#include "Spinner.h"
#include "ThreadedTimeWarpSimulationManager.h"
using std::cout;
using std::endl;

TimeWarpConfigurationManager::TimeWarpConfigurationManager(
		const vector<string> &arguments, Application *initApplication) :
	mySimulationManager(0), myCommandLineArguments(arguments),
			myEventSetFactory(0), myStateFactory(0), myGVTFactory(0),
			myCommunicationFactory(0), mySchedulingFactory(0),
			myOutputFactory(0), myApplication(initApplication) {
}

TimeWarpConfigurationManager::~TimeWarpConfigurationManager() {
	delete myEventSetFactory;
	delete myStateFactory;
	delete myGVTFactory;
	delete myCommunicationFactory;
	delete mySchedulingFactory;
	delete myOutputFactory;
	delete mySimulationManager;
	// the handles to the eventSetManager, stateManager, GVTManager,
	// CommunicationManager, schedulingManager and the outputManager
	// are deleted by the end user - the TimeWarpSimulationManager.
}

void TimeWarpConfigurationManager::configure(
		SimulationConfiguration &configuration) {
	cout << "Initializing simulation...\n";

	const TimeWarpSimulationManagerFactory *twSimMgrFactory =
			TimeWarpSimulationManagerFactory::instance();
	if (configuration.simulationTypeIs("TimeWarp")) {
		mySimulationManager
				= dynamic_cast<TimeWarpSimulationManager *> (twSimMgrFactory->allocate(
						configuration, myApplication));
	} else if (configuration.simulationTypeIs("ThreadedTimeWarp")) {
		mySimulationManager
				= dynamic_cast<ThreadedTimeWarpSimulationManager *> (twSimMgrFactory->allocate(
						configuration, myApplication));
	} else
		cout << "Should not reach here, check the config file for SimType!!";

	mySimulationManager->configure(configuration);
	Spinner::spinIfRequested("SpinBeforeSimulationStart", configuration);
}

SimulationManager *
TimeWarpConfigurationManager::getSimulationManager() {
	return mySimulationManager;
}
