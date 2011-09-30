#ifndef TIME_WARP_SIMULATION_MANAGER_FACTORY_H
#define TIME_WARP_SIMULATION_MANAGER_FACTORY_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationManager.h"
#include "Configurer.h"

/** The TimeWarpSimulationManagerFactory class.

    This is the  factory interface for the various GVT
    Estimation Algorithm implementations in the Simulation kernel. The
    specific gvt estimation implementation is hidden from the other
    components of the simulation kernel by the aggregation of the
    different implemetations in this class interface ( factory
    pattern).  All gvt estimation implementations must however be
    derived from the TimeWarpSimulationManager abstract base class.

*/
class TimeWarpSimulationManagerFactory : virtual public Configurer {
public:

  /**@name Public Class Methods of TimeWarpSimulationManagerFactory. */
  //@{

  /// Constructor.
  TimeWarpSimulationManagerFactory();

  /// Destructor
  virtual ~TimeWarpSimulationManagerFactory();

  /** Configures the GVT manager.

  @param configuration The configuration options.
  @param parent        The application being started.
  
  */
  Configurable *allocate( SimulationConfiguration &configuration,
			  Configurable *parent ) const;

  static const TimeWarpSimulationManagerFactory *instance();
   
  //@} // End of Public Class Methods of TimeWarpSimulationManagerFactory.

private:

  /**@name Private Class Attributes of TimeWarpSimulationManagerFactory. */
  //@{
   
  //@} // End of  Private Class Attributes of TimeWarpSimulationManagerFactory.
   
};

#endif
