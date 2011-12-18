#ifndef CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
#define CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include <deque>
#include "ClockFrequencyManagerImplementationBase.h"
#include "controlkit/FIRFilter.h"

class TimeWarpSimulationManager;

/** The CentralizedClockFrequencyManager base class.

    Controls the frequencies of each process from a single simulation manager
*/
class CentralizedClockFrequencyManager : public ClockFrequencyManagerImplementationBase {
public:
   
  /**@name Public Class Methods of CentralizedClockFrequencyManager. */
  //@{

  /// Constructor
  CentralizedClockFrequencyManager(TimeWarpSimulationManager* simMgr, int measurementPeriod, int numCPUs);

  /// Destructor
  virtual ~CentralizedClockFrequencyManager() {}

  /// to be called from the simulation manager's simulation loop
  virtual void poll();

  virtual void registerWithCommunicationManager();

  /// from CommunicatingEntity
  virtual void receiveKernelMessage(KernelMessage* msg);

  // from Configurable
  virtual void configure(SimulationConfiguration& configuration);

  // from ClockFrequencyManagerImplementationBase
  //virtual bool checkMeasurementPeriod();

  //@} // End of Public Class Methods of ClockFrequencyManager.

private:

  std::vector<FIRFilter<int> > myRollbackFilters;
  bool myFirstTime;
  bool myStartedRound;
  int myLastRollbacks;
  int myRound;

  //int variance(vector<FIRFilter>&);
  void adjustFrequencies(std::vector<int>&);

};



#endif //CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
