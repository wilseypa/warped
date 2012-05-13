#ifndef CLOCK_FREQUENCY_MANAGER_H
#define CLOCK_FREQUENCY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "CommunicatingEntity.h"
#include "Configurable.h"

/** The DVFSManager abstract base class.

    This is the abstract base class for the various Frequency control implementations
    in the simulation kernel.
*/
class DVFSManager : public CommunicatingEntity,
		   public Configurable {
public:

  // Rollbacks: simple rollback counter
  // RollbackFraction: 1 - (events rolled back) / (events executed)
  // EffectiveUtilization: cpu time not rolled back / total cpu time
  enum UsefulWorkMetric {
    ROLLBACKS,
    ROLLBACK_FRACTION,
    EFFECTIVE_UTILIZATION
  };

  // Fixed:       leave cores set at middle frequency
  // Performance: scaling freq down on one core always corresponds to
  //              scaling up on another core
  // Power:       only scale down, never scale up
  // Hybrid:      scale down nodes with below-average useful work, then
  //              scale up a node with above-average useful work, if one exists
  enum Algorithm {
    FIXED,
    PERFORMANCE,
    POWER,
    HYBRID
  };
 
  /**@name Public Class Methods of DVFSManager. */
  //@{

  /// Destructor
  virtual ~DVFSManager(){}

  /// to be called from the simulation manager's simulation loop
  virtual void poll() = 0;

  // each manager should implement this to register its own type of kernel message
  virtual void registerWithCommunicationManager() = 0;

  virtual string toString() = 0;

  // delay by amount necessary to simulate different clock frequency
  // using clock cycles
  virtual void delay(int) = 0;

  virtual bool doEffectiveUtilization() = 0;

  //@} // End of Public Class Methods of DVFSManager.

protected:
   
  /**@name Protected Class Methods of DVFSManager. */
  //@{
   
  /// Constructor
  DVFSManager(){}

  /// Determines whether a measurement cycle has lapsed
  virtual bool checkMeasurementPeriod() = 0;
   
  /// Determines whether this manager initiates communication with other nodes
  virtual bool isMaster() = 0;

  virtual void writeCSVRow(int, double, int) = 0;


  //@} // End of Protected Class Methods of GVTManager.
};

ostream& operator<<(ostream& out, DVFSManager& cfm);

#endif //CLOCK_FREQUENCY_MANAGER_H

