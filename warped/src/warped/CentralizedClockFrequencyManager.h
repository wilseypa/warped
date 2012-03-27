#ifndef CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
#define CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
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
  CentralizedClockFrequencyManager(TimeWarpSimulationManager* simMgr, int measurementPeriod, int firsize, bool dummy);

  /// Destructor
  virtual ~CentralizedClockFrequencyManager() {}

  /// to be called from the simulation manager's simulation loop
  virtual void poll();

  virtual void registerWithCommunicationManager();

  /// from CommunicatingEntity
  virtual void receiveKernelMessage(KernelMessage* msg);

  virtual string toString();

  // from ClockFrequencyManagerImplementationBase
  //virtual bool checkMeasurementPeriod();

  //@} // End of Public Class Methods of ClockFrequencyManager.

private:

  std::vector<int> myLastFreqs;
  bool myStartedRound;
  bool myFirstTime;

  void adjustFrequencies(std::vector<double>&);

};



#endif //CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
