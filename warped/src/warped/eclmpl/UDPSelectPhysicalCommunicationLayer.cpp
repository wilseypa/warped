#include "UDPSelectPhysicalCommunicationLayer.h"
using std::cerr;
using std::endl;

// When running timings, we don't want to print stuff...
//#define ECLMPL_DEBUG(x)

const unsigned int mtu = ECLMPL_MTU; // No messages with greater size may be received or sent.
                                     // This number itself can be changed of course...
const int maxBuf = 65535;

const bool sendExplicitAckUponReceivingDuplicate = true;

UDPSelectPhysicalCommunicationLayer::UDPSelectPhysicalCommunicationLayer() {
  connInterface = new UDPConnectionInterface(mtu);
  maxFd = -1;
  FD_ZERO(&selectListenSet);
  FD_ZERO(&fdSet);
} // End of default constructor.

UDPSelectPhysicalCommunicationLayer::~UDPSelectPhysicalCommunicationLayer(){
} // End of desctructor.

void
UDPSelectPhysicalCommunicationLayer::probeNetwork() {
  char recvBuf[maxBuf];
  unsigned int recvBufSize;
  int size = 0;
  unsigned int source = 0;
  eclmplUnreliableNetworkMessage *nwMsg;
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
    if (static_cast<UDPConnectionInterface *>(connInterface)->recvSocket[i]->wFD_ISSET(&selectListenSet) == true) {
      size = 0;
      while (size != -1) {
	recvBufSize = maxBuf;
	source = i;
	connInterface->recv(recvBufSize, recvBuf, source);
	size = (int)recvBufSize;
	nwMsg = new eclmplUnreliableNetworkMessage();
	//ECLMPL_DEBUG(cerr << physicalId << ": recvd " << recvBuf << endl;)
	nwMsg->deserialize(recvBuf, size);
	//ECLMPL_DEBUG(cerr << physicalId << ": recvd " << *nwMsg << endl;)
	source = nwMsg->getSourceId();
	
	if (nwMsg->getType() == APPLICATION) {
	  if (nwMsg->getUserDataFlag() == true) {
	    msgSqNr = nwMsg->getSequenceNr();
	    if (msgSqNr == highestInSequenceNumberReceived[source]+1) {
	      insertInOrderMessage(nwMsg);
	    }
	    //else if (msgSqNr <= highestInSequenceNumberReceived[source]) {
	    else if (SeqLEQ(msgSqNr, highestInSequenceNumberReceived[source])) {
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
        if (sendRetransmissionReq == true || (foundDuplicate && 
					      sendExplicitAckUponReceivingDuplicate)) {
	  // Peek socket and see if anything more is there.
	  size = static_cast<UDPConnectionInterface *>(connInterface)->recvSocket[i]->peek(recvBuf, 1);
	}
	else {
	  size = -1; // recvSocket[i].peek(recvBuf, 1);
	}
      } // End of while (size != -1).

      // Any new message(s) from source may cause the 
      // outOfOrderMessageQ to need to be updated.
      if (outOfOrderMessageQs[source].empty() == false) {
	ECLMPL_DEBUG(cerr << physicalId << ": NEW OUT OF ORDER MESSAGES IN oooQ[" 
	      << source <<"]" << endl;)
	updateOutOfOrderMessageQs(source, sendRetransmissionReq);
      }

      // We must send an explicit acknowledgement if the number
      // of unacknowledged messages for a certain peer exceeds 
      // maxNrUnackedMsgs.
      //if (highestInSequenceNumberReceived[source]-
      //  acknowledgedSequenceNumberSent[source] > maxNrUnackedMsgs) {
      if (SeqGT((highestInSequenceNumberReceived[source]-
	  acknowledgedSequenceNumberSent[source]), maxNrUnackedMsgs)) {
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
    if (foundDuplicate && sendExplicitAckUponReceivingDuplicate) {
      sendAck(source);
      ECLMPL_DEBUG(cerr << physicalId << ": Sent expl. ack upon receiving DUPLICATE from "
	    << source << endl;)
    }
  } // End of for (int i = 0; i < physicalSize; i++).
} // End of probeNetwork().

void
UDPSelectPhysicalCommunicationLayer::initializeCommunicationLayerAttributes() {
  ASSERT(physicalSize > 1);

  ECLMPL_ASSERT( (nextSendSequenceNumber = new SequenceNumber [physicalSize]) != 0);
  ECLMPL_ASSERT( (acknowledgedSequenceNumberReceived = new SequenceNumber [physicalSize]) != 0);
  ECLMPL_ASSERT( (acknowledgedSequenceNumberSent = new SequenceNumber [physicalSize]) != 0);
  ECLMPL_ASSERT( (highestInSequenceNumberReceived = new SequenceNumber [physicalSize]) != 0); 
  ECLMPL_ASSERT( (highestInSequenceNumberProcessed = new SequenceNumber [physicalSize]) != 0);
  ECLMPL_ASSERT( (endOfHole = new SequenceNumber [physicalSize]) != 0);

  outOfOrderMessageQs.resize(physicalSize);
  //  ECLMPL_ASSERT( (outOfOrderMessageQs = new eclmplUnreliableNetworkMessagePriorityQueue[physicalSize]) != 0);
  sendQs.resize(physicalSize);
  //  ECLMPL_ASSERT( (sendQs = new eclmplUnreliableNetworkMessageQueue[physicalSize]) != 0);
  synchBuffer.resize(physicalSize);
  //  ECLMPL_ASSERT( (synchBuffer = new eclmplUnreliableNetworkMessageQueue[physicalSize]) != 0);

  maxFd = -1;
  for (unsigned int i = 0; i < physicalSize; i++) {
    nextSendSequenceNumber[i] = 1;
    acknowledgedSequenceNumberReceived[i] = 0;
    acknowledgedSequenceNumberSent[i] = 0;
    highestInSequenceNumberReceived[i] = 0;
    highestInSequenceNumberProcessed[i] = 0;
    endOfHole[i] = 0;
    if (static_cast<UDPConnectionInterface *>(connInterface)->recvSocket[i]->getSocketFd() > maxFd) {
      maxFd = static_cast<UDPConnectionInterface *>(connInterface)->recvSocket[i]->getSocketFd();
    }
    if (i != physicalId) {
      FD_SET(static_cast<UDPConnectionInterface *>(connInterface)->recvSocket[i]->getSocketFd(), &selectListenSet);
    }
  }
  fdSet = selectListenSet;
  maxFd += 1; // Needed for call to select.
} // End of initializeCommunicationLayerAttributes().

#if 0
bool
UDPSelectPhysicalCommunicationLayer::UDP_SynchronizeMaster(const eclmplUnreliableNetworkMessageType &msgType) {
  bool confirmedId[physicalSize];
  unsigned int numberConfirmed = 0;
  bool confirmationMessage;
  timeval ttConfirmedTimerStart, ttConfirmedTimerCurr;
  double confirmedTimer;
  double confirmedTimerTimeout = synchTimeBase*(double)physicalSize;
  eclmplUnreliableNetworkMessage *synchBuffMsg;
  bool synchronized = false;
  char hostName[256];
  ECLMPL_ASSERT(gethostname(hostName, 256) != -1);

  eclmplUnreliableNetworkMessage nwMsg(msgType, physicalId, false, false, 0, 
			  0, 0, 
			  //windowSize, 
			  0, NULL); 

  for (unsigned int i = 0; i < physicalSize; i++) {
    confirmedId[i] = false;
  }
  
  string type;
  switch(msgType) {
  case UNDEFINED: type = "UNDEFINED";
    break;
  case INITIALIZE: type = "INITIALIZE";
    break;
  case START: type = "START";
    break;
  case APPLICATION: type = "APPLICATION";
    break;
  case FINALIZE: type = "FINALIZE";
    break;
  case CONTROL: type = "CONTROL";
    break;
  default: type = "ERROR-UNKNOWN-TYPE";
    break;
  }
  ECLMPL_DEBUG_FILE(outfile << physicalId << ": " << hostName << " SYNCHING on \"" << type << "\"" << endl;)

  gettimeofday(&ttConfirmedTimerStart, NULL);
  gettimeofday(&ttConfirmedTimerCurr, NULL);
  confirmedTimer = (double)(ttConfirmedTimerCurr.tv_sec-ttConfirmedTimerStart.tv_sec);
  while (numberConfirmed < physicalSize-1 && confirmedTimer < confirmedTimerTimeout) {
    probeNetwork();
    for (unsigned int i = 1; i < physicalSize; i++) {
      confirmationMessage = false;
      while (!synchBuffer[i].empty()) {
	synchBuffMsg = synchBuffer[i].front();
	synchBuffer[i].pop_front();
	if (synchBuffMsg != NULL && synchBuffMsg->getType() == msgType) {
	  //ECLMPL_DEBUG_FILE(outfile << "r[" << i << "]:" << *synchBuffMsg << " SYNCH_MASTER" << endl;)
	  if (!confirmedId[i]) {
	    confirmedId[i] = true;
	    numberConfirmed++;
	  }
	  confirmationMessage = true;
	}
	delete synchBuffMsg; 
      }
      if (confirmationMessage) {
	// Send message to i.
	send(&nwMsg, i);
      }
    }
    gettimeofday(&ttConfirmedTimerCurr, NULL);
    confirmedTimer = (double)(ttConfirmedTimerCurr.tv_sec-ttConfirmedTimerStart.tv_sec);
  } // End of while (...).

  if (numberConfirmed == physicalSize-1) {
    eclmplUnreliableNetworkMessage synMsg(SYNCH, physicalId, false, false, 0, 
			     0, 0, 
			     //windowSize, 
			     0, NULL); 
    for (unsigned int i = 1; i < physicalSize; i++) {
      send(&synMsg, i);
    }
    synchronized = true;
  }
  if (!synchronized) {
    cerr << physicalId << ": " << hostName << " __TIMED OUT__ Could not synchronize on \""
	 << type << "\" with peers." << endl;
  }
  ECLMPL_DEBUG_FILE(outfile << "                                                 " << physicalId << ": " << hostName 
	     << " SYNCHED on \"" << type << "\"" << endl;)
  return synchronized;
} // End of UDP_SynchronizeMaster(...).

bool 
UDPSelectPhysicalCommunicationLayer::UDP_SynchronizeSlave(const eclmplUnreliableNetworkMessageType &msgType) {
  timeval ttConfirmedTimerStart, ttConfirmedTimerCurr;
  double confirmedTimer;
  double confirmedTimerTimeout = synchTimeBase*(double)physicalSize;
  bool confirmed = false;
  bool synchronized = false;
  eclmplUnreliableNetworkMessage *synchBuffMsg;
  char hostName[256];
  ECLMPL_ASSERT(gethostname(hostName, 256) != -1);

  eclmplUnreliableNetworkMessage nwMsg(msgType, physicalId, false, false, 0, 
			  0, 0, 
			  //windowSize, 
			  0, NULL); 

  string type;
  switch(msgType) {
  case UNDEFINED: type = "UNDEFINED";
    break;
  case INITIALIZE: type = "INITIALIZE";
    break;
  case START: type = "START";
    break;
  case APPLICATION: type = "APPLICATION";
    break;
  case FINALIZE: type = "FINALIZE";
    break;
  case CONTROL: type = "CONTROL";
    break;
  default: type = "ERROR-UNKNOWN-TYPE";
    break;
  }
  ECLMPL_DEBUG_FILE(outfile << physicalId << ": " << hostName << " SYNCHING on \"" << type << "\"" << endl;)

  gettimeofday(&ttConfirmedTimerStart, NULL);
  gettimeofday(&ttConfirmedTimerCurr, NULL);
  confirmedTimer = (double)(ttConfirmedTimerCurr.tv_sec-ttConfirmedTimerStart.tv_sec);
  while ((!confirmed || !synchronized) && confirmedTimer < confirmedTimerTimeout) {
    if (!confirmed) {
      send(&nwMsg, 0);
      unsigned int timer = 1; // sleep one second...
      sleep(timer);
    }
    probeNetwork();
    if (!synchBuffer[0].empty()) {
      synchBuffMsg = synchBuffer[0].front();
      synchBuffer[0].pop_front();
      if (synchBuffMsg != NULL && synchBuffMsg->getType() == msgType) {
	//ECLMPL_DEBUG_FILE(outfile << "r[" << 0 << "]:" << *synchBuffMsg << " SYNCH_SLAVE" << endl;)
	confirmed = true;
      }
      else if (synchBuffMsg != NULL && synchBuffMsg->getType() == SYNCH) {
	//ECLMPL_DEBUG_FILE(outfile << "r[" << 0 << "]:" << *synchBuffMsg << " SYNCH_SLAVE" << endl;)
	synchronized = true;
      }
      delete synchBuffMsg; 
    }
    gettimeofday(&ttConfirmedTimerCurr, NULL);
    confirmedTimer = (double)(ttConfirmedTimerCurr.tv_sec-ttConfirmedTimerStart.tv_sec);
  }
  while (!synchBuffer[0].empty()) {
    eclmplUnreliableNetworkMessage *tmpMsg = synchBuffer[0].front();
    synchBuffer[0].pop_front();
    delete tmpMsg;
  }
  synchronized = confirmed || synchronized;
  if (!synchronized) {
    cerr << physicalId << ": " << hostName << " __TIMED OUT__ Could not synchronize on \""
	 << type << "\" with peers." << endl;
  }
  ECLMPL_DEBUG_FILE(outfile << "                                                 " << physicalId << ": " << hostName 
	     << " SYNCHED on \"" << type << "\"" << endl;)
  return synchronized;
} // End of UDP_SynchronizeSlave(...).

void
UDPSelectPhysicalCommunicationLayer::UDP_Finalize() {
  if (physicalId == 0) {
    UDP_SynchronizeMaster(FINALIZE);
  }
  else {
    UDP_SynchronizeSlave(FINALIZE);
  }
} // End of UDP_Finalize().

#endif


