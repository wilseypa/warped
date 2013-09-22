
#include "MessageManager.h"
#include "TimeWarpSimulationManager.h"
using std::cerr;
using std::endl;

MessageManager::MessageManager( unsigned int numLPs, 
				CommunicationManager *commManager,
				SimulationManager *simMgr,
				PhysicalCommunicationLayer *phyLib )
  : numberOfLps(numLPs), 
    myCommunicationManager( commManager ),
    mySimulationManager( simMgr ){
   
   lpId = commManager->getId();
   
   for(int i = 0; i < numberOfLps; i++) {
      Transceiver *toAdd = new Transceiver();
      toAdd->setLpId(i);
      toAdd->setMessageManagerHandle(this);
      toAdd->setCommPhyInterface(phyLib);
      send_buffer.push_back(toAdd);
   }
   
   receive_buffer = send_buffer[lpId];

   numberOfMessagesToBeSent = 0;
   numberOfMessagesSentPreviously = 0;
   ageOfMessage = 0;
   receiveCriterion = 0;
   rollingBack = false;
   waitingForMessage = 0;

   ageIncrementor = 0;
   tanTheta = 0.0;
   maxReceiveDelay = 15;
   maximumAge = 20;
   aggregateCtrlMsg = 0;
   newMaximumAge = 20;
   previousMaximumAge = 20;
   
   upCurve = true;
   downCurve = false;
   prevRate = 0.0;
}

MessageManager::~MessageManager() {
  for(int i = 0; i < numberOfLps; i++) {
     delete send_buffer[i];
  }
}


void 
MessageManager::initMessageManager(int mylpId) {}

inline void
MessageManager::probeToSend() {
  int i=0;
  for(i=0; i<numberOfLps; i++) {
    if(i != lpId) {
      send_buffer[i]->probeToSend();
    }
  }
}

inline void
MessageManager::probeToReceive() {
  receive_buffer->probeToReceive();
}

inline bool
MessageManager::isEmptyInputQ() {
  return ((TimeWarpSimulationManager *)mySimulationManager)->checkIdleStatus();
}

inline bool
MessageManager::isControlMessage(KernelMessage* msg) {

  string msgType = msg->getDataType();
  
  if((msgType == "EventMessage") || (msgType == "NegativeEventMessage")){
    return false;
  }
  else if((msgType == "InitializationMessage") ||
	  (msgType == "StartMessage") ||
	  (msgType == "CheckIdleMessage") ||
	  (msgType == "AbortSimulationMessage")){
    return true;
  }
  else {
    return ((aggregateCtrlMsg == 0) ? true : false);
  }
}

inline void
MessageManager::incrementNumberOfMessagesToBeSent() {
  numberOfMessagesToBeSent++;
}

inline void
MessageManager::incrementReceiveCriterion() {
  receiveCriterion++;
}

inline 
void 
MessageManager::resetSendCriterionInfo() {
  //reset the age count
  //reset the number of messages
  ageOfMessage = 0;
  numberOfMessagesToBeSent = 0;
}


