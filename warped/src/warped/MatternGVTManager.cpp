// See copyright notice in file Copyright in the root directory of this archive.

#include "MatternGVTManager.h"
#include "SchedulingManager.h"
#include "CommunicationManager.h"
#include "TimeWarpSimulationManager.h"
#include "MatternGVTMessage.h"
#include "GVTUpdateMessage.h"
#include "LazyOutputManager.h"
#include "ObjectID.h"
#include <sstream>
using std::istringstream;
using std::ostringstream;
using std::cout;
using std::cerr;
using std::endl;

MatternGVTManager::MatternGVTManager(TimeWarpSimulationManager *simMgr,
                                     unsigned int period) :
  GVTManagerImplementationBase(simMgr, period),
  objectRecord(new MatternObjectRecord()),
  gVTTokenPending(false)
{
  ASSERT( mySimulationManager != NULL );
  ASSERT( mySimulationManager->getSchedulingManager() != NULL );
  myScheduler = mySimulationManager->getSchedulingManager();
  ASSERT( mySimulationManager->getCommunicationManager() != NULL );
  myCommunicationManager = mySimulationManager->getCommunicationManager();
}

MatternGVTManager::MatternGVTManager(TimeWarpSimulationManager *simMgr,
                                     unsigned int period, bool objectRecordDefined) :
  GVTManagerImplementationBase(simMgr, period),
  gVTTokenPending(false)
{
  ASSERT( mySimulationManager != NULL );
  ASSERT( mySimulationManager->getSchedulingManager() != NULL );
  myScheduler = mySimulationManager->getSchedulingManager();
  ASSERT( mySimulationManager->getCommunicationManager() != NULL );
  myCommunicationManager = mySimulationManager->getCommunicationManager();
}


MatternGVTManager::~MatternGVTManager() {
  delete objectRecord;
}

bool MatternGVTManager::checkGVTPeriod() {
  if (!gVTTokenPending) {
    if(++gVTPeriodCounter == gVTPeriod){
      gVTPeriodCounter = 0;
      return true;
    }
  }
  return false;
}

// This is called before sending any event to keep track of the LTSE
void MatternGVTManager::calculateGVTInfo(const VTime &receiveTime) {
  abort();
}

// This is called before sending an event to a remote simulation manager
const string MatternGVTManager::getGVTInfo(unsigned int srcSimMgr, unsigned int destSimMgr,
                                           const VTime &sendTime) {
  //The timestamp information will be sent with the event, the color
  //is the only information stored in GVTInfo
  int tokenColor = objectRecord->getColor();
  char c[2];
  sprintf(c,"%d", tokenColor);

  if(tokenColor == WHITE){
    //Keep a count of all messages sent while the GVT Token is white
    objectRecord->incrementNumberOfWhiteMessages();
  } 
  else{
    //If the first round of GVT Control messages has passed our
    //color is red so we should be monitoring the LTSE
    const VTime *minimumTime = (&sendTime);
    objectRecord->setMinTimeStamp(*minimumTime);
  }
  return c;
}

//This is called when receiving a remote event
void MatternGVTManager::updateEventRecord(const string &infoStream, unsigned int receivingSimMgr){
  // for mattern only color information will be present in the
  // info stream received from the kernel
  int tokenColor = atoi(infoStream.c_str());

  if(tokenColor == WHITE) {
    objectRecord->decrementNumberOfWhiteMessages();
  }
}

void MatternGVTManager::calculateGVT() {
  // a few assumptions to state first:
  //
  // a. SimulationManager(0) is assumed to be the initiator of the
  //    gvt estimation procedure.
  //
  // b. if numberOfSimulationManagers == 1, then the gVT is simply the
  //    last scheduled event.
  ASSERT(mySimulationManager->getSimulationManagerID() == 0 );
  if (mySimulationManager->getNumberOfSimulationManagers() > 1) {
    // Okay, time to start off the gvt token.
    if (gVTTokenPending == false) {
      zeroWhiteMessagesAtStart=false;
      // Make sure there isn't a token already out there.
      const VTime *minimumTime = (&myScheduler->getLastEventScheduledTime());
      sendGVTToken(*minimumTime, mySimulationManager->getPositiveInfinity());

      objectRecord->setColor(RED);
      objectRecord->setNumberOfWhiteMessages(0);
      objectRecord->resetMinTimeStamp(mySimulationManager->getPositiveInfinity());
      gVTTokenPending = true;
    }
  } 
  else { // numberOfSimulationManagers == 1
    // okay, we dont need to launch a distributed gvt estimation step
    // gVT is simply the last scheduled event's timestamp
    const VTime *minimumTime = (&myScheduler->getLastEventScheduledTime());
    setGVT(*minimumTime);
    cout << "GVT = " << getGVT() << endl;
    mySimulationManager->fossilCollect(getGVT());
  }
}

