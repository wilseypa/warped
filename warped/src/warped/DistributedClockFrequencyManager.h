#ifndef DISTRIBUTED_CLOCK_FREQUENCY_MANAGER_H
#define DISTRIBUTED_CLOCK_FREQUENCY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <deque>
#include "ClockFrequencyManagerImplementationBase.h"

class TimeWarpSimulationManager;

/** The CentralizedClockFrequencyManager base class.

    Controls the frequencies of each process from a single simulation manager
*/
class DistributedClockFrequencyManager : public ClockFrequencyManagerImplementationBase {
public:
   
  /**@name Public Class Methods of CentralizedClockFrequencyManager. */
  //@{

  /// Constructor
  DistributedClockFrequencyManager(TimeWarpSimulationManager* simMgr, int measurementPeriod, int numCPUs, int firsize, bool dummy);

  /// Destructor
  virtual ~DistributedClockFrequencyManager() {}

  /// to be called from the simulation manager's simulation loop
  virtual void poll();

  virtual void registerWithCommunicationManager();

  /// from CommunicatingEntity
  virtual void receiveKernelMessage(KernelMessage* msg);

  // from Configurable
  virtual void configure(SimulationConfiguration &configuration);

  virtual string toString();

  virtual int getNominalDelay() { return myAvailableDelays[(myNumAvailableDelays - 1) / 2]; }

  //@} // End of Public Class Methods of ClockFrequencyManager.

  static const int myAvailableDelays[];
private:
  int myNumAvailableDelays;

  void adjustFrequency(std::vector<int>& rollbacks);

};

#endif //DISTRIBUTED_CLOCK_FREQUENCY_MANAGER_H
