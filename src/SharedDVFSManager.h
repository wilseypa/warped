#ifndef CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
#define CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H


#include <string>                       // for string
#include <vector>                       // for vector

#include "DVFSManager.h"                // for DVFSManager::Algorithm, etc
#include "DVFSManagerImplementationBase.h"
#include "warped.h"
using std::string;

class KernelMessage;
class SimulationConfiguration;
class TimeWarpSimulationManager;

/** The SharedDVFSManager base class.

    Uses DVFS to set the frequencies of each core
*/
class SharedDVFSManager : public DVFSManagerImplementationBase {
public:

    /**@name Public Class Methods of SharedDVFSManager. */
    //@{

    /// Constructor
    SharedDVFSManager(TimeWarpSimulationManager*, int, int, Algorithm, bool,
                      UsefulWorkMetric, double);

    /// Destructor
    virtual ~SharedDVFSManager();

    /// to be called from the simulation manager's simulation loop
    virtual void poll();

    virtual void registerWithCommunicationManager();

    virtual void configure(SimulationConfiguration&);

    /// from CommunicatingEntity
    virtual void receiveKernelMessage(KernelMessage*);

    virtual string toString();

    //@} // End of Public Class Methods of DVFSManager.

private:
    std::vector<int>myCPUs;
};

#endif //CENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