// ----------------------------------------------------------------
// A GVTTokenMessage contains:
//   1. VTime mClock;             // minimum of the local clocks
//   2. VTime mSend;              // minimum of timestamps
//   3. unsigned int count;       // number of messages in transit
// ----------------------------------------------------------------
void MatternGVTManager::sendGVTToken(const VTime &lastScheduledEventTime,
                                     const VTime &minimumTimeStamp) {
  // My(malolan) changes here The guy, whom i am going to send to has to
  // know, how many white messages, i have sent and not the white messages
  // that, i have received so changing
  // objectRecordArray[myID].getNumberOfWhiteMessages() to
  // objectRecordArray[destination].getNumberOfWhiteMessages( send the
  // token to the next guy;
  unsigned int destination = 0;
  if (mySimulationManager->getSimulationManagerID()
      != mySimulationManager->getNumberOfSimulationManagers() - 1) {
    destination = mySimulationManager->getSimulationManagerID() + 1;
  }
  // increment the token iteration number
  objectRecord->incrementTokenIterationNumber();
  KernelMessage *messageToSend = new MatternGVTMessage(
      mySimulationManager->getSimulationManagerID(), destination,
      lastScheduledEventTime, minimumTimeStamp,
      objectRecord->getNumberOfWhiteMessages());
  ASSERT(myCommunicationManager != NULL);
  myCommunicationManager->sendMessage(messageToSend, destination);
}

// send out the latest gVT value to everyone
void MatternGVTManager::sendGVTUpdate() {
  // send everybody except ourselves the gVT update message
  for (unsigned int c = 0; c < mySimulationManager->getNumberOfSimulationManagers(); c++) {
    if (mySimulationManager->getSimulationManagerID() != c) {
      KernelMessage *messageToSend =
          new GVTUpdateMessage(mySimulationManager->getSimulationManagerID(), c, getGVT());
      ASSERT(myCommunicationManager != NULL);
      myCommunicationManager->sendMessage(messageToSend, c);
    }
  }
}

// the MatternGVTManager must register the following message types with
// the communication manager: GVTTokenMessage and GVTUpdateMessage
void MatternGVTManager::registerWithCommunicationManager() {
  ASSERT(myCommunicationManager != NULL);
  myCommunicationManager->registerMessageType(GVTUpdateMessage::getGVTUpdateMessageType(), this);
  myCommunicationManager->registerMessageType(MatternGVTMessage::getMatternGVTMessageType(), this);
}

