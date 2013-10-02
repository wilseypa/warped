#include "eclmplReliablePhysicalCommunicationLayer.h"
#include "SerializedInstance.h"
#include <iostream>
using std::cout;
using std::endl;
#define ONLY_CHECK_PHYSICAL_LAYER_ON_EMPTY_QUEUE
const int maxBuf = 65535;
const int explicitAckLimit = 100;
const double retransmissionTimeout = 1.0; // seconds
const bool sendExplicitAckUponReceivingDuplicate = true;

eclmplReliablePhysicalCommunicationLayer::eclmplReliablePhysicalCommunicationLayer() {
    physicalId = 0;
    physicalSize = 0;
    nextSendSequenceNumber = NULL;
    acknowledgedSequenceNumberReceived = NULL;
    acknowledgedSequenceNumberSent = NULL;
    highestInSequenceNumberReceived = NULL;
    highestInSequenceNumberReceived = NULL;
    outOfOrderMessageQsAreEmpty = true;
    endOfHole = NULL;
    //windowSize = 1;
    maxNrUnackedMsgs = explicitAckLimit;
    retransmissionTimer = new eclmplTimer(retransmissionTimeout);
} // End of default constructor.

eclmplReliablePhysicalCommunicationLayer::~eclmplReliablePhysicalCommunicationLayer() {
    delete[] nextSendSequenceNumber;
    delete[] acknowledgedSequenceNumberReceived;
    delete[] acknowledgedSequenceNumberSent;
    delete[] highestInSequenceNumberReceived;
    delete[] highestInSequenceNumberProcessed;
    delete[] endOfHole;
    delete retransmissionTimer;
} // End of desctructor.

void
eclmplReliablePhysicalCommunicationLayer::physicalInit() {
    int argc = 0;
    char** argv = new char*[argc];
    argv[0] = NULL;

    connInterface->establishConnections(&argc, &argv);
    physicalId = connInterface->getConnectionId();
    if ((physicalSize = connInterface->getNumberOfConnections()) > 1) {
        initializeCommunicationLayerAttributes();
        retransmissionTimer->start();
    }

    delete [] argv;
} // End of physicalInit(...).

int
eclmplReliablePhysicalCommunicationLayer::physicalGetId() const {
    return physicalId;
} // End of physicalGetId().

void
eclmplReliablePhysicalCommunicationLayer::physicalSend(const SerializedInstance* toSend,
                                                       unsigned int dest) {
    const void* buffer = &toSend->getData()[0];
    const unsigned int size = toSend->getSize();

    ASSERT(buffer != NULL);
    ASSERT(dest <= physicalSize);
    ASSERT(dest != physicalId);

    // We piggy-back acknowledgements information for every message we send.
    acknowledgedSequenceNumberSent[dest] = highestInSequenceNumberReceived[dest];

    const char* newBuf = cppStrDup((char*)buffer, size);
    eclmplUnreliableNetworkMessage* nwMsg =
        new eclmplUnreliableNetworkMessage(APPLICATION,
                                           physicalId,
                                           true,
                                           true,
                                           highestInSequenceNumberReceived[dest],
                                           0,
                                           nextSendSequenceNumber[dest],
                                           //windowSize,
                                           size,
                                           const_cast<char*>(newBuf));
    nextSendSequenceNumber[dest]++;
    sendQs[dest].push_back(nwMsg);
    send(nwMsg, dest);
} // End of physicalSend(...).

SerializedInstance*
eclmplReliablePhysicalCommunicationLayer::physicalProbeRecv() {
#ifdef ONLY_CHECK_PHYSICAL_LAYER_ON_EMPTY_QUEUE
    if (inOrderMessageQ.empty() == true) {
        checkRetransmissionTimeout();
        probeNetwork();
    }
#else
    probeNetwork();
#endif
    return getNextInOrder();
} // End of physicalProbeRecv().

// bool
// eclmplReliablePhysicalCommunicationLayer::physicalProbeRecvBuffer(char *buffer,
//                               int size,
//                               bool& sizeStatus){
//   int msgSize;
//   bool retVal = false;

//   // Get messages waiting in the network input queues and
//   // insert them in our buffering queues that ensure messages
//   // are delivered in FIFO and sequence order to the kernel.
// #ifdef ONLY_CHECK_PHYSICAL_LAYER_ON_EMPTY_QUEUE
//   if (inOrderMessageQ.empty() == true) {
//     checkRetransmissionTimeout();
//     probeNetwork();
//   }
// #else
//   probeNetwork();
// #endif

//   // Peek next message and get it's size.
//   msgSize = peekNextInOrderSize();

//   if (msgSize != -1) {
//     if ( msgSize > size) {
//       sizeStatus = false;
//     }
//     else {
//       buffer = getNextInOrder();
//       sizeStatus = true;
//     }
//   }
//   else {
//     // No messages available.
//     retVal = false;
//   }
//   return retVal;
// } // End of physicalProbeRecvBuf(...).

void
eclmplReliablePhysicalCommunicationLayer::physicalFinalize() {
    connInterface->tearDownConnections();
} // End of physicalFinalize().

