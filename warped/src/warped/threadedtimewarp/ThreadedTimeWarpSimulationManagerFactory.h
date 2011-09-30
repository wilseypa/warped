#ifndef TIME_WARP_SIMULATION_MANAGER_FACTORY_H
#define TIME_WARP_SIMULATION_MANAGER_FACTORY_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "SimulationManager.h"
#include "Configurer.h"

/** The TimeWarpSimulationManagerFactory class.

    This is the  factory interface for the various GVT
    Estimation Algorithm implementations in the Simulation kernel. The
    specific gvt estimation implementation is hidden from the other
    components of the simulation kernel by the aggregation of the
    different implemetations in this class interface ( factory
    pattern).  All gvt estimation implementations must however be
    derived from the ThreadedTimeWarpSimulationManager abstract base class.

*/
class ThreadedTimeWarpSimulationManagerFactory : virtual public Configurer {
public:

  /**@name Public Class Methods of ThreadedTimeWarpSimulationManagerFactory. */
  //@{

  /// Constructor.
  ThreadedTimeWarpSimulationManagerFactory();

  /// Destructor
  virtual ~ThreadedTimeWarpSimulationManagerFactory();

  /** Configures the GVT manager.

  @param configuration The configuration options.
  @param parent        The application being started.
  
  */
  Configurable *allocate( SimulationConfiguration &configuration,
			  Configurable *parent ) const;

  static const ThreadedTimeWarpSimulationManagerFactory *instance();
   
  //@} // End of Public Class Methods of TimeWarpSimulationManagerFactory.

private:

  /**@name Private Class Attributes of TimeWarpSimulationManagerFactory. */
  //@{

  /// Counts the number of execution threads listed in the proc file
  unsigned int GetProcCount() const;
   
  //@} // End of  Private Class Attributes of TimeWarpSimulationManagerFactory.
   
};

#endif
