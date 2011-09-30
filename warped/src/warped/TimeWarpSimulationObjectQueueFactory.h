#ifndef TIME_WARP_OBJECT_QUEUE_FACTORY_H_
#define TIME_WARP_OBJECT_QUEUE_FACTORY_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "TimeWarpSimulationObjectQueue.h"
#include "SimulationObject.h"
#include "SimulationManager.h"
#include "Configurer.h"

class Event;

/** The TimeWarpSimulationObjectQueueFactory class.

    This is the class interface for the various Time Warp object queue
    implementations that are available in the Simulation kernel. The
    specific object container implementation is hidden from the other
    components of the simulation kernel by the aggregation of the
    object container implemetations in this class interface ( factory
    pattern).  All object container implementations must be derived from the
    TimeWarpSimulationObjectQueue abstract base class.

*/
class TimeWarpSimulationObjectQueueFactory : virtual public Configurer {

public:

  /**@name Public Class Methods of TimeWarpSimulationObjectQueueFactory. */
  //@{

  /// Destructor.
  virtual ~TimeWarpSimulationObjectQueueFactory(){};

  Configurable *allocate( SimulationConfiguration &configuration,
			  Configurable *parent ) const;

  static const TimeWarpSimulationObjectQueueFactory *instance();

  //@} // End of Public Class Methods of TimeWarpSimulationObjectQueueFactory.

protected:

  /**@name Protected Class Methods of TimeWarpSimulationObjectQueueFactory. */
  //@{

  /// Default Constructor.
  TimeWarpSimulationObjectQueueFactory(){};

  //@} // End of Protected Class Methods of TimeWarpSimulationObjectQueueFactory.
};

#endif /* TIME_WARP_OBJECT_QUEUE_FACTORY_H_*/
