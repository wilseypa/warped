#ifndef UDP_SELECT_PHYSICAL_COMMUNICATION_LAYER_H
#define UDP_SELECT_PHYSICAL_COMMUNICATION_LAYER_H

#include "eclmplCommonInclude.h"
#include "PhysicalCommunicationLayer.h"
#include "UDPNetworkMessage.h"
#include "UDPConnectionInterface.h"

/// The UDPSelectPhysicalCommunicationLayer class.
class UDPSelectPhysicalCommunicationLayer : public PhysicalCommunicationLayer {
public:
    /**@name Public Class Methods of UDPSelectPhysicalCommunicationLayer. */
    //@{
    
    /// Default Constructor.
    UDPSelectPhysicalCommunicationLayer();
    
    // Disable Copy Constructor and Copy Assignment
    UDPSelectPhysicalCommunicationLayer(const UDPSelectPhysicalCommunicationLayer& original) = delete;
    UDPSelectPhysicalCommunicationLayer& operator=(const UDPSelectPhysicalCommunicationLayer& rhs) = delete;
    
    /// Destructor.
    virtual ~UDPSelectPhysicalCommunicationLayer();
    
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

    /** Probe to see if there are messages to retrieve.

        @return The retrieved message (NULL if no message).
    */
    virtual SerializedInstance* physicalProbeRecv();

    /// Clean up and call MPI_Finalize.
    virtual void physicalFinalize();

    /** Return how many processes are involved in the communicator.

        @return The number of processes involved in the communicator.
    */
    virtual unsigned int physicalGetSize() const;
    
    //@} // End of Public Class Methods of UDPSelectPhysicalCommunicationLayer.
    
protected:
    /**@name Protected Class Methods of UDPSelectPhysicalCommunicationLayer. */
    //@{
    virtual void probeNetwork();
    virtual void initializeCommunicationLayerAttributes();
    
    virtual void send(UDPNetworkMessage* const nwMsg, const unsigned int& dest);
    virtual bool insertInOrderMessage(UDPNetworkMessage* nwMsg);
    virtual void processNwMsgInfo(UDPNetworkMessage* nwMsg);
    virtual void updateOutOfOrderMessageQs(const unsigned int& source, const bool& sendRetransmissionReq);
    virtual unsigned int checkRetransmissionTimeout();
    virtual void sendAck(unsigned int dest);
    
    //@} // End of Protected Class Attributes of UDPSelectPhysicalCommunicationLayer.
    
    /**@name Protected Class Attributes of UDPSelectPhysicalCommunicationLayer. */
    //@{
    
    // UDP_SELECT:
    fd_set selectListenSet, fdSet;
    int maxFd;
    
    /// Simulation manager file descriptors for sends and receives.
    UDPConnectionInterface* connInterface;
    
    SequenceNumber* nextSendSequenceNumber;
    
    SequenceNumber* acknowledgedSequenceNumberReceived;
    // i.e. what is the highest sequence number that we have
    // sent on a link that has been acknowledged.
    
    SequenceNumber* acknowledgedSequenceNumberSent;
    // highest acknowledged sequence number sent.
    
    SequenceNumber* highestInSequenceNumberReceived;
    // i.e. highest sequence number that may be acknowlegded.
    
    SequenceNumber* highestInSequenceNumberProcessed;
    // i.e. highest sequence number given to the application.
    
    SequenceNumber* endOfHole;
    
    bool outOfOrderMessageQsAreEmpty;
    // outOfOrderMessageQs is a vector of priority queues
    std::vector<std::priority_queue<UDPNetworkMessage*, std::vector<UDPNetworkMessage*>, std::less<UDPNetworkMessage*>>> outOfOrderMessageQs;
    std::deque<UDPNetworkMessage*> inOrderMessageQ;
    std::vector<std::deque<UDPNetworkMessage*>> synchBuffer;
    std::vector<std::deque<UDPNetworkMessage*>> sendQs;

    unsigned int maxNrUnackedMsgs;
    
    std::chrono::steady_clock::time_point startTime;
    
    //@} // End of Protected Class Attributes of UDPSelectPhysicalCommunicationLayer.
};

#endif
