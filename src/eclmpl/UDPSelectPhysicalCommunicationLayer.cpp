#include "UDPSelectPhysicalCommunicationLayer.h"
#include "eclmplSocket.h"
#define RETRANSMISSION_TIMEOUT 1.0 // seconds
#define SEND_EXPLICIT_ACK_UPON_RECEIVING_DUPLICATE true

UDPSelectPhysicalCommunicationLayer::UDPSelectPhysicalCommunicationLayer() : PhysicalCommunicationLayer(), maxFd(-1), outOfOrderMessageQsAreEmpty(true), maxNrUnackedMsgs(100) {
    connInterface = new UDPConnectionInterface(ECLMPL_MTU);
    
    nextSendSequenceNumber = NULL;
    acknowledgedSequenceNumberReceived = NULL;
    acknowledgedSequenceNumberSent = NULL;
    highestInSequenceNumberReceived = NULL;
    highestInSequenceNumberReceived = NULL;
    endOfHole = NULL;
    
    FD_ZERO(&selectListenSet);
    FD_ZERO(&fdSet);
} // End of default constructor.

UDPSelectPhysicalCommunicationLayer::~UDPSelectPhysicalCommunicationLayer() {
    UDPNetworkMessage* tmpMsg;
    
    delete connInterface;
    delete[] nextSendSequenceNumber;
    delete[] acknowledgedSequenceNumberReceived;
    delete[] acknowledgedSequenceNumberSent;
    delete[] highestInSequenceNumberReceived;
    delete[] highestInSequenceNumberProcessed;
    delete[] endOfHole;
    
    for(unsigned int i = 0; i < outOfOrderMessageQs.size(); i++) {
        // delete allocated memory from outOfOrderMessageQs
        while( outOfOrderMessageQs[i].empty() == false ) {
            tmpMsg = outOfOrderMessageQs[i].top();
            outOfOrderMessageQs[i].pop();
            delete tmpMsg;
        }
    }
    
    // delete allocated memory from inOrderMessageQ
    for(unsigned int i = 0; i < inOrderMessageQ.size(); i++) {
        delete inOrderMessageQ[i];
    }
    
    // delete allocated memory from the deques in synchBuffer
    for(unsigned int i = 0; i < synchBuffer.size(); i++) {
        for(unsigned int j = 0; j < synchBuffer[i].size(); j++) {
            delete (synchBuffer[i][j]);
        }
    }
    
    // delete allocated memory from the deques in sendQs
    for(unsigned int i = 0; i < sendQs.size(); i++) {
        for(unsigned int j = 0; j < sendQs[i].size(); j++) {
            delete (sendQs[i][j]);
        }
    }

} // End of desctructor.

void UDPSelectPhysicalCommunicationLayer::physicalInit() {
    std::vector<std::string> argv;
    argv.resize(1);
    argv[0] = string();

    connInterface->establishConnections(argv);
    
    if(physicalGetSize() > 1) {
        initializeCommunicationLayerAttributes();
        
        // Start the retransmission timer
        startTime = std::chrono::steady_clock::now();
    }
} // End of physicalInit().

unsigned int UDPSelectPhysicalCommunicationLayer::physicalGetId() const {
    return connInterface->getConnectionId();
} // End of physicalGetId().

void UDPSelectPhysicalCommunicationLayer::physicalSend(const SerializedInstance* toSend, unsigned int dest) {
    const unsigned int size = toSend->getSize();
    char* newBuf = new char[size];
    std::copy_n(toSend->getData().begin(), size, newBuf);
    
    ASSERT(dest <= physicalGetSize());
    ASSERT(dest != physicalGetId());
    
    // We piggy-back acknowledgements information for every message we send.
    acknowledgedSequenceNumberSent[dest] = highestInSequenceNumberReceived[dest];
    
    UDPNetworkMessage* nwMsg = new UDPNetworkMessage(APPLICATION, physicalGetId(), true, true, highestInSequenceNumberReceived[dest], 0, nextSendSequenceNumber[dest], size, newBuf);
    nextSendSequenceNumber[dest]++;
    sendQs[dest].push_back(nwMsg);
    
    // Send the message
    send(nwMsg, dest);
    
    delete newBuf;
    //toSend is created by serialize and needs to be deleted
    delete toSend;
} // End of physicalSend(...).

