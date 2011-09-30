#ifndef THREADED_TIMEWARP_CONFIGURATION_MANAGER_H
#define THREADED_TIMEWARP_CONFIGURATION_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "ThreadedTimeWarpSimulationManager.h"

class TimeWarpConfigurationManager;

/** The ThreadedTimeWarpConfigurationManager class.

    The ThreadedTimeWarpConfigurationManager class represents the concrete
    class that is responsible for constructing and configuring a
    Threaded TimeWarp Simulation Manager and its components.
*/
class ThreadedTimeWarpConfigurationManager : public TimeWarpConfigurationManager {
public:
   
  /**@name Public Class Methods of ThreadedTimeWarpConfigurationManager */
  //@{

  /** Default constructor 

  @param numSimMgrs The number of simulation managers that have been
  requested.
  @param arguments The list of arguments passed in.
  @param initApplication The application that we are starting with.

  */
  ThreadedTimeWarpConfigurationManager( const vector<string> &arguments,
				Application *initApplication );
   
  /// Default destructor
  ~ThreadedTimeWarpConfigurationManager();

  /** configure the specified simulation manager.

  @return returns a configured simulation manager
  */
  void configure( SimulationConfiguration &configuration );

  //@} // End of Public Class Methods of ThreadedTimeWarpConfigurationManager

protected:
private:
};
#endif
