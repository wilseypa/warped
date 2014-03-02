
#include <stdlib.h>                     // for exit
#include <iostream>                     // for operator<<, cerr, endl, etc

#include "SetObject.h"                  // for ostream
#include "TimeWarpCentralizedEventSet.h"
#include "TimeWarpCentralizedEventSetFactory.h"

class Configurable;
class SimulationConfiguration;

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