SerializedInstance* UDPSelectPhysicalCommunicationLayer::physicalProbeRecv() {
    SerializedInstance* retval = NULL;
    UDPNetworkMessage* nwMsg = NULL;
    
    // Only probe for new messages if the queue of received messages is empty
    if(inOrderMessageQ.empty() == true) {
        checkRetransmissionTimeout();
        probeNetwork();
    }
    
    // Get the next message in order
    if (inOrderMessageQ.empty() == false) {
        nwMsg = inOrderMessageQ.front();
        inOrderMessageQ.pop_front();
        
        unsigned int newSize = nwMsg->getUserDataSize();
        char* newData = new char[newSize];
        nwMsg->getUserData(newData, newSize);
        
        retval = new SerializedInstance(newData, newSize);
        
        delete[] newData;
        delete nwMsg;
    } // End of if (inOrderMessageQ.empty() == false).
    return retval;
    
} // End of physicalProbeRecv().

void UDPSelectPhysicalCommunicationLayer::physicalFinalize() {
    connInterface->tearDownConnections();
} // End of physicalFinalize().

unsigned int UDPSelectPhysicalCommunicationLayer::physicalGetSize() const {
    return connInterface->getNumberOfConnections();
} // End of physicalGetSize().

void UDPSelectPhysicalCommunicationLayer::probeNetwork() {
    char recvBuf[MAXBUF];
    unsigned int recvBufSize;
    int size = 0;
    unsigned int source = 0;
    unsigned int physicalId = physicalGetId();
    unsigned int physicalSize = physicalGetSize();
    UDPNetworkMessage* nwMsg;
    SequenceNumber msgSqNr;
    bool sendRetransmissionReq;
    bool discardDuplicate = false;
    bool foundDuplicate = false;

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
        foundDuplicate = false;
        sendRetransmissionReq = false;
        if (connInterface->recvSocket[i]->wFD_ISSET(&selectListenSet) == true) {
            size = 0;
            while (size != -1) {
                recvBufSize = MAXBUF;
                source = i;
                connInterface->recv(recvBufSize, recvBuf, source);
                size = (int)recvBufSize;
                nwMsg = new UDPNetworkMessage();
                nwMsg->deserialize(recvBuf, size);
                source = nwMsg->getSourceId();

                if (nwMsg->getType() == APPLICATION) {
                    if (nwMsg->getUserDataFlag() == true) {
                        msgSqNr = nwMsg->getSequenceNr();
                        if (msgSqNr == highestInSequenceNumberReceived[source]+1) {
                            insertInOrderMessage(nwMsg);
                        }
                        else if (msgSqNr <= highestInSequenceNumberReceived[source]) {
                            foundDuplicate = true;
                            discardDuplicate = true;
                        }
                        else {
                            outOfOrderMessageQs[source].push(nwMsg);
                            outOfOrderMessageQsAreEmpty = false;
                            sendRetransmissionReq = true;
                        }
                    } // End of if (nwMsg->getUserDataFlag() == true).

                    if (discardDuplicate == false) {
                        processNwMsgInfo(nwMsg);
                    } // End of if (discardDuplicate == false).
                    else {
                        // Discard duplicate message.
                        delete nwMsg;
                        discardDuplicate = false;
                    }
                } // End of if (nwMsg->getType() == APPLICATION).
                else if (nwMsg->getType() == CONTROL) {
                    processNwMsgInfo(nwMsg);
                }
                else {
                    // Must be init or finalize message or something else.
                    synchBuffer[source].push_back(nwMsg);
                }

                // CHECK TO SEE OF THERE IS ANYTHING IN THE INPUT QUEUE OF source
                // BEFORE YELLING "WOLF" AND REQUESTING A RE TRANSMISSION OF MESSAGES!
                if (sendRetransmissionReq == true || (foundDuplicate && SEND_EXPLICIT_ACK_UPON_RECEIVING_DUPLICATE)) {
                    // Peek socket and see if anything more is there.
                    size = connInterface->recvSocket[i]->peek(recvBuf, 1);
                }
                else {
                    size = -1; // recvSocket[i].peek(recvBuf, 1);
                }
            } // End of while (size != -1).

            // Any new message(s) from source may cause the
            // outOfOrderMessageQ to need to be updated.
            if (outOfOrderMessageQs[source].empty() == false) {
                ECLMPL_DEBUG(std::cerr << physicalId << ": NEW OUT OF ORDER MESSAGES IN oooQ[" << source <<"]" << std::endl;)
                updateOutOfOrderMessageQs(source, sendRetransmissionReq);
            }

            // We must send an explicit acknowledgement if the number
            // of unacknowledged messages for a certain peer exceeds
            // maxNrUnackedMsgs.
            if ((highestInSequenceNumberReceived[source]-acknowledgedSequenceNumberSent[source]) > maxNrUnackedMsgs) {
                sendAck(source);
            }
        } // End of if (recvSocket[i].wFD_ISSET(&selectListenSet) == true).

        //  N  O  T  E          N  O  T  E          N  O  T  E
        //  N  O  T  E          N  O  T  E          N  O  T  E
        //  N  O  T  E          N  O  T  E          N  O  T  E
        // We should quite possibly send an explicit ack here if we receive
        // duplicates. It's possible that someone sends us very few messages
        // without us sending anything back. This would never trigger the
        // explicit ack due to maxNrUnackedMsgs being received.
        if (foundDuplicate && SEND_EXPLICIT_ACK_UPON_RECEIVING_DUPLICATE) {
            sendAck(source);
            ECLMPL_DEBUG(std::cerr << physicalId << ": Sent expl. ack upon receiving DUPLICATE from " << source << std::endl;)
        }
    } // End of for (int i = 0; i < physicalSize; i++).
} // End of probeNetwork().

