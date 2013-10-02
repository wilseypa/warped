#ifndef TIME_WARP_MULTI_SET_ONE_ANTI_MSG_H
#define TIME_WARP_MULTI_SET_ONE_ANTI_MSG_H


#include "TimeWarpMultiSet.h"

/** The TimeWarpMultiSetOneAntiMsg class.

    The same as the TimeWarpMultiSet class except that it
    handles the one anti-message optimization.
*/
class TimeWarpMultiSetOneAntiMsg : public TimeWarpMultiSet {
public:

    /**@name Public Class Methods of TimeWarpMultiSetOneAntiMsg. */
    //@{

    /// Default Constructor.
    TimeWarpMultiSetOneAntiMsg(TimeWarpSimulationManager* initSimulationManager);

    /// Destructor.
    virtual ~TimeWarpMultiSetOneAntiMsg();

    /** Cancel out positive message corresponding to the anti message.

    @param cancelEvent The antimessage.
    @param object The object who receives the antimessage.
    */
    virtual bool handleAntiMessage(SimulationObject* object,
                                   const NegativeEvent* cancelEvent);
};

#endif
