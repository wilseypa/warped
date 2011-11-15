#ifndef CLOCK_FREQUENCY_MANAGER_IMPLEMENTATION_BASE_H
#define CLOCK_FREQUENCY_MANAGER_IMPLEMENTATION_BASE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "ClockFrequencyManager.h"

class TimeWarpSimulationManager;
class CommunicationManager;

/** The ClockFrequencyManager abstract base class.

    This is the abstract base class for the various Frequency control implementations
    in the simulation kernel.
*/
class ClockFrequencyManagerImplementationBase : public ClockFrequencyManager {
public:
   
  /**@name Public Class Methods of ClockFrequencyManager. */
  //@{

  /// Constructor
  ClockFrequencyManagerImplementationBase(TimeWarpSimulationManager* simMgr, int measurementPeriod, int numCPUs);

  /// Destructor
  virtual ~ClockFrequencyManagerImplementationBase();

  /// to be called from the simulation manager's simulation loop
  virtual void poll();

  /// from Configurable
  virtual void configure(SimulationConfiguration &configuration);

  //@} // End of Public Class Methods of ClockFrequencyManager.

protected:
   
  /**@name Protected Class Methods of ClockFrequencyManager. */
  //@{

  /// Determines whether a measurement cycle has lapsed
  virtual bool checkMeasurementPeriod();

  virtual bool iAmMaster() { return mySimulationManagerID == 0; }

  void populateAvailableFrequencies();

  void setCPUFrequency(int cpu_idx, const char* freq);

  //@} // End of Protected Class Methods of GVTManager.

  TimeWarpSimulationManager* mySimulationManager;
  CommunicationManager* myCommunicationManager;
  int mySimulationManagerID;
  int myNumSimulationManagers;
  int myNumCPUs;
  int myCPU;

  // begin() == fastest, end() == slowest
  std::vector<string> myAvailableFreqs;

private:

  int myMeasurementPeriod;
  int myMeasurementCounter;

  void setGovernorMode(const char* governor);

};
#endif //CLOCK_FREQUENCY_MANAGER_IMPLEMENTATION_BASE_H