void UDPSelectPhysicalCommunicationLayer::initializeCommunicationLayerAttributes() {
    unsigned int physicalId = physicalGetId();
    unsigned int physicalSize = physicalGetSize();
    ASSERT(physicalSize > 1);

    ECLMPL_ASSERT((nextSendSequenceNumber = new SequenceNumber [physicalSize]) != 0);
    ECLMPL_ASSERT((acknowledgedSequenceNumberReceived = new SequenceNumber [physicalSize]) != 0);
    ECLMPL_ASSERT((acknowledgedSequenceNumberSent = new SequenceNumber [physicalSize]) != 0);
    ECLMPL_ASSERT((highestInSequenceNumberReceived = new SequenceNumber [physicalSize]) != 0);
    ECLMPL_ASSERT((highestInSequenceNumberProcessed = new SequenceNumber [physicalSize]) != 0);
    ECLMPL_ASSERT((endOfHole = new SequenceNumber [physicalSize]) != 0);

    outOfOrderMessageQs.resize(physicalSize);
    sendQs.resize(physicalSize);
    synchBuffer.resize(physicalSize);
    
    maxFd = -1;
    for (unsigned int i = 0; i < physicalSize; i++) {
        nextSendSequenceNumber[i] = 1;
        acknowledgedSequenceNumberReceived[i] = 0;
        acknowledgedSequenceNumberSent[i] = 0;
        highestInSequenceNumberReceived[i] = 0;
        highestInSequenceNumberProcessed[i] = 0;
        endOfHole[i] = 0;
        if (connInterface->recvSocket[i]->getSocketFd() > maxFd) {
            maxFd = connInterface->recvSocket[i]->getSocketFd();
        }
        if (i != physicalId) {
            FD_SET(connInterface->recvSocket[i]->getSocketFd(), &selectListenSet);
        }
    }
    fdSet = selectListenSet;
    maxFd += 1; // Needed for call to select.
} // End of initializeCommunicationLayerAttributes().

void UDPSelectPhysicalCommunicationLayer::send(UDPNetworkMessage* const nwMsg, const unsigned int& dest) {
    int msgLen;
    char msg[MAXBUF];
    unsigned int destination = dest; // Underlying interface requires non-const type.
    
    msgLen = nwMsg->serialize(msg, MAXBUF);
    connInterface->send(msgLen, msg, destination);
} // End of send(...).

