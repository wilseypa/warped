#ifndef DECENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
#define DECENTRALIZED_CLOCK_FREQUENCY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "DVFSManagerImplementationBase.h"
#include <deque>
#include <fstream>

class TimeWarpSimulationManager;

/** The SharedDVFSManager base class.

    Uses delay loops to simulate a system with finer DVFS frequency steps
*/
class DistributedDVFSManager : public DVFSManagerImplementationBase {
public:
   
  /**@name Public Class Methods of DistributedDVFSManager. */
  //@{

  /// Constructor
  DistributedDVFSManager(TimeWarpSimulationManager*, int, int, bool, bool,
                       OptimizationGoal, UsefulWorkMetric);

  /// Destructor
  virtual ~DistributedDVFSManager() {}

  /// to be called from the simulation manager's simulation loop
  virtual void poll();

  virtual void registerWithCommunicationManager();
  virtual void configure(SimulationConfiguration &);

  /// from CommunicatingEntity
  virtual void receiveKernelMessage(KernelMessage* msg);

  virtual string toString();

  //@} // End of Public Class Methods of DistributedDVFSManager.

private:
  void setFrequencies(int);
};

#endif //DECENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