// the communication manager will call this method to deliver a
// GVTTokenMessage or a GVTUpdateMessage.
void MatternGVTManager::receiveKernelMessage(KernelMessage *msg) {
  ASSERT(msg != NULL);
  if (dynamic_cast<MatternGVTMessage *> (msg) != 0) {
    const MatternGVTMessage *gVTMessage = dynamic_cast<MatternGVTMessage *> (msg);
    const int count = gVTMessage->getNumMessagesInTransit();

    if (mySimulationManager->getSimulationManagerID() == 0) {
      // Initiator has received the control message.
      // Check to see if the count is zero and this is at least the second
      // round of the token. Continue until the count is 0 and all messages
      // in transit are accounted for.
      if (objectRecord->getTokenIterationNumber() > 1
          && (objectRecord->getNumberOfWhiteMessages() + count == 0)){

          // Need to remember the old gvt to compare it to the new.
          const VTime &oldGVT = getGVT();

          // Determine GVT.
          setGVT(MIN_FUNC(gVTMessage->getLastScheduledEventTime(),
                 gVTMessage->getMinimumTimeStamp()));
          ASSERT(getGVT() >= oldGVT);

          // Send out the GVT update to the other simulation managers and reset to white.
          objectRecord->setTokenIterationNumber(0);
          objectRecord->setNumberOfWhiteMessages(0);
          objectRecord->setColor(WHITE);
          sendGVTUpdate();

          // End of this gvt calculation cycle.
          gVTTokenPending = false;
          
          // Only output the value and fossil collect when it actually changes.
          if(getGVT() > oldGVT){
            // Fossil collect now with the new GVT.
            cout << "GVT = " << getGVT() << endl;
            mySimulationManager->fossilCollect(getGVT());
          }
      } 
      else {
        // Not yet ready to calculate gvt, send the token around again.
        objectRecord->setNumberOfWhiteMessages(objectRecord->getNumberOfWhiteMessages() + count);
        const VTime *lowEventTime = (&myScheduler->getLastEventScheduledTime());
        sendGVTToken(MIN_FUNC(gVTMessage->getLastScheduledEventTime(), *lowEventTime),
               MIN_FUNC(gVTMessage->getMinimumTimeStamp(), *objectRecord->getMinTimeStamp()));

        // This is reset to record the number of messages received since the last round.
        objectRecord->setNumberOfWhiteMessages(0);
      }
    } 
    else {
      // The gvt token has been received by another simulation manager.
      // [a] Set color of this sim mgr to RED; set tMin = positive infinity.
      // [b] Pass on the token to processor(i mod n) + 1.
      if (objectRecord->getColor() == WHITE) {
        objectRecord->resetMinTimeStamp(mySimulationManager->getPositiveInfinity());
        objectRecord->setColor(RED);
      }

      // Add the the local white message count to the simulation's white message total.
      objectRecord->setNumberOfWhiteMessages(objectRecord->getNumberOfWhiteMessages() + count);

      const VTime *lowEventTime = (&myScheduler->getLastEventScheduledTime());
      sendGVTToken(MIN_FUNC(gVTMessage->getLastScheduledEventTime(),*lowEventTime),
                   MIN_FUNC(gVTMessage->getMinimumTimeStamp(),*objectRecord->getMinTimeStamp()));

      // This is reset to record the number of messages received since the last round.
      objectRecord->setNumberOfWhiteMessages(0);
    }
  } 
  else if (dynamic_cast<GVTUpdateMessage *> (msg) != 0) {
    const GVTUpdateMessage *gVTMessage = dynamic_cast<GVTUpdateMessage *> (msg);

    const VTime &oldGVT = getGVT();
    setGVT(gVTMessage->getNewGVT());
    ASSERT(getGVT() >= oldGVT);

    // Only fossil collect if the value has increased.
    if(getGVT() > oldGVT){
      // Fossil collect now with the new GVT.
      mySimulationManager->fossilCollect(getGVT());
    }

    // VERY IMPORTANT NOTE!!
    // The white message count is not reset here because by the time this
    // simulation manager gets the update token, it may have already received some
    // white messages from another simulation manager that already switched back
    // to white. These need to be taken into account for the next GVT calculation.
    objectRecord->setTokenIterationNumber(0);
    objectRecord->setColor(WHITE);
  } 
  else {
    cerr << "MatternGVTManager::receiveKernelMessage() received"
        << " unknown (" << msg->getDataType() << ") message type"
        << endl;
    cerr << "Aborting simulation ..." << endl;
    abort();
  }
  // We are done with this kernel message.
  delete msg;
}

const VTime *MatternGVTManager::getEarliestEventTime(const VTime *lowEventTime) {
  if (mySimulationManager->getOutputMgrType() == LAZYMGR ||
      mySimulationManager->getOutputMgrType() == ADAPTIVEMGR) {
    LazyOutputManager *lmgr = static_cast<LazyOutputManager*>(mySimulationManager->getOutputManager());
    const VTime *lazyMinTime = &lmgr->getLazyQMinTime();
    lowEventTime = &MIN_FUNC(*lazyMinTime, *lowEventTime);
  }
  return lowEventTime;
}

void
MatternGVTManager::ofcReset(){
  gVTTokenPending = false;
  gVTPeriodCounter = 0;
  objectRecord->setNumberOfWhiteMessages(0);
  objectRecord->setTokenIterationNumber(0);
  objectRecord->setColor(WHITE);
}
bool MatternGVTManager::getGVTTokenStatus(){
    return gVTTokenPending;
}
