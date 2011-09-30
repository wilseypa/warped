// See copyright notice in file Copyright in the root directory of this archive.

#include "SchedulingManagerFactory.h"
#include "TimeWarpEventSetFactory.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"
#include "SchedulingManager.h"
#include "DefaultSchedulingManager.h"
#include "TimeWarpMultiSetSchedulingManager.h"
#include <utils/Debug.h>
#if USE_TIMEWARP
#include "threadedtimewarp/ThreadedTimeWarpSimulationManager.h"
#include "threadedtimewarp/ThreadedSchedulingManager.h"
#endif
#include "dynamic/DTTimeWarpSimulationManager.h"
#include "dynamic/DTTimeWarpMultiSetSchedulingManager.h"
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

  if ( configuration.schedulerTypeIs( "DEFAULT" ) ){
	if (configuration.getSimulationType()=="THREADEDWARP")
	{
#ifdef USE_TIMEWARP
	    ThreadedTimeWarpSimulationManager *mySimulationManager = dynamic_cast<ThreadedTimeWarpSimulationManager *>( parent );
	    ASSERT(mySimulationManager!=0);
	    retval = new ThreadedSchedulingManager( mySimulationManager );
	    utils::debug << " a ThreadedSchedulingManager." << endl;
#else
	     cerr << "You selected a TimeWarp simulation type but did not configure with 'enable-timewarp'" << endl;
	     cerr << "Aborting!!!" << endl;
	     abort();
#endif
	}
	else
	{
		TimeWarpSimulationManager *mySimulationManager = dynamic_cast<TimeWarpSimulationManager *>( parent );
		ASSERT(mySimulationManager!=0);
		retval = new DefaultSchedulingManager( mySimulationManager );
		utils::debug << " a DefaultSchedulingManager." << endl;
	}
  }
  else if(configuration.schedulerTypeIs( "MULTISET" )){
    TimeWarpSimulationManager *mySimulationManager = dynamic_cast<TimeWarpSimulationManager *>( parent );
    ASSERT(mySimulationManager!=0);
    retval = new TimeWarpMultiSetSchedulingManager( mySimulationManager );
    utils::debug << " a TimeWarpMultiSetSchedulingManager." << endl;
  }
  else {
    dynamic_cast<TimeWarpSimulationManager *>(parent)->shutdown( "Unknown SCHEDULER choice \"" +
                                                                  configuration.getSchedulerType() + "\"" );
  }
  

#if USE_TIMEWARP
	if (configuration.simulationTypeIs("DTTimeWarp")) {
		if (configuration.schedulerTypeIs("MULTISET")) {
			DTTimeWarpSimulationManager *mySimulationManager =
					dynamic_cast<DTTimeWarpSimulationManager *> (parent);
			ASSERT(mySimulationManager!=0);
			retval = new DTTimeWarpMultiSetSchedulingManager(mySimulationManager);
			utils::debug << " a DT TimeWarpMultiSetSchedulingManager." << endl;
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
