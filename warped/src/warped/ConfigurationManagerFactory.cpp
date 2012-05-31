// See copyright notice in file Copyright in the root directory of this archive.

#include "ConfigurationManagerFactory.h"
#include "SequentialConfigurationManager.h"
#include "TimeWarpConfigurationManager.h"
#include "Application.h"
#include "SimulationConfiguration.h"
#include <utils/Debug.h>
using std::cerr;
using utils::debug;
using std::endl;

const ConfigurationManagerFactory *
ConfigurationManagerFactory::instance() {
	static const ConfigurationManagerFactory *retval =
			new ConfigurationManagerFactory();

	return retval;
}

Configurable *
ConfigurationManagerFactory::allocate(SimulationConfiguration &configuration,
		Configurable *parent) const {
	Configurable *retval = 0;

	if (configuration.simulationTypeIs("Sequential")) {
		retval = new SequentialConfigurationManager((Application *) parent);
		utils::debug << "Configured a SequentialSimulationManager" << endl;
	} else if (configuration.simulationTypeIs("TimeWarp")
			|| configuration.simulationTypeIs("ThreadedTimeWarp")) {
		retval = new TimeWarpConfigurationManager(configuration.getArguments(),
				(Application *) parent);
		utils::debug << "Configured a TimeWarpSimulationManager" << endl;
	} /*else if (configuration.simulationTypeIs("ThreadedWarp")) {
#ifdef USE_TIMEWARP
		retval = new ThreadedTimeWarpConfigurationManager( configuration.getArguments(),
				(Application *)parent );
		utils::debug << "Configured a ThreadedTimeWarpSimulationManager" << endl;
#else
		cerr
				<< "You selected a ThreadedTimeWarpSimulationManager but did not configure with enable-timewarp"
				<< endl;
		cerr << "Aborting!!!" << endl;
		abort();
#endif
	}*/ else {
		cerr << "Unknown Simulation type \""
				<< configuration.getSimulationType() << "\"" << endl;
		exit(-1);
	}

	return retval;
}
