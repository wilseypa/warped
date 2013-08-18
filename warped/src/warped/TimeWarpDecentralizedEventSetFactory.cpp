
#include "TimeWarpDecentralizedEventSetFactory.h"
#include "TimeWarpSimulationManager.h"
#include "SchedulingData.h"
#include "SimulationConfiguration.h"

using std::cerr;
using std::endl;

TimeWarpDecentralizedEventSetFactory::TimeWarpDecentralizedEventSetFactory( TimeWarpSimulationManager * ){}

Configurable *
TimeWarpDecentralizedEventSetFactory::allocate( SimulationConfiguration &configuration,
						Configurable * ) const {
  // the following case is possible:

  // (1) following the event set choice, the word "all" is specified
  // meaning that a separate event set is to be maintained for each
  // simulation object.


  // first check to see if it is case (1) or case (2).
//   unsigned int pos = configurationMap["EVENTLIST"].find(":");
//   if(pos != string::npos){
//     // yes, we are in case (2).
//     // we are going to instantiate one queue for each simulation object
//     // and we better not be performing a sequential simulation
//     choice = configurationMap["EVENTLIST"].substr(0,pos);
//   }
  
//  SimulationManager *mySimulationManager = dynamic_cast<SimulationManager *>(parent);
  //  int numberOfObjectQueues = mySimulationManager->getNumberOfSimulationObjects();

  // Since control is here, the configurable class HAS TOBE
  // TimeWarpReceiverQueue
  //  TimeWarpReceiverQueue *myConfigurableObject = dynamic_cast<TimeWarpReceiverQueue *>(myEventSet);
   
  //  ASSERT(myConfigurableObject != 0);
  //  myConfigurableObject->schedulingData = mySimulationManager->getSchedulingData();
  if( configuration.eventListOrganizationIs( "DECENTRALIZED" ) ){
    cerr << "Decentralized lists are currently broken!" << endl;
    abort();
  }
//     if( configuration["CONTROLMANAGER"] == "ACTIVE" ) {
//       for(unsigned int count = 0; count < numberOfObjectQueues; count++) {
// 	ReceiverQueueContainer *receiverQContainer = new ReceiverQueueContainer();
// 	receiverQContainer->eventSet = new TimeWarpMultiSetWithControl(mySimulationManager);
// 	myConfigurableObject->myReceiverQueue.push_back(receiverQContainer);
// 	ScheduleListContainer *scheduleListContainer = 
// 	  new ScheduleListContainer(&receiverQContainer->headEventPtr);
// 	myConfigurableObject->schedulingData->scheduleList->push_back(scheduleListContainer);
//       }
//       DEBUG (
// 	     if(configurationMap["DEBUG"] == "TRUE"){
// 	       cout << "Configured a TimeWarpMultiSetWithControl as the event set"
// 		    << endl;
// 	     }
// 	     );
//     }
//     else{
//       for(unsigned int count = 0; count < numberOfObjectQueues; count++) {
// 	ReceiverQueueContainer* receiverQContainer = new ReceiverQueueContainer();
// 	receiverQContainer->eventSet = new TimeWarpMultiSet();
// 	myConfigurableObject->myReceiverQueue.push_back(receiverQContainer);
// 	ScheduleListContainer* scheduleListContainer = new ScheduleListContainer(&receiverQContainer->headEventPtr);
// 	myConfigurableObject->schedulingData->scheduleList->push_back(scheduleListContainer);
//       }
//       DEBUG (
// 	     if(configurationMap["DEBUG"] == "TRUE"){
// 	       cout << "Configured a TimeWarpMultiSet as the event set" << endl;
// 	     }
// 	     );
//     }
//   }
//   else if (selected == "APPENDQ"){
//     for(unsigned int count = 0; count < numberOfObjectQueues; count++) {
//       ReceiverQueueContainer* receiverQContainer = new ReceiverQueueContainer();
//       receiverQContainer->eventSet = new TimeWarpAppendQueue();
//       (myConfigurableObject->myReceiverQueue).push_back(receiverQContainer);
//       ScheduleListContainer* scheduleListContainer = new ScheduleListContainer(&receiverQContainer->headEventPtr);
//       myConfigurableObject->schedulingData->scheduleList->push_back(scheduleListContainer);
//     }
//     DEBUG (
// 	   if(configurationMap["DEBUG"] == "TRUE"){
// 	     cout << "Configured a TimeWarpAppendQueue as the event set" << endl;
// 	   }
// 	   );
//   }
//   else {
//     cerr << "Unknown EVENTLIST choice encountered" << endl;
//     cerr << "Exiting simulation ... " << endl;
//     exit(-1);
//   }

  return 0;
}
