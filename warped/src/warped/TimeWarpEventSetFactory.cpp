// See copyright notice in file Copyright in the root directory of this archive.

#include "DefaultTimeWarpEventSet.h"
#include "TimeWarpEventSetFactory.h"
#include "TimeWarpMultiSet.h"
#include "TimeWarpMultiSetOneAntiMsg.h"
#include "TimeWarpSimulationManager.h"
#include "TimeWarpSenderQueue.h"
#include "SimulationConfiguration.h"
#include "SchedulingData.h"
#if USE_TIMEWARP
#include "threadedtimewarp/ThreadedTimeWarpEventSet.h"
#endif

#include "dynamic/DTTimeWarpMultiSet.h"
#include "dynamic/DTTimeWarpSimulationManager.h"


using std::cerr;
using std::endl;

Configurable *
TimeWarpEventSetFactory::allocate(SimulationConfiguration &configuration,
		Configurable *parent) const {
	TimeWarpEventSet *retval = 0;
	TimeWarpSimulationManager *mySimulationManager =
			dynamic_cast<TimeWarpSimulationManager *> (parent);

	ASSERT( mySimulationManager != 0 );

	bool usingOneAntiMessageOpt = false;
	if (configuration.antiMessagesIs("ONE")) {
		usingOneAntiMessageOpt = true;
	}
#if USE_TIMEWARP
	if (configuration.simulationTypeIs("DTTimeWarp")) {
		if (configuration.eventListTypeIs("MULTISET")) {
			DTTimeWarpEventSet *retvalue = 0;
			retvalue = new DTTimeWarpMultiSet(
					dynamic_cast<DTTimeWarpSimulationManager *> (parent));
			utils::debug << "("
					<< mySimulationManager->getSimulationManagerID()
					<< ") configured an Dynamic Threaded MultiSet as the event set"
					<< endl;
			return retvalue;
		} else {
			mySimulationManager->shutdown(
					"Event list type \"" + configuration.getEventListType()
							+ "\" is not a valid choice.");
		}
	}
#endif
	if (configuration.eventListTypeIs("DEFAULT")) {
		retval = new DefaultTimeWarpEventSet(mySimulationManager,
				usingOneAntiMessageOpt);
		utils::debug << "(" << mySimulationManager->getSimulationManagerID()
				<< ") configured a DefaultTimeWarpEventSet as the event set"
				<< endl;
	} else if (configuration.eventListTypeIs("MULTISET")) {
		if (usingOneAntiMessageOpt) {
			retval = new TimeWarpMultiSetOneAntiMsg(mySimulationManager);
			utils::debug << "("
					<< mySimulationManager->getSimulationManagerID()
					<< ") configured a TimeWarpMultiSetOneAntiMsg as the event set"
					<< endl;
		} else {
			retval = new TimeWarpMultiSet(mySimulationManager);
			utils::debug << "("
					<< mySimulationManager->getSimulationManagerID()
					<< ") configured a TimeWarpMultiSet as the event set"
					<< endl;
		}
	}
#if USE_TIMEWARP
	else if (configuration.eventListTypeIs("THREADED")) {
		retval = new ThreadedTimeWarpEventSet(mySimulationManager);
		utils::debug << "(" << mySimulationManager->getSimulationManagerID()
				<< ") configured an ThreadedTimeWarpEventSet as the event set"
				<< endl;
	}
#endif
	else {
		mySimulationManager->shutdown(
				"Event list type \"" + configuration.getEventListType()
						+ "\" is not a valid choice.");
	}



	return retval;
}

const TimeWarpEventSetFactory *
TimeWarpEventSetFactory::instance() {
	static const TimeWarpEventSetFactory *retval =
			new TimeWarpEventSetFactory();
	return retval;
}
