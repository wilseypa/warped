#ifndef TIME_WARP_EVENT_SET_FACTORY_H
#define TIME_WARP_EVENT_SET_FACTORY_H


#include "warped.h"
#include "TimeWarpEventSet.h"
#include "SimulationObject.h"
#include "SimulationManager.h"
#include "Configurer.h"

class Event;

/** The TimeWarpEventSetFactory class.

    This is the class interface for the various Time Warp event set
    implementations that are available in the Simulation kernel. The
    specific event set implementation is hidden from the other
    components of the simulation kernel by the aggregation of the
    event set implemetations in this class interface ( factory
    pattern).  All event set implementations must be derived from the
    TimeWarpEventSet abstract base class.
    
*/
class TimeWarpEventSetFactory : virtual public Configurer {
   
public:
   
  /**@name Public Class Methods of TimeWarpEventSetFactory. */
  //@{

  /// Destructor.
  virtual ~TimeWarpEventSetFactory(){};

  Configurable *allocate( SimulationConfiguration &configuration,
			  Configurable *parent ) const;

  static const TimeWarpEventSetFactory *instance();
      
  //@} // End of Public Class Methods of TimeWarpEventSetFactory.
   
protected:
   
  /**@name Protected Class Methods of TimeWarpEventSetFactory. */
  //@{
      
  /// Default Constructor.
  TimeWarpEventSetFactory(){};
   
  //@} // End of Protected Class Methods of TimeWarpEventSetFactory.
};

#endif
