
#include "TimeWarpEventSet.h"
#include "TimeWarpCentralizedEventSetFactory.h"
#include "TimeWarpCentralizedEventSet.h"
#include "TimeWarpSenderQueue.h"
#include "SimulationConfiguration.h"
using std::cerr;
using std::endl;

TimeWarpCentralizedEventSetFactory::TimeWarpCentralizedEventSetFactory() {}

TimeWarpCentralizedEventSetFactory::~TimeWarpCentralizedEventSetFactory() {
    // myEventSet will be deleted by the end user - the
    // TimeWarpSimulationManager.
}

Configurable*
TimeWarpCentralizedEventSetFactory::allocate(SimulationConfiguration&,
                                             Configurable*) const {

    TimeWarpCentralizedEventSet* retval = 0;
    cerr << "Unknown EVENTLIST choice encountered" << endl;
    cerr << "Exiting simulation ... " << endl;
    exit(-1);

    return retval;
}

