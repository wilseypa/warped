#ifndef TIME_WARP_CENTRALIZED_EVENT_SET_H
#define TIME_WARP_CENTRALIZED_EVENT_SET_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "TimeWarpEventSet.h"

class Event;
class SimulationObject;

/** The TimeWarpCentralizedEventSet abstract base class.

    This is the abstract base class for all Time Warp specific
    centralized event set implementations in the kernel. Centralized
    EventSet implementations share a single queue across all
    simulation objects in a simulation manager. Different Time Warp
    centralized event set implementations must be derived from this
    abstract class.

*/
class TimeWarpCentralizedEventSet : public TimeWarpEventSet {
public:
  /**@name Public Class Methods of TimeWarpCentralizedEventSet. */
  //@{
   
  /// Virtual Destructor
  virtual ~TimeWarpCentralizedEventSet(){};

  //@} // End of Public Class Methods of TimeWarpCentralizedEventSet.

protected:

  /**@name Protected Class Methods of TimeWarpCentralizedEventSet. */
  //@{

  /// Default Constructor
  TimeWarpCentralizedEventSet() {};

  //@} // End of Protected Class Methods of TimeWarpCentralizedEventSet.
};

#endif 
