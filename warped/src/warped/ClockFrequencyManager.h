#ifndef CLOCK_FREQUENCY_MANAGER_H
#define CLOCK_FREQUENCY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "CommunicatingEntity.h"
#include "Configurable.h"

/** The ClockFrequencyManager abstract base class.

    This is the abstract base class for the various Frequency control implementations
    in the simulation kernel.
*/
class ClockFrequencyManager : public CommunicatingEntity,
		   public Configurable {
public:
   
  /**@name Public Class Methods of ClockFrequencyManager. */
  //@{

  /// Destructor
  virtual ~ClockFrequencyManager(){}

  /// to be called from the simulation manager's simulation loop
  virtual void poll() = 0;

  // each manager should implement this to register its own type of kernel message
  virtual void registerWithCommunicationManager() = 0;

  virtual string toString() = 0;

  // delay by amount necessary to simulate different clock frequency
  // using clock cycles
  virtual void delay(int) = 0;

  //@} // End of Public Class Methods of ClockFrequencyManager.

protected:
   
  /**@name Protected Class Methods of ClockFrequencyManager. */
  //@{
   
  /// Constructor
  ClockFrequencyManager(){}

  /// Determines whether a measurement cycle has lapsed
  virtual bool checkMeasurementPeriod() = 0;
   
  /// Determines whether this manager initiates communication with other nodes
  virtual bool isMaster() = 0;

  virtual void writeCSVRow(int node, int avgRollbacks, int currentRollbacks, int freq, int hystlow, int hysthigh) = 0;


  //@} // End of Protected Class Methods of GVTManager.
};

ostream& operator<<(ostream& out, ClockFrequencyManager& cfm);

#endif //CLOCK_FREQUENCY_MANAGER_H

