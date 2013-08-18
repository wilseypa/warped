#ifndef SCHEDULING_MANAGER_H
#define SCHEDULING_MANAGER_H


#include "warped.h"
#include "Configurable.h"

class Event;
class SimulationObject;
class TimeWarpEventSet;
class SchedulingData;

/** The SchedulingManager abstract base class.

This is the abstract base class for all scheduler implementations in
the kernel. Different scheduler implementations must be derived from 
this abstract class.  

*/
class SchedulingManager : virtual public Configurable {
public:
  /**@name Public Class Methods of SchedulingManager. */
  //@{
   
  /// Virtual Destructor.
  virtual ~SchedulingManager(){};
  
  /**
     Ask the scheduler which event is next.
  */
  virtual const Event *peekNextEvent() = 0;

  /** Get the timestamp of the last event that was scheduled.

  This is a pure virtual function that has to be overriden.

  @return Timestamp of the last event that was scheduled for execution.
  */
  virtual const VTime &getLastEventScheduledTime() = 0;

  //@} // End of Public Class Methods of SchedulingManager.
protected:
  /**@name Protected Class Methods of SchedulingManager. */
  //@{

  /// Default Constructor.
  SchedulingManager() {};

  //@} // End of Protected Class Methods of SchedulingManager.
};

#endif
