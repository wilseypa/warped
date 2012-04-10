#ifndef DECENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
#define DECENTRALIZED_CLOCK_FREQUENCY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "DVFSManagerImplementationBase.h"
#include <deque>
#include <fstream>

class TimeWarpSimulationManager;

/** The RealDVFSManager base class.

    Uses delay loops to simulate a system with finer DVFS frequency steps
*/
class SimulatedDVFSManager : public DVFSManagerImplementationBase {
public:
   
  /**@name Public Class Methods of SimulatedDVFSManager. */
  //@{

  /// Constructor
  SimulatedDVFSManager(TimeWarpSimulationManager*, int, int, bool, bool,
                       UsefulWorkMetric);

  /// Destructor
  virtual ~SimulatedDVFSManager() {}

  /// to be called from the simulation manager's simulation loop
  virtual void poll();

  virtual void registerWithCommunicationManager();
  virtual void configure(SimulationConfiguration &);

  /// from CommunicatingEntity
  virtual void receiveKernelMessage(KernelMessage* msg);

  virtual string toString();

  virtual void delay(int);

  //@} // End of Public Class Methods of SimulatedDVFSManager.

private:
  int mySimulatedFrequencyIdx;
  static const int simulatedFrequencies[];
  static const int numSimulatedFrequencies;
};

#endif //DECENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
