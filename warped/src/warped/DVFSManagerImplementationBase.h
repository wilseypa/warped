#ifndef CLOCK_FREQUENCY_MANAGER_IMPLEMENTATION_BASE_H
#define CLOCK_FREQUENCY_MANAGER_IMPLEMENTATION_BASE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "DVFSManager.h"
#include "controlkit/FIRFilter.h"
#include "StopWatch.h"

class TimeWarpSimulationManager;
class CommunicationManager;

enum UsefulWorkMetric {
  UWM_ROLLBACKS,
  UWM_EFFICIENCY,
  UWM_EFFECTIVE_UTILIZATION,
  UWM_NONE
};

/** The DVFSManagerImplementationBase implementation base class.

    This contains the default implementations of DVFS functions
*/
class DVFSManagerImplementationBase : public DVFSManager {
public:
   
  /**@name Public Class Methods of DVFSManagerImplementationBase. */
  //@{

  /// Constructor
  DVFSManagerImplementationBase(TimeWarpSimulationManager*,
                                int, int, bool, bool, UsefulWorkMetric);

  /// Destructor
  virtual ~DVFSManagerImplementationBase();

  /// to be called from the simulation manager's simulation loop
  virtual void poll();

  /// from Configurable
  virtual void configure(SimulationConfiguration &configuration);

  void resetMeasurementCounter() { myMeasurementCounter = 0;}

  virtual void delay(int) {}

  //@} // End of Public Class Methods of DVFSManagerImplementationBase.

protected:
   
  /**@name Protected Class Methods of DVFSManagerImplementationBase. */
  //@{

  /// Determines whether a measurement cycle has lapsed
  virtual bool checkMeasurementPeriod();

  virtual bool isMaster() { return mySimulationManagerID == 0; }

  virtual void writeCSVRow(int, double, int);

  int getPeriod() { return myMeasurementPeriod; }

  void populateAvailableFrequencies();

  void setCPUFrequency(int cpu_idx, int freq);

  bool updateFrequencyIdxs();

  void fillUsefulWork(vector<double>&);

  //@} // End of Protected Class Methods of DVFSManagerImplementationBase.

  TimeWarpSimulationManager* mySimulationManager;
  CommunicationManager* myCommunicationManager;
  int mySimulationManagerID;
  int myNumSimulationManagers;
  int myCPU;
  int myFIRSize;
  bool myIsDummy;
  std::vector<FIRFilter<double> > myUtilFilters;

  // begin() == fastest, end() == slowest
  std::vector<int> myAvailableFreqs;
  vector<int> myFrequencyIdxs;
  int myMaxFreqIdx;

private:
  int myMeasurementPeriod;
  StopWatch myStopwatch;
  int myMeasurementCounter;
  bool myPowerSave;
  UsefulWorkMetric myUWM;

  void setGovernorMode(const char* governor);

};


#endif //CLOCK_FREQUENCY_MANAGER_IMPLEMENTATION_BASE_H