bool UDPSelectPhysicalCommunicationLayer::insertInOrderMessage(UDPNetworkMessage* nwMsg) {
    bool retVal;
    unsigned int source = nwMsg->getSourceId();
    
    // Don't insert if duplicate.
    if (nwMsg->getSequenceNr() <= highestInSequenceNumberReceived[source]) {
        retVal = false;
    }
    else {
        inOrderMessageQ.push_back(nwMsg);
        highestInSequenceNumberReceived[source]++;
        retVal = true;
    }
    return retVal;
} // End of insertInOrderMessage(...).

void UDPSelectPhysicalCommunicationLayer::processNwMsgInfo(UDPNetworkMessage* nwMsg) {
    // NOTE: Both APPLICATION and CONTROL messages are processed in this method.
    // CONTROL messages will not have a proper sequence number!
    // Do NOT add code here that checks the sequence number of an nwMsg.
    unsigned int source = nwMsg->getSourceId();
    if (nwMsg->getAckFlag() == true) {
        if (nwMsg->getAckSequenceNr() > acknowledgedSequenceNumberReceived[source]) {
            acknowledgedSequenceNumberReceived[source] = nwMsg->getAckSequenceNr();
        }

        // Remove all acknowledged messages from the send Q as we
        // don't need to keep them around anymore.
        while (sendQs[source].empty() == false && (sendQs[source].front()->getSequenceNr() <= acknowledgedSequenceNumberReceived[source])) {
            UDPNetworkMessage* tmp = sendQs[source].front();
            sendQs[source].pop_front();
            delete tmp;
        }

        // Retransmitt messages in gap if requested, i.e.
        // if acknowledged sequence number is <
        // end of hole number.
        if (nwMsg->getAckSequenceNr() < nwMsg->getEndOfHoleNr()) {
            SequenceNumber startHole = nwMsg->getAckSequenceNr();
            SequenceNumber endHole = nwMsg->getEndOfHoleNr();
            std::deque<UDPNetworkMessage*>::iterator msgIter = sendQs[source].begin();
            while (msgIter != sendQs[source].end() && ((*msgIter)->getSequenceNr() < startHole)) {
                msgIter++; // Get the guy behind you in the queue. i.e. sendQs[source].getPrev();
            }
            ECLMPL_DEBUG(std::cerr << physicalGetId() << ": Retransmitting message." << std::endl;)

            while (msgIter != sendQs[source].end() && ((*msgIter)->getSequenceNr() < endHole)) {
                (*msgIter)->setAckSequenceNr(highestInSequenceNumberReceived[source]);
                send((*msgIter), source);
                msgIter++; // Get the guy behind you in the queue. i.e. sendQs[source].getPrev();
            }
        }
    } // End of if (nwMsg->getAckFlag() == true).
} // End of UDP_ProcessNwMsgInfo(...).

