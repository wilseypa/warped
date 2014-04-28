#ifndef TCP_SELECT_PHYSICAL_COMMUNICATION_LAYER_H
#define TCP_SELECT_PHYSICAL_COMMUNICATION_LAYER_H

#include "eclmplCommonInclude.h"
#include "TCPNetworkMessage.h"
#include "PhysicalCommunicationLayer.h"
#include "TCPConnectionInterface.h"

/** The TCPSelectPhysicalCommunicationLayer class.

*/
class TCPSelectPhysicalCommunicationLayer : public PhysicalCommunicationLayer {
public:
    /**@name Public Class Methods of TCPSelectPhysicalCommunicationLayer. */
    //@{

    /// Default Constructor.
    TCPSelectPhysicalCommunicationLayer();
    
    // Disable Copy Constructor and Copy Assignment
    TCPSelectPhysicalCommunicationLayer(const TCPSelectPhysicalCommunicationLayer& original) = delete;
    TCPSelectPhysicalCommunicationLayer& operator=(const TCPSelectPhysicalCommunicationLayer& rhs) = delete;

    /// Destructor.
    virtual ~TCPSelectPhysicalCommunicationLayer();
    
    /** Init physical layer.
    */
    virtual void physicalInit();

    /** Get the Id of the simulation manager.

        @return Id of the simulation manager.
    */
    virtual unsigned int physicalGetId() const;

    /** Send data.

        @param toSend Serialized instance to send.
        @param dest Destination to send to.
    */
    virtual void physicalSend(const SerializedInstance* toSend, unsigned int dest);

    /** Check the probe to see if there are messages to retrieve.

        @return The retrieved message (NULL if no message).
    */
    virtual SerializedInstance* physicalProbeRecv();

    /// Clean up and call MPI_Finalize.
    virtual void physicalFinalize();

    /** Return how many processes are involved in the communicator.

        @return The number of processes involved in the communicator.
    */
    virtual unsigned int physicalGetSize() const;

    //@} // End of Public Class Methods of TCPSelectPhysicalCommunicationLayer.

protected:
    /**@name Protected Class Methods of TCPSelectPhysicalCommunicationLayer. */
    //@{

    virtual void initializeCommunicationLayerAttributes();
    virtual void probeNetwork();
    virtual SerializedInstance* getNextInSequence();

    //@} // End of Protected Class Attributes of TCPSelectPhysicalCommunicationLayer.

    /**@name Protected Class Attributes of TCPSelectPhysicalCommunicationLayer. */
    //@{

    // For calls to select.
    fd_set selectListenSet, fdSet;
    int maxFd;
    char* recvBuf;
    
    /// Simulation manager file descriptors for sends and receives.
    TCPConnectionInterface* connInterface;
    
    std::deque<TCPNetworkMessage*> inOrderMessageQ;

    //@} // End of Protected Class Attributes of TCPSelectPhysicalCommunicationLayer.
};

#endif
