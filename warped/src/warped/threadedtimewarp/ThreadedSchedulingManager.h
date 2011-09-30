#ifndef THREADED_SCHEDULING_MANAGER_H
#define THREADED_SCHEDULING_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "SchedulingManager.h"

class SimulationObject;
class ThreadedTimeWarpSimulationManager;
class ThreadedTimeWarpEventSet;

/** The ThreadedSchedulingManager */
class ThreadedSchedulingManager :  public SchedulingManager {
public:
  ThreadedSchedulingManager( ThreadedTimeWarpSimulationManager *initSimulationManager );
  ~ThreadedSchedulingManager();
  
  const Event *peekNextEvent();

  const VTime &getLastEventScheduledTime();

  void configure(SimulationConfiguration &);

private:
  void setLastScheduledEventTime( const VTime *newTime );

  ThreadedTimeWarpSimulationManager *mySimulationManager;
  ThreadedTimeWarpEventSet *myThreadedEventSet;
  const VTime *lastScheduledTime;
};

#endif
