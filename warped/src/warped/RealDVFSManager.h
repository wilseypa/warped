#ifndef CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
#define CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "DVFSManagerImplementationBase.h"

class TimeWarpSimulationManager;

/** The RealDVFSManager base class.

    Uses DVFS to set the frequencies of each core
*/
class RealDVFSManager : public DVFSManagerImplementationBase {
public:
   
  /**@name Public Class Methods of RealDVFSManager. */
  //@{

  /// Constructor
  RealDVFSManager(TimeWarpSimulationManager*, int, int, bool, bool,
                  OptimizationGoal, UsefulWorkMetric);

  /// Destructor
  virtual ~RealDVFSManager() {}

  /// to be called from the simulation manager's simulation loop
  virtual void poll();

  virtual void registerWithCommunicationManager();

  virtual void configure(SimulationConfiguration &);

  /// from CommunicatingEntity
  virtual void receiveKernelMessage(KernelMessage*);

  virtual string toString();

  //@} // End of Public Class Methods of DVFSManager.
};

#endif //CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