void UDPSelectPhysicalCommunicationLayer::updateOutOfOrderMessageQs(const unsigned int& source, const bool& sendRetransmissionReq) {
    // If we have out of order messages we need to check, for every
    // other message that we insert (whether that message is in order
    // order not), whether we can move some messages from an out-of
    // -order message queue to the in-order message queue.
    bool movedMessage = false;
    bool done = false;
    unsigned int physicalSize = physicalGetSize();
    UDPNetworkMessage* tmpMsg;

    while (outOfOrderMessageQs[source].empty() == false && done == false) {
        if (outOfOrderMessageQs[source].top()->getSequenceNr() == highestInSequenceNumberReceived[source]+1) {
            tmpMsg = outOfOrderMessageQs[source].top();
            outOfOrderMessageQs[source].pop();
            ECLMPL_ASSERT(insertInOrderMessage(tmpMsg) != false);
            movedMessage = true;
        }
        else if (outOfOrderMessageQs[source].top()->getSequenceNr() <= highestInSequenceNumberReceived[source]) {
            // Discard duplicate.
            UDPNetworkMessage* tmp = outOfOrderMessageQs[source].top();
            outOfOrderMessageQs[source].pop();
            delete tmp;
        }
        else {
            done = true;
        }
    } // End of while(...).

    // Update end-of-hole information.
    if (outOfOrderMessageQs[source].empty() == false) {
        endOfHole[source] = outOfOrderMessageQs[source].top()->getSequenceNr();
    }
    else {
        endOfHole[source] = 0;
    }

    // Request retransmission of out of sequence messages were received this "round".
    if (outOfOrderMessageQs[source].empty() == false && sendRetransmissionReq == true) {
        acknowledgedSequenceNumberSent[source] = highestInSequenceNumberReceived[source];
        // Since this is a control message we do not need to specify any sequence
        // number. We set it arbitrarily to 0 (zero).
        UDPNetworkMessage nwMsg(CONTROL, physicalGetId(), true, false, highestInSequenceNumberReceived[source], endOfHole[source], 0, 0, NULL);

        // Note that we should NOT push this control message into the sendQ!
        // There's no reason to keep track of a retransmission request... if we
        // need to send a new one we'll just do that instead of retransmitting
        // an old retransmission request.
        send(&nwMsg, source);
        ECLMPL_DEBUG(std::cerr << physicalGetId() << ": Retransmission request sent to " << source << std::endl;)
    }

    // If we moved a message from one queue to another and that out of order
    // queue is now empty, we need to check if we have any other out of order
    // messages left.
    if (movedMessage == true && outOfOrderMessageQs[source].empty() == true) {
        outOfOrderMessageQsAreEmpty = true;
        for (unsigned int iter = 0; iter < physicalSize && outOfOrderMessageQsAreEmpty == true; iter++) {
            if (outOfOrderMessageQs[iter].empty() == false) {
                outOfOrderMessageQsAreEmpty = false;
            }
        }
    }
} // End of updateOutOfOrderMessageQs().

unsigned int UDPSelectPhysicalCommunicationLayer::checkRetransmissionTimeout() {
    unsigned int physicalId = physicalGetId();
    unsigned int physicalSize = physicalGetSize();
    unsigned int numberOfRetransmittedMessages = 0;
    std::chrono::duration<double> retransmissionTimeout(RETRANSMISSION_TIMEOUT);
    std::chrono::duration<double> timeDifference(0.0);
    
    // Get the current time
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    // Calculate elapsed time from when the timer started
    timeDifference = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);
    
    // Check if the elapsed time has reached the timed out threshold
    if (timeDifference >= retransmissionTimeout) {
        for (unsigned int i = 0; i < physicalSize; i++) {
            if (i != physicalId && sendQs[i].empty() == false) {
                // This implies:
                //  SeqGT((sendQs[source].getFront()->getSequenceNr()), acknowledgedSequenceNumberReceived[i]) == true
                std::deque<UDPNetworkMessage*>::iterator tmpIter = sendQs[i].begin();
                ECLMPL_ASSERT(tmpIter != sendQs[i].end());
                for ( ; tmpIter != sendQs[i].end() && (*tmpIter)->timedOut(RETRANSMISSION_TIMEOUT); tmpIter++) {
                    // Update ack info for message.
                    (*tmpIter)->setAckSequenceNr(highestInSequenceNumberReceived[i]);
                    send((*tmpIter), i);
                    numberOfRetransmittedMessages++;
                }
            }
        }
        // Restart timer...
        startTime = std::chrono::steady_clock::now();

    }
    ECLMPL_DEBUG(if (numberOfRetransmittedMessages > 0) {
    std::cerr << physicalId << ": Retransmission timer timed out. Retransmitted " << numberOfRetransmittedMessages << " messages." << std::endl;
})
    return numberOfRetransmittedMessages;
} // End of checkRetransmissionTimeout().

void UDPSelectPhysicalCommunicationLayer::sendAck(unsigned int dest) {
    ASSERT(dest <= physicalGetSize());
    acknowledgedSequenceNumberSent[dest] = highestInSequenceNumberReceived[dest];
    UDPNetworkMessage nwMsg(APPLICATION, physicalGetId(), true, false, highestInSequenceNumberReceived[dest], 0, 0, 0, NULL);
    send(&nwMsg, dest);
} // End of sendAck(...).

#undef RETRANSMISSION_TIMEOUT
#undef SEND_EXPLICIT_ACK_UPON_RECEIVING_DUPLICATE

