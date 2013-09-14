
#include "SchedulingManagerFactory.h"
#include "TimeWarpEventSetFactory.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"
#include "SchedulingManager.h"
#include "DefaultSchedulingManager.h"
#include "TimeWarpMultiSetSchedulingManager.h"
#include <WarpedDebug.h>
/*
#if USE_TIMEWARP
#include "threadedtimewarp/ThreadedTimeWarpSimulationManager.h"
#include "threadedtimewarp/ThreadedSchedulingManager.h"
#endif
*/
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

  if(configuration.schedulerTypeIs( "MULTISET" )){
    TimeWarpSimulationManager *mySimulationManager = dynamic_cast<TimeWarpSimulationManager *>( parent );
    ASSERT(mySimulationManager!=0);
    retval = new TimeWarpMultiSetSchedulingManager( mySimulationManager );
    debug::debugout << " a TimeWarpMultiSetSchedulingManager." << endl;
  }
  else {
    dynamic_cast<TimeWarpSimulationManager *>(parent)->shutdown( "Unknown SCHEDULER choice \"" +
                                                                  configuration.getSchedulerType() + "\"" );
  }
  

#if USE_TIMEWARP
	if (simulationType == "ThreadedTimeWarp") {
		if (configuration.schedulerTypeIs("MULTISET")) {
			ThreadedTimeWarpSimulationManager *mySimulationManager =
					dynamic_cast<ThreadedTimeWarpSimulationManager *> (parent);
			ASSERT(mySimulationManager!=0);
			retval = new ThreadedTimeWarpMultiSetSchedulingManager(mySimulationManager);
			debug::debugout << " a Threaded TimeWarpMultiSetSchedulingManager." << endl;
		} else {
			dynamic_cast<TimeWarpSimulationManager *> (parent)->shutdown(
					"Unknown SCHEDULER choice \""
							+ configuration.getSchedulerType() + "\"");
		}
	}
#endif

  return retval;
}

const SchedulingManagerFactory *
SchedulingManagerFactory::instance(){
  static SchedulingManagerFactory *singleton = new SchedulingManagerFactory();
  return singleton;
}