int
eclmplReliablePhysicalCommunicationLayer::physicalGetSize() const {
    return physicalSize;
} // End of physicalGetSize().

void
eclmplReliablePhysicalCommunicationLayer::send(eclmplUnreliableNetworkMessage* const nwMsg,
                                               const unsigned int& dest) {
    int msgLen;
    char msg[maxBuf];
    msgLen = nwMsg->serialize(msg, maxBuf);
    ASSERT(nwMsg->setSendTime() != -1);
    unsigned int destination = dest; // Underlying interface requires non-const type.
    connInterface->send(msgLen, msg, destination);
} // End of send(...).

bool
eclmplReliablePhysicalCommunicationLayer::insertInOrderMessage(eclmplUnreliableNetworkMessage*
                                                               nwMsg) {
    bool retVal;
    unsigned int source = nwMsg->getSourceId();
    // Don't insert if duplicate.
    //if (nwMsg->getSequenceNr() <= highestInSequenceNumberReceived[source]) {
    if (SeqLEQ(nwMsg->getSequenceNr(), highestInSequenceNumberReceived[source])) {
        retVal = false;
    } else {
        inOrderMessageQ.push_back(nwMsg);
        highestInSequenceNumberReceived[source]++;
        retVal = true;
    }
    return retVal;
} // End of insertInOrderMessage(...).

void
eclmplReliablePhysicalCommunicationLayer::processNwMsgInfo(eclmplUnreliableNetworkMessage* nwMsg) {
    // NOTE: Both APPLICATION and CONTROL messages are processed in this method.
    // CONTROL messages will not have a proper sequence number!
    // Do NOT add code here that checks the sequence number of an nwMsg.
    unsigned int source = nwMsg->getSourceId();
    if (nwMsg->getAckFlag() == true) {
        if (SeqGT(nwMsg->getAckSequenceNr(), acknowledgedSequenceNumberReceived[source])) {
            acknowledgedSequenceNumberReceived[source] = nwMsg->getAckSequenceNr();
        }

        // Remove all acknowledged messages from the send Q as we
        // don't need to keep them around anymore.
        while (sendQs[source].empty() == false &&
                SeqLEQ(sendQs[source].front()->getSequenceNr(),
                       acknowledgedSequenceNumberReceived[source])) {
            eclmplUnreliableNetworkMessage* tmp = sendQs[source].front();
            sendQs[source].pop_front();
            delete(char*)(tmp->getUserData());
            delete tmp;
        }

        // Retransmitt messages in gap if requested, i.e.
        // if acknowledged sequence number is <
        // end of hole number.
        if (SeqLT(nwMsg->getAckSequenceNr(), nwMsg->getEndOfHoleNr())) {
            SequenceNumber startHole = nwMsg->getAckSequenceNr();
            SequenceNumber endHole = nwMsg->getEndOfHoleNr();
            deque<eclmplUnreliableNetworkMessage*>::iterator msgIter = sendQs[source].begin();
            while (msgIter != sendQs[source].end() &&
                    SeqLT((*msgIter)->getSequenceNr(), startHole)) {
                msgIter++; // Get the guy behind you in the queue. i.e. sendQs[source].getPrev();
            }
            ECLMPL_DEBUG(std::cerr << physicalId << ": Retransmitting message." << std::endl;)

            while (msgIter != sendQs[source].end() && SeqLT((*msgIter)->getSequenceNr(), endHole)) {
                (*msgIter)->setAckSequenceNr(highestInSequenceNumberReceived[source]);
                send((*msgIter), source);
                msgIter++; // Get the guy behind you in the queue. i.e. sendQs[source].getPrev();
            }
        }
    } // End of if (nwMsg->getAckFlag() == true).
} // End of UDP_ProcessNwMsgInfo(...).

