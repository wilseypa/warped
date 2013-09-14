#include "DefaultTimeWarpEventSet.h"
#include "TimeWarpEventSetFactory.h"

#include <iostream>

#include "TimeWarpMultiSet.h"
#include "TimeWarpMultiSetOneAntiMsg.h"
#include "TimeWarpSimulationManager.h"
#include "TimeWarpSenderQueue.h"
#include "SimulationConfiguration.h"
#include "SchedulingData.h"

#include "ThreadedTimeWarpMultiSet.h"
#include "ThreadedTimeWarpSimulationManager.h"


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
	
	std::string eventListType = configuration.get_string({"EventList", "Type"}, "Default");
    std::string simulationType = configuration.get_string({"Simulation"}, "Sequential");

#if USE_TIMEWARP
	if (simulationType == "ThreadedTimeWarp") {
		if (eventListType == "MultiSet") {
			ThreadedTimeWarpEventSet *retvalue = 0;
			retvalue = new ThreadedTimeWarpMultiSet(
					dynamic_cast<ThreadedTimeWarpSimulationManager *> (parent));
			debug::debugout << "(" << mySimulationManager->getSimulationManagerID()
					<< ") configured an Dynamic Threaded MultiSet as the event set"
					<< std::endl;
			return retvalue;
		} else {
			mySimulationManager->shutdown("Event list type \"" + eventListType
											+ "\" is not a valid choice.");
		}
	}
#endif
	if (eventListType == "Default") {
		retval = new DefaultTimeWarpEventSet(mySimulationManager,
				usingOneAntiMessageOpt);
		debug::debugout << "(" << mySimulationManager->getSimulationManagerID()
				<< ") configured a DefaultTimeWarpEventSet as the event set"
				<< std::endl;
	} else if (eventListType == "MultiSet") {
		if (usingOneAntiMessageOpt) {
			retval = new TimeWarpMultiSetOneAntiMsg(mySimulationManager);
			debug::debugout << "("
					<< mySimulationManager->getSimulationManagerID()
					<< ") configured a TimeWarpMultiSetOneAntiMsg as the event set"
					<< std::endl;
		} else {
			retval = new TimeWarpMultiSet(mySimulationManager);
			debug::debugout << "("
					<< mySimulationManager->getSimulationManagerID()
					<< ") configured a TimeWarpMultiSet as the event set"
					<< std::endl;
		}
	}
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
