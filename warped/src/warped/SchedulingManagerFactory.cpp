
#include "SchedulingManagerFactory.h"
#include "TimeWarpEventSetFactory.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"
#include "SchedulingManager.h"
#include "DefaultSchedulingManager.h"
#include "TimeWarpMultiSetSchedulingManager.h"
#include <WarpedDebug.h>

#include "ThreadedTimeWarpSimulationManager.h"
#include "ThreadedTimeWarpMultiSetSchedulingManager.h"
using std::cerr;
using std::endl;

SchedulingManagerFactory::SchedulingManagerFactory(){}

SchedulingManagerFactory::~SchedulingManagerFactory(){
  // myScheduler will be deleted by the end user - the
  // TimeWarpSimulationManager
}

Configurable *
SchedulingManagerFactory::allocate( SimulationConfiguration &configuration,
				    Configurable *parent ) const {

  SchedulingManager *retval = 0;

  std::string simulationType = configuration.get_string({"Simulation"}, "Sequential");
  std::string schedulerType = configuration.get_string({"TimeWarp", "Scheduler", "Type"},
                                                        "Default");

  if(schedulerType == "MultiSet"){
    TimeWarpSimulationManager *mySimulationManager = dynamic_cast<TimeWarpSimulationManager *>( parent );
    ASSERT(mySimulationManager!=0);
    retval = new TimeWarpMultiSetSchedulingManager( mySimulationManager );
    debug::debugout << " a TimeWarpMultiSetSchedulingManager." << endl;
  } else {
    dynamic_cast<TimeWarpSimulationManager *>(parent)->shutdown(
       "Unknown SCHEDULER choice \"" + schedulerType + "\"" );
  }
  

	if (simulationType == "ThreadedTimeWarp") {
		if (schedulerType == "MultiSet") {
			ThreadedTimeWarpSimulationManager *mySimulationManager =
					dynamic_cast<ThreadedTimeWarpSimulationManager *> (parent);
			ASSERT(mySimulationManager!=0);
			retval = new ThreadedTimeWarpMultiSetSchedulingManager(mySimulationManager);
			debug::debugout << " a Threaded TimeWarpMultiSetSchedulingManager." << endl;
		} else {
			dynamic_cast<TimeWarpSimulationManager *> (parent)->shutdown(
					"Unknown SCHEDULER choice \"" + schedulerType + "\"");
		}
	}

  return retval;
}

const SchedulingManagerFactory *
SchedulingManagerFactory::instance(){
  static SchedulingManagerFactory *singleton = new SchedulingManagerFactory();
  return singleton;
}