void
eclmplReliablePhysicalCommunicationLayer::updateOutOfOrderMessageQs(const unsigned int& source,
                                                                    const bool& sendRetransmissionReq) {
    // If we have out of order messages we need to check, for every
    // other message that we insert (whether that message is in order
    // order not), whether we can move some messages from an out-of
    // -order message queue to the in-order message queue.
    bool movedMessage = false;
    bool done = false;
    eclmplUnreliableNetworkMessage* tmpMsg;

    while (outOfOrderMessageQs[source].empty() == false &&
            done == false) {
        if (outOfOrderMessageQs[source].top()->getSequenceNr() ==
                highestInSequenceNumberReceived[source]+1) {
            tmpMsg = outOfOrderMessageQs[source].top();
            outOfOrderMessageQs[source].pop();
            ECLMPL_ASSERT(insertInOrderMessage(tmpMsg) != false);
            movedMessage = true;
        } else if (outOfOrderMessageQs[source].top()->getSequenceNr() ==
                   highestInSequenceNumberReceived[source]) {
            // Discard duplicate.
            eclmplUnreliableNetworkMessage* tmp = outOfOrderMessageQs[source].top();
            outOfOrderMessageQs[source].pop();
            delete tmp;
        } else {
            done = true;
        }
    } // End of while(...).

    // Update end-of-hole information.
    //startHole = highestInSequenceNumberReceived[source];
    if (outOfOrderMessageQs[source].empty() == false) {
        endOfHole[source] = outOfOrderMessageQs[source].top()->getSequenceNr();
    } else {
        endOfHole[source] = 0;
    }

    // Request retransmission of out of sequence messages were received
    // this "round".
    if (outOfOrderMessageQs[source].empty() == false &&
            sendRetransmissionReq == true) {
        acknowledgedSequenceNumberSent[source] = highestInSequenceNumberReceived[source];
        // Since this is a control message we do not need to specify any sequence
        // number. We set it arbitrarily to 0 (zero).
        eclmplUnreliableNetworkMessage nwMsg(CONTROL, physicalId, true, false,
                                             highestInSequenceNumberReceived[source],
                                             endOfHole[source],
                                             0,
                                             //windowSize,
                                             0, NULL);

        // Note that we should NOT push this control message into the sendQ!
        // There's no reason to keep track of a retransmission request... if we
        // need to send a new one we'll just do that instead of retransmitting
        // an old retransmission request.
        send(&nwMsg, source);
        ECLMPL_DEBUG(std::cerr << physicalId << ": Retransmission request sent to " << source << std::endl;)
    }

    // If we moved a message from one queue to another and that out of order
    // queue is now empty, we need to check if we have any other out of order
    // messages left.
    if (movedMessage == true && outOfOrderMessageQs[source].empty() == true) {
        unsigned int iter = 0;
        outOfOrderMessageQsAreEmpty = true;
        while (iter < physicalSize && outOfOrderMessageQsAreEmpty == true) {
            if (outOfOrderMessageQs[iter].empty() == false) {
                outOfOrderMessageQsAreEmpty = false;
            }
            iter++;
        }
    }
} // End of updateOutOfOrderMessageQs().

unsigned int
eclmplReliablePhysicalCommunicationLayer::checkRetransmissionTimeout() {
    unsigned int numberOfRetransmittedMessages = 0;

    timeval currTime;
    gettimeofday(&currTime, NULL);

    if (retransmissionTimer->timedOut()) {
        for (unsigned int i = 0; i < physicalSize; i++) {
            if (i != physicalId && sendQs[i].empty() == false) {
                // This implies:
                //  SeqGT((sendQs[source].getFront()->getSequenceNr()),
                //        acknowledgedSequenceNumberReceived[i]) == true
                deque<eclmplUnreliableNetworkMessage*>::iterator tmpIter = sendQs[i].begin();
                ECLMPL_ASSERT(tmpIter != sendQs[i].end());
                while (tmpIter != sendQs[i].end() &&
                        (*tmpIter)->timedOut(currTime, retransmissionTimeout)) {
                    // Update ack info for message.
                    (*tmpIter)->setAckSequenceNr(highestInSequenceNumberReceived[i]);
                    send((*tmpIter), i);
                    numberOfRetransmittedMessages++;
                    tmpIter++; // tmpMsg = sendQs[i].getPrev();
                }
            }
        }
        // Restart timer...
        retransmissionTimer->start();

    }
    ECLMPL_DEBUG(if (numberOfRetransmittedMessages > 0) {
    std::cerr << physicalId << ": Retransmission timer timed out. Retransmitted "
              << numberOfRetransmittedMessages << " messages." << std::endl;
})
    return numberOfRetransmittedMessages;
} // End of UDP_retransmit().

SerializedInstance*
eclmplReliablePhysicalCommunicationLayer::getNextInOrder() {
    SerializedInstance* retval = 0;

    eclmplUnreliableNetworkMessage* nwMsg;

    if (inOrderMessageQ.empty() == false) {
        nwMsg = inOrderMessageQ.front();
        inOrderMessageQ.pop_front();
        retval = new SerializedInstance(nwMsg->getUserData(),
                                        nwMsg->getUserDataSize());
        delete nwMsg;
    } // End of if (inOrderMessageQIsEmpty == false).
    return retval;
} // End of getNextInSequence().

// Size -1 will be returned if no in-sequence message is avaiable.
int
eclmplReliablePhysicalCommunicationLayer::peekNextInOrderSize() {
    int size = -1;
    if (inOrderMessageQ.empty() == false) {
        size = inOrderMessageQ.front()->getUserDataSize();
    }
    return size;
} // End of peekNextInSequenceSize().

void
eclmplReliablePhysicalCommunicationLayer::sendAck(unsigned int dest) {
    ASSERT(dest <= physicalSize);
    acknowledgedSequenceNumberSent[dest] = highestInSequenceNumberReceived[dest];
    eclmplUnreliableNetworkMessage nwMsg(APPLICATION, physicalId, true, false,
                                         highestInSequenceNumberReceived[dest],
                                         0, 0,
                                         //windowSize,
                                         0, NULL);
    send(&nwMsg, dest);
} // End of sendAck(...)

#ifdef ONLY_CHECK_PHYSICAL_LAYER_ON_EMPTY_QUEUE
#undef ONLY_CHECK_PHYSICAL_LAYER_ON_EMPTY_QUEUE
#endif
