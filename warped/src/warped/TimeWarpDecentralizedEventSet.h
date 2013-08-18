#ifndef TIME_WARP_DECENTRALIZED_EVENT_SET_H
#define TIME_WARP_DECENTRALIZED_EVENT_SET_H


#include "warped.h"
#include "TimeWarpEventSet.h"

class Event;
class SimulationObject;

/** The TimeWarpDecentralizedEventSet abstract base class.

    This is the abstract base class for all Time Warp specific
    decentralized event set implementations in the
    kernel. DeCentralized EventSet implementations share one queue per
    simulation object in a simulation manager. Different Time Warp
    decentralized event set implementations must be derived from this
    abstract class.

*/
class TimeWarpDecentralizedEventSet : public TimeWarpEventSet {
public:

   /**@name Public Class Methods of TimeWarpDecentralizedEventSet. */
   //@{

   /// Virtual Destructor
   virtual ~TimeWarpDecentralizedEventSet(){};

   //@} // End of Public Class Methods of TimeWarpDecentralizedEventSet.

protected:
   /**@name Protected Class Methods of TimeWarpDecentralizedEventSet. */
   //@{

   /// Default Constructor
   TimeWarpDecentralizedEventSet() {};

   //@} // End of Protected Class Methods of TimeWarpDecentralizedEventSet.
};

#endif //ifdef TIME_WARP_DECENTRALIZED_EVENT_SET
