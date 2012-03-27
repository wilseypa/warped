#ifndef CLOCK_FREQUENCY_MANAGER_IMPLEMENTATION_BASE_H
#define CLOCK_FREQUENCY_MANAGER_IMPLEMENTATION_BASE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "ClockFrequencyManager.h"
#include "controlkit/FIRFilter.h"

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
  ClockFrequencyManagerImplementationBase(TimeWarpSimulationManager* simMgr, int measurementPeriod, int firsize, bool dummy);

  /// Destructor
  virtual ~ClockFrequencyManagerImplementationBase();

  /// to be called from the simulation manager's simulation loop
  virtual void poll();

  /// from Configurable
  virtual void configure(SimulationConfiguration &configuration);

  void resetMeasurementCounter() { myMeasurementCounter = 0;}

  virtual void delay(int) {}

  //@} // End of Public Class Methods of ClockFrequencyManager.

protected:
   
  /**@name Protected Class Methods of ClockFrequencyManager. */
  //@{

  /// Determines whether a measurement cycle has lapsed
  virtual bool checkMeasurementPeriod();

  virtual bool isMaster() { return myAmMaster; }
  void setMaster(bool m) { myAmMaster = m; }

  virtual void writeCSVRow(int node, int avgRollbacks, int currentRollbacks, int freq, int hystlow,
                           int hysthigh);

  int getPeriod() { return myMeasurementPeriod; }

  void populateAvailableFrequencies();

  void setCPUFrequency(int cpu_idx, int freq);

  //@} // End of Protected Class Methods of GVTManager.

  TimeWarpSimulationManager* mySimulationManager;
  CommunicationManager* myCommunicationManager;
  int mySimulationManagerID;
  int myNumSimulationManagers;
  int myCPU;
  int myFIRSize;
  int myRound;
  bool myIsDummy;
  std::vector<FIRFilter<double> > myUtilFilters;

  // begin() == fastest, end() == slowest
  std::vector<int> myAvailableFreqs;

private:

  int myMeasurementPeriod;
  int myMeasurementCounter;
  bool myAmMaster;

  void setGovernorMode(const char* governor);

};


#endif //CLOCK_FREQUENCY_MANAGER_IMPLEMENTATION_BASE_H