inline 
void 
MessageManager::resetReceiveCriterionInfo() {
  //reset the age count for receiving the messages
  receiveCriterion = 0;
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedPessimism() {
  return ((ageOfMessage > maximumAge) ? SEND : DO_NOT_SEND );
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedSlope() {
  //Check with Threshold curve to see, what has to be done with the message

  if((ageOfMessage > 10) &&
     (numberOfMessagesToBeSent + 1 < (ageOfMessage * ageOfMessage * tanTheta))) {
    return SEND;
  }
  return DO_NOT_SEND;
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedSlopeWithError() {

  if(rollingBack == true) {
    return DO_NOT_SEND;
  }
  if(isEmptyInputQ()) {
    return SEND;
  }
  return getSendCriteriaInFixedSlope();
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInMeanOfFactors() {
  return ((ageOfMessage > newMaximumAge) ? SEND : DO_NOT_SEND );
}

inline
AggregationSendCriteria
MessageManager::getSendCriteria(KernelMessage* msg, int size) {
  if(!send_buffer[msg->getReceiver()]->canWriteMessage(size)) {
    if(isControlMessage(msg)) {
      return SEND_WRITE_SEND;
    }
    else {
      return SEND_AND_WRITE;
    }
  }
  else {
    if(isControlMessage(msg)) {
      return WRITE_AND_SEND;
    }
    else {
      return WRITE;
    }
  }
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedMsgCount() {
  if (numberOfMessagesToBeSent >= maximumAge) {
    return SEND;
  }
  else {
    return DO_NOT_SEND;
  }
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedMsgCount(KernelMessage* msg, int size) {
  AggregationSendCriteria criteria = getSendCriteria(msg, size);
  if(criteria == WRITE) {
    return getSendCriteriaInFixedMsgCount();
  }
  else {
    return criteria;
  }
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedPessimism(KernelMessage* msg, int size) {
  AggregationSendCriteria criteria = getSendCriteria(msg, size);
  if(criteria == WRITE) {
    return getSendCriteriaInFixedPessimism();
  }
  else {
    return criteria;
  }
}

// This function should be able to call a set of functions depending
// on the criteria. currently, the ifdef's will be used
inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedSlope(KernelMessage* msg, int size) {
  // Check with Threshold curve to see, what has to be done with the
  // message
  AggregationSendCriteria criteria = getSendCriteria(msg, size);
  if(criteria == WRITE) {
    return getSendCriteriaInFixedSlope();
  }
  else {
    return criteria;
  }
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInFixedSlopeWithError(KernelMessage* msg, int size) {
  // (1) See if we can write the message
  // (2) See if it is a control message, then flush it
  // (3) See if the LP is rolling back,
  //     Then aggregate till the roll back is over.
  // (4) See if the inputQ is empty. If it is empty then flush it
  // (5) See if the message rate is the expected message rate
  // Step (1) & (2)
  AggregationSendCriteria criteria = getSendCriteria(msg, size);
  if(criteria == WRITE) {
    //step (3), (4) & (5)
    return getSendCriteriaInFixedSlopeWithError();
  }
  else {
    return criteria;
  }
}

inline 
AggregationSendCriteria
MessageManager::getSendCriteriaInMeanOfFactors(KernelMessage* msg, int size) {
  // The strategy is as follows
  // (1) send if there is no space
  // (2) If it is a control message do accordingly
  // (3) If the time is less then the calculated maximum age then aggregate
  // Step (1) and (2)
  AggregationSendCriteria criteria = getSendCriteria(msg, size);
  if(criteria == WRITE) {
     //step (3)
     return getSendCriteriaInMeanOfFactors();
  }
  else {
     return criteria;
  }
}

inline 
AggregationReceiveCriteria
MessageManager::getReceiveCriteria() {
  //Check with the age count to see when to receive the message

  if((receive_buffer->numberOfMsgs <= 0) &&
     (receiveCriterion >= maxReceiveDelay)) {
     return RECEIVE;
  }
  else {
     return NO_NEED_TO_RECEIVE;
  }
}

inline void 
MessageManager::writeMessage(SerializedInstance* msg, int mylpId) {
  send_buffer[mylpId]->writeMessage(msg);

  incrementNumberOfMessagesToBeSent();
  setAgeIncrementor(1);
}

inline 
void 
MessageManager::checkToReceive() {
  AggregationReceiveCriteria whatToDo;
  
  whatToDo = getReceiveCriteria();
  switch(whatToDo) {
  case RECEIVE:
    receive_buffer->receiveMessage();
    resetReceiveCriterionInfo();
    break;
  case NO_NEED_TO_RECEIVE:
    break;
  };
}

void 
MessageManager::sendMessage() {
  for(int i = 0; i < numberOfLps; i++) {
     if((i != lpId) && (send_buffer[i]->numberOfMsgs > 0)) {
        send_buffer[i]->sendMessage();
     }
  }
  resetSendCriterionInfo();
}

inline 
void 
MessageManager::checkToSend() {

  AggregationSendCriteria whatToDo;
  
  whatToDo = getSendCriteriaInFixedPessimism();
  switch(whatToDo) {
  case SEND:
    sendMessage();
    break;
  default:
    break;
  }

}

void
MessageManager::incrementAgeOfMessage() {
  ageOfMessage = ageOfMessage + ageIncrementor;
}

void 
MessageManager::sendMessage(KernelMessage* msg, int dest) {

  AggregationSendCriteria whatToDo;

  SerializedInstance *toSend = msg->serialize();
  
  whatToDo = getSendCriteriaInFixedPessimism(msg, toSend->getSize());

  switch(whatToDo) {
  case SEND_AND_WRITE:
    sendMessage();
    writeMessage(toSend, dest);
    break;
  case WRITE_AND_SEND:
  case SEND:
    writeMessage(toSend, dest);
    sendMessage();
    break;
  case WRITE:
    writeMessage(toSend, dest);
    break;
  case SEND_WRITE_SEND:
    sendMessage();
    writeMessage(toSend, dest);
    sendMessage();
    break;
  case DO_NOT_SEND:
    writeMessage(toSend, dest);
    break;
  default:
    cerr << "Invalid aggregation criterion received, writing the message anyway";
    cerr << " " << whatToDo << endl;
    writeMessage(toSend, dest);
    break;
  }

  incrementReceiveCriterion();
  checkToReceive();
  delete toSend;
}

SerializedInstance *
MessageManager::readMessage() {
  return receive_buffer->readMessage();
}

SerializedInstance *
MessageManager::receiveMessage() {

  SerializedInstance *msg = NULL;
  
  if(receive_buffer->numberOfMsgs > 0) {
    msg = readMessage();
  }
  else {
    receive_buffer->receiveMessage();
    if(receive_buffer->numberOfMsgs > 0) {
      msg = readMessage();
      resetReceiveCriterionInfo();
    }
    else {
      checkToSend();
    }
  }
  return msg;
}

std::ostream&
operator<<(std::ostream &os, const AggregationSendCriteria& asc) {
  os << " ";
  switch (asc) {
  case SEND:
    os << "Send";
    break;
  case WRITE:
    os << "Write";
    break;
  case SEND_AND_WRITE:
    os << "SendAndWrite";
    break;
  case WRITE_AND_SEND:
    os << "WriteAndSend";
    break;
  case DO_NOT_SEND:
    os << "DoNotSend";
    break;
  case SEND_WRITE_SEND:
    os << "SendWriteSend";
    break;
  case DO_NOT_KNOW:
    os << "DoNotKnow";
    break;
  default:
    os << "[Error!!]";
  }

  os << " ";

  return os;
}
    
