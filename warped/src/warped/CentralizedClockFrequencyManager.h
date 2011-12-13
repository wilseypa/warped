#ifndef CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
#define CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include <deque>
#include "ClockFrequencyManagerImplementationBase.h"

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

  struct Rollback {
    int CPU;
    int average;

    bool operator<(const Rollback& rhs) const { return average < rhs.average; }
  };

  std::vector<int> myLastRollbacks;
  std::vector<std::deque<int> > myRollbackFIR;
  std::vector<int> myAverageRollbacks;
  std::vector<Rollback> myRollbacks;
  bool myFirstTime;
  bool myStartedRound;
  bool myDoAdjust;
  int myRound;

  int variance(vector<int>&);
  int averageRollbacks(int lp);
  void updateRollbacks(std::vector<int>& rollbacks);
  void adjustFrequencies();

};
#endif //CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
