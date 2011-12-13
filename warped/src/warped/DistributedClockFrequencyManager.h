#ifndef DISTRIBUTED_CLOCK_FREQUENCY_MANAGER_H
#define DISTRIBUTED_CLOCK_FREQUENCY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
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
  DistributedClockFrequencyManager(TimeWarpSimulationManager* simMgr, int measurementPeriod, int numCPUs);

  /// Destructor
  virtual ~DistributedClockFrequencyManager() {}

  /// to be called from the simulation manager's simulation loop
  virtual void poll();

  virtual void registerWithCommunicationManager();

  /// from CommunicatingEntity
  virtual void receiveKernelMessage(KernelMessage* msg);

  // from Configurable
  virtual void configure(SimulationConfiguration &configuration);

  //@} // End of Public Class Methods of ClockFrequencyManager.

private:
  void adjustFrequency(double avgRollbacks, unsigned int min, unsigned int max);
  std::deque<unsigned int> myRollbacks;
  unsigned int myLastRollbacks;
  unsigned int myRba;
  int myFreqidx;

};
#endif //DISTRIBUTED_CLOCK_FREQUENCY_MANAGER_H
