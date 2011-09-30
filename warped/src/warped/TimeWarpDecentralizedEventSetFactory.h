#ifndef TIME_WARP_DECENTRALIZED_EVENT_SET_FACTORY_H
#define TIME_WARP_DECENTRALIZED_EVENT_SET_FACTORY_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "TimeWarpEventSetFactory.h"
#include "TimeWarpDecentralizedEventSet.h"
#include "TimeWarpSimulationManager.h"

class Event;
class SimulationManager;

/** The TimeWarpDecentralizedEventSetFactory class.

This is the class interface for the various Time Warp centralized
event setimplementations that are available in the Simulation
kernel. The specific event set implementation is hidden from the
other components of the simulation kernel by the aggregation of
the event set implemetations in this class interface (factory
pattern).  All Centralized event set implementations must be
derived from the TimeWarpCentralizedEventSet abstract base class.

*/
class TimeWarpDecentralizedEventSetFactory : virtual public TimeWarpEventSetFactory {

public:

  /**@name Public Class Methods of EventSetAbstractFactory. */
  //@{

  /// Default Constructor.
  TimeWarpDecentralizedEventSetFactory( TimeWarpSimulationManager *simMgr );
					
  
  /// Destructor.
  ~TimeWarpDecentralizedEventSetFactory() {
    // myEventSet will be deleted by the end user - the
    // TimeWarpSimulationManager.
  }

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

#endif
