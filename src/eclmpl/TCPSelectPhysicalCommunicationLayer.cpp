#include "TCPSelectPhysicalCommunicationLayer.h"

TCPSelectPhysicalCommunicationLayer::TCPSelectPhysicalCommunicationLayer() : PhysicalCommunicationLayer(), maxFd(-1) {
    connInterface = new TCPConnectionInterface(ECLMPL_MTU);
    
    FD_ZERO(&selectListenSet);
    FD_ZERO(&fdSet);
    recvBuf = new char[MAXBUF];
} // End of default constructor.

TCPSelectPhysicalCommunicationLayer::~TCPSelectPhysicalCommunicationLayer() {
    delete connInterface;
    delete[] recvBuf;
    
    for (unsigned int i = 0; i < inOrderMessageQ.size(); i++) {
        delete inOrderMessageQ[i];
    }
} // End of desctructor.

void TCPSelectPhysicalCommunicationLayer::physicalInit() {
    std::vector<std::string> argv;
    argv.resize(1);
    argv[0] = string();

    connInterface->establishConnections(argv);
    
    if (physicalGetSize() > 1) {
        initializeCommunicationLayerAttributes();
    }
} // End of physicalInit().

unsigned int TCPSelectPhysicalCommunicationLayer::physicalGetId() const {
    return connInterface->getConnectionId();
} // End of physicalGetId().

void TCPSelectPhysicalCommunicationLayer::physicalSend(const SerializedInstance* toSend, unsigned int dest) {
    const void* buffer = &toSend->getData()[0];
    unsigned int size = toSend->getSize();
    unsigned int physicalSize = physicalGetSize();

    ECLMPL_ASSERT(buffer != NULL);
    ECLMPL_ASSERT(dest <= physicalSize);
    ECLMPL_ASSERT(size <= connInterface->getMTU());
    
    if (physicalSize > 1) {
        ECLMPL_ASSERT(dest != physicalGetId());
        connInterface->send(size, buffer, dest);
    }
    
    //toSend is created by serialize and needs to be deleted
    delete toSend;
} // End of physicalSend(...).

SerializedInstance* TCPSelectPhysicalCommunicationLayer::physicalProbeRecv() {
    // Only probe for new messages if the queue of received messages is empty
    if (connInterface->getNumberOfConnections() > 1 && inOrderMessageQ.empty() == true) {
        probeNetwork();
    }
    
    return getNextInSequence();
} // End of physicalProbeRecv().

void TCPSelectPhysicalCommunicationLayer::physicalFinalize() {
    connInterface->tearDownConnections();
} // End of physicalFinalize().

unsigned int TCPSelectPhysicalCommunicationLayer::physicalGetSize() const {
    return connInterface->getNumberOfConnections();
} // End of physicalGetSize().

void TCPSelectPhysicalCommunicationLayer::initializeCommunicationLayerAttributes() {
    unsigned int physicalSize = physicalGetSize();
    
    connInterface->disableNagle();
    maxFd = -1;
    for (unsigned int i = 0; i < physicalSize; i++) {
        if (connInterface->socket[i]->getSocketFd() > maxFd) {
            maxFd = connInterface->socket[i]->getSocketFd();
        }
        FD_SET(connInterface->socket[i]->getSocketFd(),
               &selectListenSet);
    }
    fdSet = selectListenSet;
    maxFd += 1; // Needed for call to select.
} // End of setupAttributes(...).

void TCPSelectPhysicalCommunicationLayer::probeNetwork() {
    unsigned int physicalSize = connInterface->getNumberOfConnections();
    unsigned int size = 0;
    TCPNetworkMessage* nwMsg;

    // Probe to see if any messages are available.
    timeval timeout;

    // NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
    // The effect of this timer will greatly impact performance. The effect is sometimes
    // not predictable. However, the effect seems to be machine (read Linux Pentium or
    // Solaris Sun) dependent. Try values of tv_sec = 0, and tv_usec = 0, 1, 100 for
    // starters.
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    // NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE

    selectListenSet = fdSet;
    select(maxFd, &selectListenSet, NULL, NULL, &timeout);
    for (unsigned int i = 0; i < physicalSize; i++) {
        if (connInterface->socket[i]->wFD_ISSET(&selectListenSet) == true) {
            size = MAXBUF;
            connInterface->recv(size, recvBuf, i);
            if (size > 0) {
                nwMsg = new TCPNetworkMessage(size, recvBuf);
                inOrderMessageQ.push_back(nwMsg);
            }
        } // End of if (recvSocket[i].wFD_ISSET(&selectListenSet) == true).
    } // End of for (int i = 0; i < physicalSize; i++).
} // End of TCP_probeNetwork().

SerializedInstance* TCPSelectPhysicalCommunicationLayer::getNextInSequence() {
    SerializedInstance* retval = NULL;

    TCPNetworkMessage* nwMsg;
    char* msg = NULL;
    unsigned int msgSize = 0;

    if (inOrderMessageQ.empty() == false) {
        nwMsg = inOrderMessageQ.front();
        inOrderMessageQ.pop_front();
        
        msgSize = nwMsg->getUserDataSize();
        msg = new char[msgSize];
        nwMsg->getUserData(msg, msgSize);
        
        retval = new SerializedInstance(msg, msgSize);
        delete nwMsg;
    } // End of if (inOrderMessageQIsEmpty == false).
    
    return retval;
} // End of getNextInSequence().
