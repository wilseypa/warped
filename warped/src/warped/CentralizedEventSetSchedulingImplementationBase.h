#ifndef CENTRALIZED_EVENTSET_SCHEDULING_IMPLEMENTATION_BASE_H
#define CENTRALIZED_EVENTSET_SCHEDULING_IMPLEMENTATION_BASE_H


#include "warped.h"
#include "CentralizedEventSetSchedulingManager.h"

class SimulationManager;
class SimulationObject;
class TimeWarpEventSetFactory;
class Event;

/** The CentralizedEventSetSchedulingImplementationBase class.

    This is the base class for all Centralized scheduler
    implementations in the kernel. Different scheduler implementations
    must be derived from this implementation base class.  This class
    aggregates methods and member data common to all scheduler
    implementations.

*/
class CentralizedEventSetSchedulingImplementationBase : public CentralizedEventSetSchedulingManager {
public:

  /**@name Public Class Methods of CentralizedEventSetSchedulingImplementationBase. */
  //@{

  /** Constructor.

      @param simMgr Pointer to the simulation manager.
  */  
  CentralizedEventSetSchedulingImplementationBase(SimulationManager *simMgr);
  
  /// Virtual Destructor
  virtual ~CentralizedEventSetSchedulingImplementationBase();
  
  /// Schedule a process for execution.
  virtual void runProcesses( const Event *event );
  
  /** Get the last event that was scheduled for execution.

      @return The last event scheduled for execution.
  */
  virtual const VTime& getLastEventScheduledTime();

  /** Get the next event to schedule for execution.

      @return Next event to schedule.
  */
  virtual const Event *getEventToSchedule();

  virtual void configure( SimulationConfiguration &configure ){}
   
  //@} // End of Public Class Methods of CentralizedEventSetSchedulingImplementationBase
};

#endif
