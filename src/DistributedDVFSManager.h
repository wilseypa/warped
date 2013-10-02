#ifndef DECENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
#define DECENTRALIZED_CLOCK_FREQUENCY_MANAGER_H


#include "DVFSManagerImplementationBase.h"
#include <deque>
#include <fstream>

class TimeWarpSimulationManager;

/** The SharedDVFSManager base class.

    Uses delay loops to simulate a system with finer DVFS frequency steps
*/
class DistributedDVFSManager : public DVFSManagerImplementationBase {
public:

    /**@name Public Class Methods of DistributedDVFSManager. */
    //@{

    /// Constructor
    DistributedDVFSManager(TimeWarpSimulationManager*, int, int, Algorithm, bool,
                           UsefulWorkMetric, double);

    /// Destructor
    virtual ~DistributedDVFSManager();

    /// to be called from the simulation manager's simulation loop
    virtual void poll();

    virtual void registerWithCommunicationManager();
    virtual void configure(SimulationConfiguration&);

    /// from CommunicatingEntity
    virtual void receiveKernelMessage(KernelMessage* msg);

    virtual string toString();

    //@} // End of Public Class Methods of DistributedDVFSManager.

private:
    void setFrequencies(int);
    void setGovernors(const char*);

    bool myWaitingForMessage;
};

#endif //DECENTRALIZED_CLOCK_FREQUENCY_MANAGER_H
