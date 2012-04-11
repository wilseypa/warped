// See copyright notice in file Copyright in the root directory of this archive.

#include "SerializedInstance.h"
#include "TimeWarpSimulationManager.h"
#include "ThreadedTimeWarpSimulationManager.h"
#include "CommunicationManagerImplementationBase.h"
#include "TerminateToken.h"
#include "StartMessage.h"
#include "InitializationMessage.h"
#include "CirculateInitializationMessage.h"
#include <sstream>

using std::istringstream;
using std::ostringstream;
using std::cerr;
using std::endl;

CommunicationManagerImplementationBase::CommunicationManagerImplementationBase( PhysicalCommunicationLayer *physicalLayer, 
										TimeWarpSimulationManager *simMgr) :
  CommunicationManager( physicalLayer, simMgr ), 
  mySimulationManager(simMgr){
}

void
CommunicationManagerImplementationBase::initializeCommunicationManager(){}
   
void
CommunicationManagerImplementationBase::sendMessage( KernelMessage *message,
                                                     unsigned int dest){
  message->setIncarnationNumber(numCatastrophicRollbacks);

  if(!recoveringFromCkpt){
    myPhysicalCommunicationLayer->physicalSend( message->serialize(), 
                                                dest );
  }
  else{
    // When recovering from rollback, only send the restore messages.
    if(message->getDataType() == "RestoreCkptMessage"){
      myPhysicalCommunicationLayer->physicalSend( message->serialize(),
                                                  dest );
    }
  }
  delete message;
}


void
CommunicationManagerImplementationBase::routeMessage( KernelMessage *msg ){
  ASSERT(msg != NULL);
  string messageType = msg->getDataType();
  
  if(recoveringFromCkpt){
    if( msg->getIncarnationNumber() < numCatastrophicRollbacks){
      delete msg;
      return;
    }
  }

  // It's not a CHECK-IDLE MESSAGE.
  // look-up and call the appropriate receiver for this message
  //map::find returns an iterator pair, (Key, Value), the second selects the value
  CommunicatingEntity *receiver = (*(listOfReceivers.find(messageType))).second;
 
  if( receiver == 0 ){
    cerr << "CommunicationManagerImplementationBase::routeMessage - " <<
      "don't know how to handle a message of type " << messageType << "." << endl;
    abort();
  }
  else{
      receiver->receiveKernelMessage(msg);
  }
}

SerializedInstance *
CommunicationManagerImplementationBase::retrieveMessageFromPhysicalLayer(){
  return myPhysicalCommunicationLayer->physicalProbeRecv();
}

unsigned int
CommunicationManagerImplementationBase::checkPhysicalLayerForMessages(int maxNum){
  int numberOfMessagesReceived = 0;
  bool noMessagesLeft = false;

  while(numberOfMessagesReceived != maxNum && noMessagesLeft == false){
    SerializedInstance *msg = retrieveMessageFromPhysicalLayer();
    if(msg == NULL){
      // there are no messages at this time
      noMessagesLeft = true;
    }
    else {
      // there are messages to pick up
      numberOfMessagesReceived++;
      // deserialize the message and route it to the appropriate
      // receiver
      KernelMessage *kMsg = dynamic_cast<KernelMessage *>(msg->deserialize());
      delete msg;
      routeMessage( kMsg );
    }
  }// end while loop
   
  return numberOfMessagesReceived;
}

void
CommunicationManagerImplementationBase::sendStartMessage(unsigned int myID){

  unsigned int numSimMgrs =
    mySimulationManager->getNumberOfSimulationManagers();

  // send everybody except ourselves the start message
  for(unsigned int count = 0; count < numSimMgrs; count++){
    if(myID != count){
      KernelMessage *messageToSend =
	new StartMessage( myID, count );
      sendMessage(messageToSend, count);
    }
  }
}

void
CommunicationManagerImplementationBase::waitForStart(){
  bool finished = false;
  KernelMessage *msg = NULL;
  while( !finished ){
    SerializedInstance *networkMessage = retrieveMessageFromPhysicalLayer();
    if( networkMessage != 0 ){
      msg = dynamic_cast<KernelMessage *>(networkMessage->deserialize());
      
      delete networkMessage;
      
      if( msg->getDataType() == "StartMessage" ){
	finished = true;
      }
      routeMessage(msg);
    }
  }
}

void
CommunicationManagerImplementationBase::waitForInitialization(unsigned int numExpected){
  unsigned int numReceived = 0;

  vector<string> *objectNames =
    mySimulationManager->getSimulationObjectNames();

  unsigned int numberOfSimulationManagers =
    mySimulationManager->getNumberOfSimulationManagers();

  unsigned int mySimulationManagerID =
    mySimulationManager->getSimulationManagerID();


  // send everybody execept ourselves the init message
  for(unsigned int count = 0; count < numberOfSimulationManagers; count++){
    if(mySimulationManagerID != count){
      KernelMessage *messageToSend =
	new InitializationMessage( mySimulationManagerID, 
				   count,
				   *objectNames,
				   numberOfSimulationManagers);
      sendMessage(messageToSend, count);
    }
  }
  delete objectNames;

  bool circulate = false;  
  if (numExpected==0)
  {
	  circulate=true;
  }
  // now wait to hear from others
  while(numReceived != numExpected ||
	(mySimulationManagerID != 0 && circulate == false)){
    SerializedInstance *networkMessage = retrieveMessageFromPhysicalLayer();
    if(networkMessage != NULL){
      KernelMessage *msg = dynamic_cast<KernelMessage *>(networkMessage->deserialize());
      if( msg != NULL ){
	if( msg->getDataType() == getInitializationMessageType() ){
	  numReceived++;
	  routeMessage(msg);
	}
	else if( msg->getDataType() == getCirculateInitializationMessageType() ){
	  circulate = true;
	}
	else{
	  // we didn't find an initialization message, but we need to
	  // check for other types of messages
	  routeMessage(msg);
	}
      }
      delete networkMessage;
    }
  }// end of while loop

  // Now we need to circulate a message to synchronize all communication managers and
  // make sure they are in the same "phase". When comm mgr 0 sends this, all other
  // comm mgrs are still stuck in the while loop above.

  KernelMessage *messageToSend = new CirculateInitializationMessage( mySimulationManagerID,
        (mySimulationManagerID+1) % numberOfSimulationManagers );
  sendMessage(messageToSend, (mySimulationManagerID+1) % numberOfSimulationManagers);

  
  // If we are simulation manager 0 we have to catch the CirculateInitializationMessage.
  while (circulate == false) {
    SerializedInstance *networkMessage = retrieveMessageFromPhysicalLayer();
    if(networkMessage != NULL){
      KernelMessage *msg = dynamic_cast<KernelMessage *>(networkMessage->deserialize());
      
      if (msg != NULL && msg->getDataType() == getCirculateInitializationMessageType() ){
	circulate = true;
      }
      else if (msg != NULL){
	// we didn't find an initialization message, but we need to
	// check for other types of messages
	routeMessage(msg);
      }
      delete networkMessage;
    }
  }// end of while loop
}

const string &
CommunicationManagerImplementationBase::getInitializationMessageType(){
  static const string messageType = "InitializationMessage";
  return messageType;
}

const string &
CommunicationManagerImplementationBase::getCirculateInitializationMessageType(){
  static const string messageType = "CirculateInitializationMessage";
  return messageType;
}

const string &
CommunicationManagerImplementationBase::getCheckIdleMessageType(){
  static const string messageType = "CheckIdleMessage";
  return messageType;
}
