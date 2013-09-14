
#include "TimeWarpSimulationObjectQueueFactory.h"
#include "TimeWarpSimulationManager.h"
#include "TimeWarpSimulationObjectQueue.h"
#include "SimulationConfiguration.h"
#include "SchedulingData.h"
using std::cerr;
using std::endl;

Configurable *
TimeWarpSimulationObjectQueueFactory::allocate( SimulationConfiguration &configuration,
				   Configurable *parent ) const {
  TimeWarpSimulationObjectQueue *retval = 0;
  return retval;
}

const TimeWarpSimulationObjectQueueFactory *
TimeWarpSimulationObjectQueueFactory::instance(){
  static const TimeWarpSimulationObjectQueueFactory *retval = new TimeWarpSimulationObjectQueueFactory();
  return retval;
}
