#ifndef TIME_WARP_CENTRALIZED_EVENT_SET_FACTORY_H
#define TIME_WARP_CENTRALIZED_EVENT_SET_FACTORY_H


#include "SimulationManager.h"
#include "TimeWarpEventSetFactory.h"

class Event;
class TimeWarpCentralizedEventSet;

/** The TimeWarpCentralizedEventSetFactory class.

    This is the class interface for the various Time Warp centralized
    event setimplementations that are available in the Simulation
    kernel. The specific event set implementation is hidden from the
    other components of the simulation kernel by the aggregation of
    the event set implemetations in this class interface (factory
    pattern).  All Centralized event set implementations must be
    derived from the TimeWarpCentralizedEventSet abstract base class.

*/
class TimeWarpCentralizedEventSetFactory : public TimeWarpEventSetFactory {

public:

  /**@name Public Class Methods of EventSetAbstractFactory. */
  //@{

  /// Default Constructor.
  TimeWarpCentralizedEventSetFactory();

  /// Destructor.
  virtual ~TimeWarpCentralizedEventSetFactory();

  /** Set the configured event set implementation.
      
      Function to attach the particular event set implementation in
      the kernel. Will be called by the configuration step to set the
      appropriate event set implementation.

      @param configurationMap Set of configuration options.
  */
  Configurable *allocate( SimulationConfiguration &configuration,
			  Configurable *parent ) const;

  //@} // End of Public Class Methods of EventSetAbstractFactory.
   
protected:
   
  /**@name Protected Class Attributes of EventSetAbstractFactory. */
  //@{

  //@} // End of Protected Class Attributes of EventSetAbstractFactory.
};

#endif //ifdef TIME_WARP_CENTRALIZED_EVENT_SET_FACTORY_H
