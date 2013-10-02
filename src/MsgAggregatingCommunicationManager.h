#ifndef MSG_AGGREGATING_COMMUNICATION_MANAGER_H
#define MSG_AGGREGATING_COMMUNICATION_MANAGER_H


#include "warped.h"
#include "CommunicationManagerImplementationBase.h"
#include "MessageManager.h"

/** The MsgAggregatingCommunicationManager class.

As the name implies, this is the communication manager
implementation that uses the message aggregation optimization.
This class is derived from the CommunicationManager abstract base
class.

*/
class MsgAggregatingCommunicationManager : public CommunicationManagerImplementationBase {

public:

    /**@name Public Class Methods of MsgAggregatingCommunicationManager. */
    //@{

    /** Constructor.

    @param physicalLayer Handle to the phys. comm. mgr.
    */
    MsgAggregatingCommunicationManager(PhysicalCommunicationLayer* physicalLayer,
                                       TimeWarpSimulationManager* simMgr);

    /// Destructor.
    ~MsgAggregatingCommunicationManager();

    /// initialize the communication manager
    void initializeCommunicationManager();

    /** Send message.

    @param msg Message to send.
    */
    void sendMessage(KernelMessage* msg, unsigned int dest);

    /** Retrieve a message from the physical comm. layer.

    @return Retrieved message.
    */
    SerializedInstance* retrieveMessageFromPhysicalLayer();

    unsigned int checkPhysicalLayerForMessages(int maxNum);

    void incrementAgeOfMessage() {
        myMessageManager->incrementAgeOfMessage();
    }

    void setRollingBackFlagInMessageManager() {
        myMessageManager->setRollBackFlag();
    }

    void resetRollingBackFlagInMessageManager() {
        myMessageManager->resetRollBackFlag();
    }

    void flush() {
        myMessageManager->sendMessage();
    }

    void flushIfAgeExceeded() {
        myMessageManager->flushIfAgeExceeded();
    }

    void flushIfWaitedTooLong() {
        myMessageManager->flushIfWaitedTooLong(waitedWithNoInputMessages);
    }

    virtual void configure(SimulationConfiguration&) {}

private:

    MessageManager* myMessageManager;
    int waitedWithNoInputMessages;

    //@} // End of Public Class Methods of MsgAggregatingCommunicationManager
};

#endif
