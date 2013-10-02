#ifndef AGGRESSIVE_OUTPUT_MANAGER_H
#define AGGRESSIVE_OUTPUT_MANAGER_H


#include <fstream>
#include "warped.h"
#include "OutputManagerImplementationBase.h"

class Event;
class SimulationObject;
class OutputManager;

/** The AggressiveOutputManager class.

    This class implements an aggressive cancellation scheme as a part of
    its output manager functionality.

*/
class AggressiveOutputManager : public OutputManagerImplementationBase {
public:

    /**@name Public Class Methods of AggressiveOutputManager. */
    //@{

    /** Constructor.

        @param simMgr Handle to the simulation manager.
    */
    AggressiveOutputManager(TimeWarpSimulationManager* simMgr);

    /// Destructor.
    ~AggressiveOutputManager();


    /** Send out anti-messages.

        @param rollbackTime Bound for how many antimessages will be sent.
        @param object A pointer to the object who experienced rollback.
    */
    void rollback(SimulationObject* object,  const VTime& rollbackTime);

    virtual void configure(SimulationConfiguration&) {}

    //@} // End of Public Class Methods of AggressiveOutputManager.

};

#endif

