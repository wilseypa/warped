#ifndef DECENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
#define DECENTRALIZED_CLOCK_FREQUENCY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <deque>
#include "ClockFrequencyManagerImplementationBase.h"

class TimeWarpSimulationManager;

/** The CentralizedClockFrequencyManager base class.

    Controls the frequencies of each process from a single simulation manager
*/
class DecentralizedClockFrequencyManager : public ClockFrequencyManagerImplementationBase {
public:
   
  /**@name Public Class Methods of CentralizedClockFrequencyManager. */
  //@{

  /// Constructor
  DecentralizedClockFrequencyManager(TimeWarpSimulationManager* simMgr, int measurementPeriod, int firsize, bool dummy);

  /// Destructor
  virtual ~DecentralizedClockFrequencyManager() {}

  /// to be called from the simulation manager's simulation loop
  virtual void poll();

  virtual void registerWithCommunicationManager();

  /// from CommunicatingEntity
  virtual void receiveKernelMessage(KernelMessage* msg);

  virtual string toString();

  virtual void delay(int);

  //@} // End of Public Class Methods of ClockFrequencyManager.

  static const int simulatedFrequencies[];

private:
  int mySimulatedFrequencyIdx;
  void adjustFrequency();

};

#endif //DECENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
