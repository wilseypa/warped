#ifndef CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
#define CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "DVFSManagerImplementationBase.h"

class TimeWarpSimulationManager;

/** The SharedDVFSManager base class.

    Uses DVFS to set the frequencies of each core
*/
class SharedDVFSManager : public DVFSManagerImplementationBase {
public:
   
  /**@name Public Class Methods of SharedDVFSManager. */
  //@{

  /// Constructor
  SharedDVFSManager(TimeWarpSimulationManager*, int, int, bool, bool,
                  OptimizationGoal, UsefulWorkMetric);

  /// Destructor
  virtual ~SharedDVFSManager();

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
