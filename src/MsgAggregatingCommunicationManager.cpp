
#include "TimeWarpSimulationManager.h"
#include "MsgAggregatingCommunicationManager.h"

MsgAggregatingCommunicationManager::MsgAggregatingCommunicationManager( PhysicalCommunicationLayer *physicalLayer, 
									TimeWarpSimulationManager *simMgr ) :
  CommunicationManagerImplementationBase(physicalLayer, simMgr),
  waitedWithNoInputMessages(0){
  unsigned int numSimMgrs = mySimulationManager->getNumberOfSimulationManagers();
  myMessageManager = new MessageManager(numSimMgrs, this, mySimulationManager,
					physicalLayer);
};

MsgAggregatingCommunicationManager::~MsgAggregatingCommunicationManager(){
  delete myMessageManager;
};

void
MsgAggregatingCommunicationManager::initializeCommunicationManager(){}

void
MsgAggregatingCommunicationManager::sendMessage(KernelMessage *msg,
                                                unsigned int dest){
  myMessageManager->sendMessage(msg, dest);
  delete msg;
}

SerializedInstance *
MsgAggregatingCommunicationManager::retrieveMessageFromPhysicalLayer(){
  return myMessageManager->receiveMessage();
}

unsigned int
MsgAggregatingCommunicationManager::checkPhysicalLayerForMessages(int maxNum){
  int numberOfMessagesReceived = 0;
  bool noMessagesLeft = false;
  SerializedInstance *msg = NULL;

  while(numberOfMessagesReceived != maxNum && noMessagesLeft == false){
    msg = retrieveMessageFromPhysicalLayer();
    if(msg == NULL){
      // there are no messages at this time
      noMessagesLeft = true;
      waitedWithNoInputMessages++;
    }
    else {
      // there are messages to pick up
      numberOfMessagesReceived++;
      waitedWithNoInputMessages = 0;
         
      // deserialize the message and route it to the appropriate
      // receiver
      routeMessage(dynamic_cast<KernelMessage *>(msg->deserialize()));
      // since we allocated it, we delete it
      delete msg;
    }
  }// end while loop
   
  return numberOfMessagesReceived;
}


