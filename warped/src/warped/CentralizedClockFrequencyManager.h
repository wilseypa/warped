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

  class FIRFilter {
  public: 
    FIRFilter(int size) 
      :myInput(0)
      ,mySize(size)
      ,myAverage(0) {}
    void update(int);
    int getAverage() { return myAverage; }
    int getLast() { return myInput.front(); }
    bool operator<(const FIRFilter& rhs) const { return myAverage < rhs.myAverage; }

  private:
    std::deque<int> myInput;
    int mySize;
    int myAverage;
  };

  std::vector<FIRFilter> myRollbackFilters;
  bool myFirstTime;
  bool myStartedRound;
  int myLastRollbacks;
  int myRound;

  int variance(vector<FIRFilter>&);
  void adjustFrequencies();

};



#endif //CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
